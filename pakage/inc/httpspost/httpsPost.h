#ifndef __HTTPS_POST_H__
#define __HTTPS_POST_H__


#include "sslSocket.h"

using namespace std;

// socket的状态
typedef enum socket_state
{ 
    HTTPS_ERROR   = -1,           // 出错
    HTTPS_OK      = 0,            // 成功
    
    HTTPS_FREE,                   // 空闲状态
    HTTPS_CONNECTED,              // 已经连接
    HTTPS_CONNECT_FALSE,          // 连接失败
    
    HTTPS_SENTED,                 // 已经发送出去
    HTTPS_SENT_FALSE,             // 发送失败
    
    HTTPS_RECVING,                // 正在接收
    HTTPS_RECV_FALSE,             // 接收失败
    HTTPS_RECVED,                 // 接收成功

}https_state;


class HttpsPost
{
public:
    HttpsPost();
    ~HttpsPost();
    
    /**
    * @brief  SSLSocket连接主机的函数
    * @param  host 输入参数，主机名，可以是域名或者IP
    * @param  port 输入参数，端口号
    * @param  type 输入参数，指明主机名类型，HOSTNAME_TYPE或IP_TYPE，默认是HOSTNAME_TYPE
    * @return 是否成功连接，HTTPS_CONNECTED-成功，HTTPS_CONNECT_FALSE-失败，当前处于发送或接收数据时返回相应状态
    * @note   只需要连接一次即可，如果要重新连接，请先Close
    */
    int Connect(string& host, unsigned int port, h_type type = HOSTNAME_TYPE);
    
    /**
    * @brief  HttpsPost发送函数
    * @param  host 输入参数，URL
    * @param  port 输入参数，POST的数据内容
    * @return 是否发送成功，HTTPS_SENTED-成功，HTTPS_SENT_FALSE-失败，当前处于其他状态时返回相应状态
    * @note   只有返回HTTPS_SENTED才算发送成功
    */
    int Send(string& url, string& reqStr);
    
    /**
    * @brief  HttpsPost接收函数
    * @param  rspStr 输入参数，接收数据缓存
    * @return 是否发送成功，HTTPS_RECVED-成功，HTTPS_RECV_FALSE-失败，HTTPS_RECVING-还在接收数据，当前处于其他状态时返回相应状态
    * @note   只有返回HTTPS_RECVED才算接收成功，如果处于HTTPS_RECVING需要重复接收，直到数据接收完全，或者等到接收超时就重新开始
    */
    int Recv(string& rspStr);
    
    /**
    * @brief  HttpsPost关闭函数
    * @return 返回成功，1
    * @note   清除状态，清除缓存，关闭socket
    */
    int Close();
    
private:
    /**
    * @brief  HttpsPost HTTP头部解析函数
    * @return 是否发送成功，1-成功，0-失败
    * @note   内部使用函数
    */
    int Parse();
    
private:
    SSLSocket        sslSocket;         // 有ssl功能的socket
    
    string           _host;             // 域名或者IP
    unsigned int     _port;             // 端口
    h_type           _type;             // host的类型
    
    int              _stat;             // 记录当前状态
    string           _buf;              // 用于接收数据的缓存
    
    string           _httphead;         // 用于存放接收到的http头部
    string           _response;         // 用于存放接收到的响应内容
    
};


#endif

