#ifndef __SSL_SOCKET_H__
#define __SSL_SOCKET_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>

#include "openssl/ssl.h"
#include "openssl/err.h"

using namespace std;

// 主机名的类型，域名或者IP
typedef enum host_type
{ 
    HOSTNAME_TYPE = 0,      // 域名
    IP_TYPE                 // IP
}h_type;


/*
 *  SSLSocket类
 */
class SSLSocket
{
public:
    SSLSocket();
    ~SSLSocket();
    
    
    /**
    * @brief  SSLSocket连接主机的函数
    * @param  host 输入参数，主机名，可以是域名或者IP
    * @param  port 输入参数，端口号
    * @param  type 输入参数，指明主机名类型，HOSTNAME_TYPE或IP_TYPE
    * @return 是否成功连接，0-失败，1-成功
    * @note   
    */
    int Connect(string& host, unsigned int port, h_type type = HOSTNAME_TYPE);
    int Send(string& buf);
    int Recv(string& buf);
    int Close();

private:
    
    int     _fd;
    SSL_CTX *ctx;
    SSL *ssl;
};


#endif

