#include "sslSocket.h"
#include "httpsConfig.h"
#include <poll.h>
#include <signal.h>

#include <netinet/tcp.h>
#include "kl_debug.h"

/*
 *  打印证书内容
 */
void ShowCerts(SSL * ssl)
{
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl);
    if (cert != NULL)
    {
        TRACE_HTTX("数字证书信息:\n");
        
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        TRACE_HTTX("证  书: %s\n", line);
        free(line);
        
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        TRACE_HTTX("颁发者: %s\n", line);
        free(line);
        
        X509_free(cert);
    }
    else
    {
        TRACE_HTTX("无证书信息！\n");
    }
    
    if(SSL_get_verify_result(ssl) != X509_V_OK)
    {
        TRACE_HTTX("证书验证失败！\n");
    }
}

/**
 * @brief  SSLSocket构造函数
 * @param  空
 * @return 空
 * @note   
 */
SSLSocket::SSLSocket()
    :_fd( -1 ),
    _status(HTTPS_FREE),
    tcpFalseCount(0),
    sslFalseCount(0),
    ctx(NULL),
    ssl(NULL)
{
    signal(SIGPIPE, SIG_IGN);
    
    /* SSL 库初始化 */
    SSL_library_init();
    TRACE_HTTX("SSL_library_init...\n");
    
    /* 载入所有 SSL 算法 */
    OpenSSL_add_all_algorithms();
    TRACE_HTTX("OpenSSL_add_all_algorithms...\n");
    
    /* 载入所有 SSL 错误消息 */
    SSL_load_error_strings();
    TRACE_HTTX("SSL_load_error_strings...\n");
}  


/**
 * @brief  SSLSocket析构函数
 * @param  空
 * @return 空
 * @note   
 */
SSLSocket::~SSLSocket()  
{  
    Close();
}  

/**
 * @brief  SSLSocket连接主机的函数
 * @param  host 输入参数，主机名，可以是域名或者IP
 * @param  port 输入参数，端口号
 * @param  type 输入参数，主机名类型，HOSTNAME_TYPE或IP_TYPE
 * @return 是否成功连接， HTTPS_FREE-失败， HTTPS_TCP_CONNECTING-成功
 * @note   所有失败的地方，返回前都要执行Close()
 */
int SSLSocket::TcpConnect(string& host, unsigned int port, h_type type)
{
    int ret = 0;
    // 如果不是空闲状态，或不处于连接中，直接返回
    if( _status != HTTPS_FREE && _status != HTTPS_TCP_CONNECTING)
    {
        return _status;
    }
    else if(_status == HTTPS_FREE)   // 处于空闲状态时，才做这部分初始工作
    {
        struct sockaddr_in peer;
        
        bzero( &peer, sizeof(peer) );  
        peer.sin_family = AF_INET;
        peer.sin_port   = htons(port);  
        
        // 如果提供的是域名，获取IP
        if( HOSTNAME_TYPE == type )
        {
            struct hostent *hp = gethostbyname( host.c_str() );
            if ( hp == NULL )  
            {  
                TRACE_HTTX("unknow host: %s\n" , host.c_str() );  
                return HTTPS_FREE;  
            }  
            peer.sin_addr = *( ( struct in_addr * )hp->h_addr );  
        }
        else
        {
            peer.sin_addr.s_addr = inet_addr( host.c_str() );   // 由IP转换
        }

        TRACE_HTTX("connceting to %s:%d\n", inet_ntoa(peer.sin_addr), ntohs( peer.sin_port ) );  
        
        // 创建socket，并连接到服务器
        _fd = socket( AF_INET, SOCK_STREAM, 0 );  
        if ( _fd < 0 )  
        {  
            TRACE_HTTX( "socket call failed\n" );  
            return HTTPS_FREE;  
        }  
      
        // 非阻塞
        int flag = fcntl(_fd, F_GETFL, 0);
        fcntl(_fd, F_SETFL, flag | O_NONBLOCK);
        
        ret = connect( _fd, (struct sockaddr *)&peer, sizeof( peer ) );
        
        // 如果小于0，而且不是EINPROGRESS，表示连接失败
        if(ret < 0)
        {
            if(errno != EINPROGRESS)
            {
                TRACE_HTTX("tcp connect error: %s\n", strerror(errno) );
                Close();
                return HTTPS_FREE;
            }
        }
        
        // 如果==0表示连接成功
        if ( 0 ==  ret)
        {
            TRACE_HTTX("socket connect successful, immediately\n" );
            return HTTPS_TCP_CONNECTED;
        }
        
        _status = HTTPS_TCP_CONNECTING;        // 处于连接状态
        tcpFalseCount = 0;
    }
    
    // 尝试十次，5*6秒，就算失败
    ++tcpFalseCount;
    if(tcpFalseCount > 6)
    {
        Close();
        return HTTPS_FREE;
    }
    
    // 有可能还在处理中，用select等待
    struct timeval tv;
    fd_set rset, wset;
    int error = -1;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    FD_ZERO(&rset);
    FD_SET(_fd, &rset);
    wset = rset;

    TRACE_HTTX("tcp connecting: wait for select(5 S)\n");

    ret = select(_fd + 1, &rset, &wset, NULL, &tv);
    
    // 超时, 下次再尝试
    if (ret == 0)
    {
        TRACE_HTTX("tcp connecting: timeout while wait for select\n");
        _status = HTTPS_TCP_CONNECTING;
        return HTTPS_TCP_CONNECTING;
    }
    
    if(FD_ISSET(_fd, &rset) || FD_ISSET(_fd, &wset))
    {
        // 可读又可写，有可能是错误，也有可能是连接成功
        // 需要通过获取error值来判断
        socklen_t len = sizeof(error);
        if(getsockopt(_fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        {
            TRACE_HTTX("getsockopt false, tcp connet false\n");
            Close();
            return HTTPS_FREE;
        }

        if (error)
        {
            TRACE_HTTX("getsockopt, error != 0, tcp connet false\n");
            Close();
            return HTTPS_FREE;
        }
    }
    else
    {
        // 返回值ret>0，而既不可读也不可写，则是select出错了
        TRACE_HTTX("select error, tcp connet false\n");
        Close();
        return HTTPS_FREE;
    }
    
    //SetKeepAlive();
    
    TRACE_HTTX("socket connect successful, after select\n" );

    return HTTPS_TCP_CONNECTED;
}

#define     CONNECT_TRY_MAX      6
/**
 * @brief  SSLSocket连接主机的函数
 * @param  host 输入参数，主机名，可以是域名或者IP
 * @param  port 输入参数，端口号
 * @param  type 输入参数，主机名类型，HOSTNAME_TYPE或IP_TYPE
 * @return 是否成功连接，0-失败，1-成功，-1-TCP正在连接中，-2-SSL正在连接中
 * @note   所有失败的地方，返回前都要执行Close()
 */
int SSLSocket::SslConnect(string& host, unsigned int port, h_type type)  
{  
    _status = TcpConnect(host, port, type);
    
    // 只有TCP连接成功或SLL正在连接的时候才继续下去，否则返回
    if(_status != HTTPS_TCP_CONNECTED && _status != HTTPS_SSL_CONNECTING)
    {
        return _status;
    }
    else if(_status == HTTPS_TCP_CONNECTED)   // TCP刚连接上时，才进行这部分初始化工作
    {
        /* 以 SSL V2 和 V3 标准兼容方式产生一个 SSL_CTX ，即 SSL Content Text */
        ctx = SSL_CTX_new(SSLv23_client_method());
        /* 也可以用 SSLv2_client_method() 或 SSLv3_client_method() 单独表示 V2 或 V3标准 */
        if (ctx == NULL)
        {
            ERR_print_errors_fp(stderr);
            TRACE_HTTX("create SSL_CTX false\n");
            Close();
            return HTTPS_FREE;
        }

        /* 基于 ctx 产生一个新的 SSL */
        TRACE_HTTX( "SSL_CTX_new...\n" );
        ssl = SSL_new(ctx);
        if (ssl == NULL) {   
            ERR_print_errors_fp(stderr);
            TRACE_HTTX("creat SSL false\n");
            Close();
            return HTTPS_FREE;   
        }
        else
        {
            TRACE_HTTX( "SSL_new...\n" );
        }
        
        SSL_set_fd(ssl, _fd);
        SSL_set_connect_state (ssl);
        
        _status = HTTPS_SSL_CONNECTING;
        sslFalseCount = 0;
    }

    int tryCount   = 7;
    int ret = 0;
    int events = POLLIN | POLLOUT;
    
    do
    {
        ret = SSL_do_handshake(ssl);
        TRACE_HTTX( "SSL_do_handshake ...\n" );
        
        if(ret == 1)
        {
            TRACE_HTTX( "SSL_do_handshake ok ...\n" );
            ShowCerts(ssl);
            _status = HTTPS_CONNECTED;
            return HTTPS_CONNECTED;
        }
        
        int err = SSL_get_error(ssl, ret);
        if (err == SSL_ERROR_WANT_WRITE)                // 操作没完成，需要在下一次写事件中继续
        {
            events |= POLLOUT;
            events &= ~POLLIN;
        } else if (err == SSL_ERROR_WANT_READ)          // 操作没完成，需要在下一次读事件中继续   
        {
            events |= POLLIN;
            events &= ~POLLOUT;
        }
        else
        {
            ERR_print_errors_fp(stderr);
            TRACE_HTTX("SSL_do_handshake false\n" );
            Close();
            return HTTPS_FREE;
        }
        
        struct pollfd pfd;
        pfd.fd = _fd;
        pfd.events = events;
        
        TRACE_HTTX( "SSL_do_handshake, wait for poll (5S)...\n" );
        int poolRet = poll(&pfd, 1, 5000);    
        
        if(poolRet < 0) 
        {
            // 小于0表示出错，直接算失败，返回0
            TRACE_HTTX("poll return %d error events: %d errno %d %s\n", ret, pfd.revents, errno, strerror(errno));
            Close();
            return HTTPS_FREE;
        }
        else if(poolRet == 0 || pfd.revents & POLLERR)
        {
            // poolRet==0表示超时，算一次失败，
            // 或者有返回，但是是发生错误了
            // 再给多一些机会，下次再尝试

            ++sslFalseCount;
            TRACE_HTTX( "SSL_do_handshake, can not do handshake, false couter = %d\n", sslFalseCount);
            if(sslFalseCount > 6)
            {
                Close();
                return HTTPS_FREE;
            }
            
            _status = HTTPS_SSL_CONNECTING;
            return HTTPS_SSL_CONNECTING;
        }
        
        // 如果本次成功，会继续进行下一步握手
        // 因为是5次握手，只循环七次
        
    }while(tryCount--);
    
    _status = HTTPS_SSL_CONNECTING;
    return HTTPS_SSL_CONNECTING;
}
  
int SSLSocket::Send(string& buf) 
{
    int retLen       = 0;
    int sendLen      = 0;
    int bufLen       = buf.size();
    const char *pBuf = buf.c_str();
    

    if(NULL == ssl)
    {
        return -1;
    }
    
    TRACE_HTTX("\n---------------------------------------------------\n");
    TRACE_HTTX("%s\n", buf.c_str());
    TRACE_HTTX("---------------------------------------------------\n");

    /* 发消息给服务器 */
    
    while(bufLen > 0)
    {
        retLen = SSL_write(ssl, pBuf + sendLen, bufLen);

        if (retLen < 0)
        {
            // 发送被中断，或将被阻塞，或要求重发，就重新发送
            if (errno == EINTR  || errno == EWOULDBLOCK || errno == EAGAIN)
            {
                continue;
            }
            TRACE_HTTX("send false！errno:%d, %s !\n", errno, strerror(errno));
            return -1;
        }
        else
        {
            sendLen += retLen;
            bufLen  -= retLen;
            
            TRACE_HTTX("send ok, len = %d\n", retLen );
        }
    }
    
    return sendLen;
}
  
int SSLSocket::Recv(string& buf)  
{
    int len = 0;
    int selectRet = 0;
    char tempBuf[BUFSIZ] = {0};
    
    struct timeval tv;
    fd_set fs_read;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO(&fs_read);
    FD_SET(_fd, &fs_read);

    // 使用select接收数据，等待超时60时
    selectRet = select(_fd + 1, &fs_read, NULL, NULL, &tv);
    if (selectRet < 0)
    {
        TRACE_HTTX("recv: select false\n");
        return -1;
    }
    else if (0 == selectRet)
    {
        return 0;
    }
    
    len = SSL_read(ssl, tempBuf, BUFSIZ);
    
    // 如果select可以接收数据，但是又接收不到数据，认为是失败
    if (len <= 0)
    {
        TRACE_HTTX("recv: select ok, buf SSL_read false\n");
        return -1;
    }
    
    buf.append(tempBuf, len);
    TRACE_HTTX("recv data, len = %d\n", len);

    TRACE_HTTX("---------------------------------------------------\n");
    TRACE_HTTX("%s\n", tempBuf);
    TRACE_HTTX("---------------------------------------------------\n");

    return len;
}

// 1 表示连接OK，0表示失败
int SSLSocket::NetCheck()
{
    char buf[8];
    int ret = -1;
    
    if(_fd == -1)
    {
        _status = HTTPS_FREE;
        return 0;
    }
    
    // 还没断开网络，就返回-1，errno == 11 == EAGAIN,
    // 对方主动关闭后，返回为0，是因为对方发送了关闭的信号过来，变成可读了
    // 拔掉网线，一真还是返回-1，errno == 11 == EAGAIN，无法感知断网
    // 这个方法只适应于客户端，而且服务器不主动发送消息的情况
    ret = recv(_fd, buf, 0, 0);

    if( ret < 0 )
    {
        if(errno != EAGAIN)
        {
            return 0;
        }

    }
    else if(ret == 0)   // 断开了
    {
        return 0;
    }
    
    return 1;
}

int SSLSocket::SetKeepAlive()
{
    int keepAlive    = 1;
    int keepIdle     = 10;
    int keepInterval = 10;
    int keepCount    = 3;

    // 使得连接空闲时检测连接是否异常断开
    if (setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive))==-1)
    {
        TRACE_HTTX("Set SockOpt SO_KEEPALIVE failed on fd\n");
        return 0;
    }

    // 开始首次KeepAlive探测前的TCP空闭时间
    if (setsockopt(_fd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle))==-1)
    {
        TRACE_HTTX("Set SockOpt TCP_KEEPIDLE failed on fd\n");
        return 0;
    }

    // 两次KeepAlive探测间的时间间隔
    if (setsockopt(_fd, SOL_TCP, TCP_KEEPINTVL, (void*)&keepInterval, sizeof(keepInterval))==-1)
    {
        TRACE_HTTX("Set SockOpt TCP_KEEPINTVL failed on fd");
        return 0;
    }

    // 判定断开前的KeepAlive探测次数
    if (setsockopt(_fd, SOL_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(keepCount))==-1)
    {
        TRACE_HTTX("Set SockOpt TCP_KEEPCNT failed on fd");
        return 0;
    }
    
    TRACE_HTTX("SetKeepAlive successful ----\n");
    

    return 1;
}

/* 释放资源，关闭连接 */
int SSLSocket::Close()
{
    if(ssl != NULL)
    {
        SSL_shutdown( ssl );
        SSL_free( ssl );
        ssl = NULL;
    }
    
    if(ctx != NULL)
    {
        SSL_CTX_free( ctx );
        ctx = NULL;
    }

    if( _fd != -1 )
    {
        char buf[512];
        
        // 关闭写，清除读缓冲区，再关闭socket
        if (shutdown(_fd, SHUT_WR) >= 0)
        {
            recv(_fd, buf, sizeof(buf), 0);
        }
        close( _fd );
        TRACE_HTTX("socket close successful\n");
        _fd = -1;
    }
    
    _status = HTTPS_FREE;

    return 1;
}


