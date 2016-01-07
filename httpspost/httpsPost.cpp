#include "httpsPost.h"
#include "httpsConfig.h"
#include "kl_debug.h"

/**
* @brief  HttpsPost构造函数
*/
HttpsPost::HttpsPost()
    :_stat(HTTPS_FREE)
{

}


/**
* @brief  HttpsPost析构函数
*/
HttpsPost::~HttpsPost()
{
    
}


/**
* @brief  HttpsPost连接主机的函数
* @param  host 输入参数，主机名，可以是域名或者IP
* @param  port 输入参数，端口号
* @param  type 输入参数，指明主机名类型，HOSTNAME_TYPE或IP_TYPE，默认是HOSTNAME_TYPE
* @return 是否连接成功，HTTPS_CONNECTED-成功，HTTPS_CONNECT_FALSE-失败，当前处于发送或接收数据时返回相应状态
* @note   只需要连接一次即可，如果要重新连接，请先Close
*/
int HttpsPost::Connect(string& host, unsigned int port, h_type type)
{
    // 处于空闲状态时则去连接。处于连接过程中，则继续完成连接
    if( HTTPS_FREE == _stat || HTTPS_TCP_CONNECTING == _stat ||
        HTTPS_TCP_CONNECTED == _stat || HTTPS_SSL_CONNECTING == _stat )
    {
        // 保存到类内部变量
        _host = host;
        _port = port;
        _type = type;
        
        _stat = sslSocket.SslConnect(_host, _port, _type);
        
        if( HTTPS_FREE == _stat)
        {
            // 只要失败都close，原有状态还是HTTPS_FREE，返回连接失败HTTPS_CONNECT_FALSE
            sslSocket.Close();
            _stat = HTTPS_FREE;
            return HTTPS_CONNECT_FALSE;
        }
    }
    
    return _stat;
}


/**
* @brief  HttpsPost发送函数
* @param  url      输入参数，URL
* @param  reqParam 输入参数，POST的数据内容
* @return 是否发送成功，HTTPS_SENTED-成功，HTTPS_SENT_FALSE-失败，当前处于其他状态时返回相应状态
* @note   只有返回HTTPS_SENTED才算发送成功
*/
int HttpsPost::Send(string& url, string& reqParam)
{
    // 只有处于连接状态时才发送数据
    if( HTTPS_CONNECTED == _stat )
    {
        string strHttp;
        char paramLen[64];
        char strPort[16];
        
        // 组装HTTP报文
        strHttp += "POST " + url + " HTTP/1.1\r\n";      // 请求行
        
        // 请求头部
        sprintf(strPort, "%d", _port);
        strHttp += "Host:" + _host + ":" + strPort + "\r\n";
        //strHttp += "User-agent:Mozilla/4.0\r\n";
        //strHttp += "Accept-language:zh-cn\r\n";
        //strHttp += "Accept-Encoding:gzip,deflate\r\n";
        strHttp += "Content-Type: application/json;charset=utf-8\r\n";
        strHttp += "Connection:keep-alive\r\n";                                      //保持Tcp请求连接
        sprintf (paramLen,"Content-Length:%d\r\n", (int)reqParam.size());
        strHttp += paramLen;
        strHttp += "\r\n";
        
        strHttp += reqParam;                             // 请求数据
        
        // 发送数据，而且成功的字节数据与要发出去的字节数相等
        int ret = sslSocket.Send(strHttp);
        if(ret == (int)strHttp.size())
        {
            _stat = HTTPS_SENTED;           // 状态变成已经发出去
            _buf.erase();
        }
        else
        {
            // 只要失败都close，回到空闲状态HTTPS_FREE，返回发送失败HTTPS_SENT_FALSE
            sslSocket.Close();
            _stat = HTTPS_FREE;
            return HTTPS_SENT_FALSE;
        }
    }
    
    return _stat;
}


/**
* @brief  HttpsPost接收函数
* @param  rspStr 输入参数，接收数据缓存
* @return 是否发送成功，HTTPS_RECVED-成功，HTTPS_RECV_FALSE-失败，HTTPS_RECVING-还在接收数据，当前处于其他状态时返回相应状态
* @note   只有返回HTTPS_RECVED才算接收成功，如果处于HTTPS_RECVING需要重复接收，直到数据接收完全，或者等到接收超时就重新开始
*/
int HttpsPost::Recv(string& rspStr)
{
    // 只有数据发出去之后才需要接收数据
    if( HTTPS_SENTED == _stat || HTTPS_RECVING == _stat )
    {
        _stat = HTTPS_RECVING;           // 正在接收数据
        
        // 从socket中接收数据，数据不一定是完整的
        int ret = sslSocket.Recv(_buf);
        
        if(ret < 0)
        {
            // 只要失败都close，回到空闲状态HTTPS_FREE，返回接收失败HTTPS_RECV_FALSE
            sslSocket.Close();
            _stat = HTTPS_FREE;
            TRACE_HTTX("socket recv error, close socket, return HTTPS_RECV_FALSE\n" );
            return HTTPS_RECV_FALSE;
        }
        else if(ret > 0)
        {
            /*
             * 接收到数据，并解析成功
             * 回到已经连接状态，清空缓存，准备下一次发送数据
             * 返回接收成功HTTPS_RECVED
             */
            int retParse = Parse();
            if( retParse > 0 )
            {
                _stat = HTTPS_CONNECTED;
                _buf.erase();
                rspStr = _response;
                
                return HTTPS_RECVED;
            }
            else if(retParse == -1)  // 接收错误
            {
                sslSocket.Close();
                _stat = HTTPS_FREE;
                _buf.erase();
                TRACE_HTTX("buf parse error, close socket, return HTTPS_RECV_FALSE\n" );
                return HTTPS_RECV_FALSE;
            }
        }
        else
        {
            // 没有接收到数据
            // 如果一定时间内没有接收到到数据，上层应该根据超时时间断开连接
        }
    }

    return _stat;
}


/**
* @brief  HttpsPost HTTP头部解析函数
* @return 是否发送成功，1-成功，0-失败，-1-错误
* @note   内部使用函数
*/
int HttpsPost::Parse()
{
    _httphead.erase();
    _response.erase();

#if 1
    /* 找最开始的HTTP, 之前的数据全部清除掉 */
    string strHTTP = "HTTP";
    size_t httpPos  = _buf.find(strHTTP);
    if( httpPos != string::npos )
    {
        _buf.erase(0, httpPos);
    }
    else   // 没有找到，就继续接收
    {
        return 0;
    }
#endif


    size_t    bufLen = _buf.size();
    const char* pbuf = _buf.c_str();

    /* 需要找到连接的四个字节表示头部结束：\r\n\r\n */
    string strHeadEnd = "\r\n\r\n";
    size_t headEndPos  = _buf.find(strHeadEnd);
    if( headEndPos != string::npos )
    {
        // 把http头部内容保存到缓存
        headEndPos += 4;
        _httphead.append(_buf.c_str(), headEndPos);
    }
    else   // 没有找到，就继续接收
    {
        return 0;
    }

    // 打印头部
    //TRACE_HTTX("head: %s\n", _httphead.c_str() );

#if 1
    /* 如果找不到"200 OK", 找出头部错误的代码信息，并返回错误 */
    string str200OK = "200 OK";
    if( _httphead.find(str200OK) == string::npos )
    {
        string::size_type endPos;
        string strEro;
        if( (endPos = _httphead.find("\r\n")) != string::npos )
        {
            strEro.append(_httphead.c_str() + 8, endPos - 8);
            TRACE_HTTX("HTTP revc error info: %s\n", strEro.c_str() );
        }

        return -1;
    }
#endif

    /*
    *  在收到的缓存中找Content-Length字段，确定后面数据内容的长度
    */
    size_t bodyLen = 0;
    string::size_type pos  = 0;
    string strTemp = "Content-Length:";
    if( (pos = _httphead.find(strTemp)) != string::npos )
    {
        bodyLen = atoi( _httphead.c_str() + pos + strTemp.size() );

        // 打印长度
        // TRACE_HTTX(_httphead.c_str() + pos );
    }

    // 如果没有指定长度，则通过Body的最后的"0\r\n\r\n"来判断是否应该结束
    if(bodyLen == 0)
    {
        /* 需要找到连接的四个字节表示头部结束：0\r\n\r\n */
        string strBodyEnd = "0\r\n\r\n";
        size_t bodyEndPos  = _buf.find(strBodyEnd);
        if( bodyEndPos != string::npos )
        {
            // 把http头部内容保存到缓存
            bodyEndPos += 5;
            _response.append(pbuf + headEndPos, pbuf + bodyEndPos);
            return 1;
        }
        else   // 没有找到，就继续接收
        {
            return 0;
        }
    }
    else if( (bufLen - headEndPos) >= bodyLen )
    {
        // 去掉头部后，剩下的数据长度符合就保存响应数据，如果长度不符合，相当于数据不全
        // 如果上面找不到长度，则bodyLen为0，把所有数据都返回

        _response.append(pbuf + headEndPos, bufLen - headEndPos);

        return 1;         // 成功
    }


    return 0;        // 没找到头部，或者是响应数据不全
}

/**
* @brief  HttpsPost获取状态函数
* @param  void
* @return https_state
* @note   
*/
int HttpsPost::State()
{
    if(sslSocket.NetCheck() == 0)
    {
        TRACE_HTTX("check state, server may be close, so close\n" );
        Close();
    }
    
    return _stat;
}
    
/**
* @brief  HttpsPost关闭函数
* @return 返回成功，1
* @note   清除状态，清除缓存，关闭socket
*/
int HttpsPost::Close()
{
    _stat = HTTPS_FREE;
    _buf.erase();
    _httphead.erase();
    _response.erase();

    sslSocket.Close();
    
    return 1;
}











