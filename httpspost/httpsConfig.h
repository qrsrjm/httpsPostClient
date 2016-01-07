#ifndef __HTTPS_CONFIG_H__
#define __HTTPS_CONFIG_H__


/* 是否启用SSL，1表示使用，即HTTPS，0表示不使用，即HTTP */
#define       SSL_ENABLE                1


/* 是否启动SSL调试，1表示使用，则会输出一些调试log，当然一些错误log是必然打印出来的 */
#define       SSL_DEBUG_ENABLE          1

#if   SSL_DEBUG_ENABLE
#define SSL_DEBUG(x)           (x)
#else
#define SSL_DEBUG(x)
#endif

//#SSL_ERROR(x)                  (x)



// socket的状态
typedef enum socket_state
{ 
    HTTPS_ERROR   = -1,           // 出错
    HTTPS_OK      = 0,            // 成功
    
    HTTPS_FREE,                   // 空闲状态
    HTTPS_TCP_CONNECTING,         // TCP连接中
    HTTPS_TCP_CONNECTED,          // TCP已经连接
    HTTPS_SSL_CONNECTING,         // SSL连接中
    HTTPS_CONNECTED,              // 已经连接，包括TCP、SSL
    HTTPS_CONNECT_FALSE,          // 连接失败
    
    HTTPS_SENTED,                 // 已经发送出去
    HTTPS_SENT_FALSE,             // 发送失败
    
    HTTPS_RECVING,                // 正在接收
    HTTPS_RECV_FALSE,             // 接收失败
    HTTPS_RECVED,                 // 接收成功

}https_state;















#endif

