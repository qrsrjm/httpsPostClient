#include "../httpspost/httpsPost.h"
#include <dlfcn.h>

void *handle = NULL;
/*
static void  loadLib(void) __attribute__((constructor));
static void  loadLib(void)
{
    printf("loadLib \n");
    handle = dlopen("libhttpspost_x86.so", RTLD_NOW);
}
*/

int main()
{

#if 1

    //string ip = "service.evchong.com";
    string ip = "123.59.53.85";
    string buf;
    string url;
    string param;
    HttpsPost httpspost;
    
    do
    {
        //int ret = httpspost.Connect(ip, 443, HOSTNAME_TYPE);
        int ret = httpspost.Connect(ip, 443, IP_TYPE);
        if( HTTPS_CONNECTED ==  ret)
        {
            break;
        }
        else if( HTTPS_CONNECT_FALSE ==  ret)
        {
            break;
        }
        
        // 防止死循环，设置超时或者尝试次数，close break
        
    }while(1);
    
#if 1
    int ok_couter = 0;
    int false_couter = 0;
    int connectCouter = 1;
/*
    while(1)
    {
        httpspost.State();
        ok_couter++;
        cout<< ok_couter<< endl;
        sleep(1);

    }
*/
    while(1)
    {
        // heartbeat
        url     = "https://service.evchong.com:443/evchong_API/evchong/v1/heartbeat";
        param   = "{\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"1012090561\"},\"chargePointSerialNumber\":{\"chargePointSerialNumber\":\"CSN00001\"}}";
        
        httpspost.State();
        //if(httpspost.State() == HTTPS_FREE)
        {
            do
            {
                //int ret = httpspost.Connect(ip, 443, HOSTNAME_TYPE);
                int ret = httpspost.Connect(ip, 443, IP_TYPE);
                if( HTTPS_CONNECTED ==  ret)
                {
                    connectCouter++;
                    break;
                }
                else if( HTTPS_CONNECT_FALSE ==  ret)
                {
                    break;
                }
                
                // 防止死循环，设置超时或者尝试次数，close break
                
            }while(1);
        }

        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            cout<< ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> send heartbeat" << endl;
            while(1)
            {
                int ret = httpspost.Recv(buf);
                if(HTTPS_RECVED == ret)
                {
                    ok_couter++;
                    //cout<< ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> revc heartbeat  " << endl;
                    //cout<< "response: " << endl << buf << endl;
                    break;
                }
                else if(HTTPS_RECV_FALSE == ret)
                {
                    false_couter++;
                    httpspost.Close();

                    //cout<< ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> recv heartbeat false and close" << endl;
                    break;
                }
            }
        }
        cout<<"connectCouter = "<< connectCouter << ", false_couter = " << false_couter << ", ok_couter = " << ok_couter << endl;
        sleep(65);
    }
#endif

    


#endif



#if 0
    string ip = "183.131.144.74";
    string buf;
    string url;
    string param;
    HttpsPost httpspost;
    
    do
    {
        int ret = httpspost.Connect(ip, 8443, IP_TYPE);
        if( HTTPS_CONNECTED ==  ret)
        {
            break;
        }
        else if( HTTPS_CONNECT_FALSE ==  ret)
        {
            break;
        }
        
        // 防止死循环，设置超时或者尝试次数，close break
        
    }while(1);
    

    
#if 1
{
    /*
     * 例子4
     * HttpsPost类测试，HTTP，SSL_ENABLE需要置为0
     */


        
        // deviceBoot
        url     = "https://183.131.144.74:8443/evchong_API/evchong/v1/deviceBoot";
        param   = "{\"iccid\":{\"iccid\":\"112233\"},\"chargePointModel\":{\"chargePointModel\":\"11\"},\"chargePointVendor\":{\"chargePointVendor\":\"3456\"},\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"123456\"},\"chargePointSerialNumber\":{\"chargePointSerialNumber\":\"45678\"},\"imsi\":{\"imsi\":\"xxxxx\"},\"meterType\":{\"meterType\":\"xxxxx\"},\"firmwareVersion\":{\"firmwareVersion\":\"xxxxx\"},\"meterSerialNumber\":{\"meterSerialNumber\":\"xxxxx\"}}";
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            cout<< "send deviceBoot ----------------------- " << endl;
            while(1)
            {
                int ret = httpspost.Recv(buf);
                if(HTTPS_RECVED == ret)
                {
                    cout<< "recv deviceBoot" << endl;
                    cout<< "response: " << endl << buf << endl;
                    break;
                }
                else if(HTTPS_RECV_FALSE == ret)
                {
                    httpspost.Close();
                    cout<< "recv deviceBoot false and close" << endl;
                    break;
                }
            }
        }
        
        sleep(2);

}
#endif

#if 1
{
        // heartbeat
        url     = "https://183.131.144.74:8443/evchong_API/evchong/v1/heartbeat";
        param   = "{\"chargePointSerialNumber\":{\"chargePointSerialNumber\":\"112233\"},\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"33qw34\"}}";
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            cout<< "send heartbeat ----------------------- " << endl;
            while(1)
            {
                int ret = httpspost.Recv(buf);
                if(HTTPS_RECVED == ret)
                {
                    cout<< "recv heartbeat" << endl;
                    cout<< "response: " << endl << buf << endl;
                    break;
                }
                else if(HTTPS_RECV_FALSE == ret)
                {
                    httpspost.Close();
                    cout<< "recv heartbeat false and close" << endl;
                    break;
                }
            }
        }
        
        sleep(2);
}
#endif

#if 1
{
        // authorize
        url     = "https://183.131.144.74:8443/evchong_API/evchong/v1/authorize";
        param   = "{\"idToken\":{\"idToken\":\"12345\"},\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"0004455\"}}";
        
        buf.erase();
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            cout<< "send authorize ----------------------- " << endl;
            while(1)
            {
                if(HTTPS_RECVED == httpspost.Recv(buf))
                {
                    cout<< "recv authorize" << endl;
                    cout<< "response: " << endl << buf << endl;
                    break;
                }
            }
        }
        
        sleep(2);
}
#endif

#if 0
{
        // startTrans
        url     = "https://183.131.144.74:8443/evchong_API/evchong/v1/startTrans";
        param   = "{\"idTag\":{\"idToken\":\"223344\"},\"connectorId\":1,\"meterStart\":1,\"reservationId\":1,\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"123456\"},\"timestamp\":1425289031076}";
        
        buf.erase();
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            cout<< "send startTrans ----------------------- " << endl;
            while(1)
            {
                if(HTTPS_RECVED == httpspost.Recv(buf))
                {
                    cout<< "recv startTrans" << endl;
                    cout<< "response: " << endl << buf << endl;
                    break;
                }
            }
        }
        
        sleep(2);
}
#endif

#if 0
{
        // meterValues
        url     = "https://183.131.144.74:8443/evchong_API/evchong/v1/meterValues";
        param   = "{\"connectorId\":1,\"transactionId\":1,\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"123456\"},\"values\":[{\"measurand\":\"Energy_Active_Import_Register\",\"unit\":\"Wh\",\"value\":\"xxxxx\",\"location\":\"Outlet\",\"context\":\"Sample_Periodic\",\"format\":\"Raw\",\"timestamp\":1425289652130}]}";
        
        buf.erase();
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            cout<< "send meterValues ----------------------- " << endl;
            while(1)
            {
                if(HTTPS_RECVED == httpspost.Recv(buf))
                {
                    cout<< "recv meterValues" << endl;
                    cout<< "response: " << endl << buf << endl;
                    break;
                }
            }
        }
        
        sleep(2);
}
#endif

#if 0
{
        // stopTrans
        url     = "https://183.131.144.74:8443/evchong_API/evchong/v1/stopTrans";
        param   = "{\"idTag\":{\"idToken\":\"123456\"},\"transactionId\":1,\"transactionData\":{\"values\":{\"measurand\":\"Energy_Active_Import_Register\",\"unit\":\"Wh\",\"value\":\"xxxxxx\",\"location\":\"Outlet\",\"context\":\"Sample_Periodic\",\"format\":\"Raw\",\"timestamp\":1425886218825}},\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"445566\"},\"meterStop\":1,\"timestamp\":1425886218829}";
        
        buf.erase();
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            cout<< "send stopTrans ----------------------- " << endl;
            while(1)
            {
                if(HTTPS_RECVED == httpspost.Recv(buf))
                {
                    cout<< "recv stopTrans" << endl;
                    cout<< "response: " << endl << buf << endl;
                    break;
                }
            }
        }
        
        sleep(2);
}
#endif

#if 0
{
        // statusNotify
        url     = "https://183.131.144.74:8443/evchong_API/evchong/v1/statusNotify";
        param   = "{\"connectorId\":1,\"vendorErrorCode\":\"1122\",\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"123456\"},\"errorCode\":\"GroundFailure\",\"vendorId\":\"4455\",\"info\":\"xxxxx\",\"timestamp\":1425290540330,\"status\":\"Available\"}";
        
        buf.erase();
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            cout<< "send statusNotify ----------------------- " << endl;
            while(1)
            {
                if(HTTPS_RECVED == httpspost.Recv(buf))
                {
                    cout<< "recv statusNotify" << endl;
                    cout<< "response: " << endl << buf << endl;
                    break;
                }
            }
        }
        
        sleep(2);
        


}
#endif

    httpspost.Close();
#endif
















////////////////////////////////////////////////////////////////////////////////////////

    
#if 0
{
    /*
     * 例子1
     * sslSocket类测试
     */
    SSLSocket  ssocket;
    string ip1 = "127.0.0.1";
    string buf1 = "hello, I am sslSocket";
  
    ssocket.Connect(ip1, 7838, IP_TYPE);
    ssocket.Send(buf1);
    ssocket.Recv(buf1);
    ssocket.Close();
}
#endif


#if 0
{
    /*
     * 例子2
     * HttpsPost类测试，HTTPS, 需要开server
     */
    string ip = "127.0.0.1";
    string buf;
    HttpsPost httpspost;
    if( HTTPS_CONNECTED == httpspost.Connect(ip, 7838, IP_TYPE) )
    {
        string url = "127.0.0.1";
        string param = "hehe";
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            if( HTTPS_RECVED == httpspost.Recv(buf))
            {
                cout<< "response: " << buf << endl;
            }
        }

        httpspost.Close();
    }
}
#endif



#if 0
{
    /*
     * 例子3
     * HttpsPost类测试，HTTPS，发送给微信，SSL_ENABLE需要置为1
     */
    string ip = "api.weixin.qq.com";
    string buf;
    HttpsPost httpspost;
    if( HTTPS_CONNECTED == httpspost.Connect(ip, 443, HOSTNAME_TYPE) )
    {
        string url = "https://api.weixin.qq.com/cgi-bin/menu/create?access_token=ACCESS_TOKEN";
        string param = "'button':[{'type':'click','name':'helloworld','key':'HELLO_WORLD'}]";
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            if( HTTPS_RECVED == httpspost.Recv(buf))
            {
                cout<< "response: " << buf << endl;
            }
        }

        httpspost.Close();
    }
}
#endif


#if 0
{
    /*
     * 例子4
     * HttpsPost类测试，HTTP，SSL_ENABLE需要置为0
     */
    string ip = "183.131.144.74";
    string buf;
    HttpsPost httpspost;
    if( HTTPS_CONNECTED == httpspost.Connect(ip, 8080, IP_TYPE) )
    {
        string url;
        string param;
        
        // deviceBoot
        url     = "http://183.131.144.74:8080/evchong_API/evchong/v1/deviceBoot";
        param   = "{\"iccid\":{\"iccid\":\"112233\"},\"chargePointModel\":{\"chargePointModel\":\"11\"},\"chargePointVendor\":{\"chargePointVendor\":\"3456\"},\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"123456\"},\"chargePointSerialNumber\":{\"chargePointSerialNumber\":\"45678\"},\"imsi\":{\"imsi\":\"xxxxx\"},\"meterType\":{\"meterType\":\"xxxxx\"},\"firmwareVersion\":{\"firmwareVersion\":\"xxxxx\"},\"meterSerialNumber\":{\"meterSerialNumber\":\"xxxxx\"}}";
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            if( HTTPS_RECVED == httpspost.Recv(buf))
            {
                //cout<< "response: " << buf << endl;
            }
        }
        
        // heartbeat
        url     = "http://183.131.144.74:8080/evchong_API/evchong/v1/heartbeat";
        param   = "{\"chargePointSerialNumber\":{\"chargePointSerialNumber\":\"112233\"},\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"33qw34\"}}";
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            if( HTTPS_RECVED == httpspost.Recv(buf))
            {
                //cout<< "response: " << buf << endl;
            }
        }
        
        // authorize
        url     = "http://183.131.144.74:8080/evchong_API/evchong/v1/authorize";
        param   = "{\"idToken\":{\"idToken\":\"12345\"},\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"0004455\"}}";
        
        buf.erase();
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            if( HTTPS_RECVED == httpspost.Recv(buf))
            {
                // cout<< "response: " << buf << endl;
            }
        }
        
        // startTrans
        url     = "http://183.131.144.74:8080/evchong_API/evchong/v1/startTrans";
        param   = "{\"idTag\":{\"idToken\":\"223344\"},\"connectorId\":1,\"meterStart\":1,\"reservationId\":1,\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"123456\"},\"timestamp\":1425289031076}";
        
        buf.erase();
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            if( HTTPS_RECVED == httpspost.Recv(buf))
            {
                // cout<< "response: " << buf << endl;
            }
        }
        
        // meterValues
        url     = "http://183.131.144.74:8080/evchong_API/evchong/v1/meterValues";
        param   = "{\"connectorId\":1,\"transactionId\":1,\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"123456\"},\"values\":[{\"measurand\":\"Energy_Active_Import_Register\",\"unit\":\"Wh\",\"value\":\"xxxxx\",\"location\":\"Outlet\",\"context\":\"Sample_Periodic\",\"format\":\"Raw\",\"timestamp\":1425289652130}]}";
        
        buf.erase();
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            if( HTTPS_RECVED == httpspost.Recv(buf))
            {
                // cout<< "response: " << buf << endl;
            }
        }
        
        // stopTrans
        url     = "http://183.131.144.74:8080/evchong_API/evchong/v1/stopTrans";
        param   = "{\"idTag\":{\"idToken\":\"123456\"},\"transactionId\":1,\"transactionData\":{\"values\":{\"measurand\":\"Energy_Active_Import_Register\",\"unit\":\"Wh\",\"value\":\"xxxxxx\",\"location\":\"Outlet\",\"context\":\"Sample_Periodic\",\"format\":\"Raw\",\"timestamp\":1425886218825}},\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"445566\"},\"meterStop\":1,\"timestamp\":1425886218829}";
        
        buf.erase();
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            if( HTTPS_RECVED == httpspost.Recv(buf))
            {
                // cout<< "response: " << buf << endl;
            }
        }
        
        // statusNotify
        url     = "http://183.131.144.74:8080/evchong_API/evchong/v1/statusNotify";
        param   = "{\"connectorId\":1,\"vendorErrorCode\":\"1122\",\"chargeBoxSerialNumber\":{\"chargeBoxSerialNumber\":\"123456\"},\"errorCode\":\"GroundFailure\",\"vendorId\":\"4455\",\"info\":\"xxxxx\",\"timestamp\":1425290540330,\"status\":\"Available\"}";
        
        buf.erase();
        
        if( HTTPS_SENTED == httpspost.Send(url, param) )
        {
            if( HTTPS_RECVED == httpspost.Recv(buf))
            {
                // cout<< "response: " << buf << endl;
            }
        }
        

        httpspost.Close();
    }
    
}
#endif


    
}


