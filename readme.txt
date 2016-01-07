这是一个与变电产品部合作的项目，主要功能是实现https post功能，用于充电桩向web server请求数据，并接收响应数据。




///////////////////////////////////////////////////////////////////////////
使用方法：
1、进入openssl编译，libcrypto.a、libssl.a
2、进入httpspost编译，得到libhttpspost_arm.so
3、进入test编译，得到exsample_arm
4、把libhttpspost_arm.so和exsample_arm上传到终端
5、设置环境变量LD_LIBRARY_PATH
6、执行exsample_arm



/////////////////////////////////////////////////////////////////////////////
目录说明：

doc:
存放技术文档

openssl:
是openssl库文件，分为x86版和arm版，分别进入make即可得到libcrypto.a、libssl.a



httpspost:
是httpspost的源码，封装了库openssl，加上了HTTP头部，维护socket连接等。
进入目录，make,默认是编译arm版本,libhttpspost_arm.so
使用make CC=gcc，编译x86版本，libhttpspost_x86.so



test:
是测试代码，exsample.cpp
进入目录，make,默认是编译arm版本,exsample_arm
使用make CC=gcc，编译x86版本，exsample_x86


pakage:
是提供给变电部门的包，包括头文件以及动态库


curl：
是http库，但没用使用到，暂时存放，以备用
























