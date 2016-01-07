#ifndef __HTTPS_CONFIG_H__
#define __HTTPS_CONFIG_H__


/* 是否启用SSL，1表示使用，即HTTPS，0表示不使用，即HTTP */
#define       SSL_ENABLE                1


/* 是否启动SSL调试，1表示使用，则会输出一些调试log，当然一些错误log是必然打印出来的 */
#define       SSL_DEBUG_ENABLE          0

#if   SSL_DEBUG_ENABLE
#define SSL_DEBUG(x)           (x)
#else
#define SSL_DEBUG(x)
#endif

//#SSL_ERROR(x)                  (x)


















#endif

