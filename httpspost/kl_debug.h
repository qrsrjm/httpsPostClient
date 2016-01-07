#ifndef __KL_DEBUG_H__
#define __KL_DEBUG_H__

#ifdef ZQFK
#define ALARMTOLCD
#endif


#ifndef _DEBUG
#define _DEBUG
#endif

#define _LOGTOFILE

//#define _DIABLE_MESSAGE
//#define __LCD_DEBUG_L1

#ifdef _DEBUG
//#define __CONNECTION_DEBUG_L1	
//#define __CONNECTION_DEBUG_L2//不用轻易开放，已经通过"/mnt/ptu/common/cfg/hex_data_trace.flag"以及"/mnt/ptu/common/cfg/asc_data_trace.flag"文件来决定是否开发收发流
#endif

#ifdef _DEBUG
#define __GDFKTASK_ALR_DEBUG_L1
#define __GDFKTASK_ALR_DEBUG_L2
#endif

#ifdef _DEBUG
//#define __CLIENT_NET_CONNECTION_DEBUG_L1
#endif

#ifdef _DEBUG
//#define __SMS_CONNECTION_DEBUG_L1
//#define __SMS_CONNECTION_DEBUG_L2
#endif

#ifdef _DEBUG
//#define __SERIAL_CONNECTION_DEBUG_L1
#endif

//#define __DAEMON_DEBUG_L1
//#define __DAEMON_DEBUG_L2
#ifdef _DEBUG
//#define __BUS_CONNECTION_DEBUG_L1
//#define __BUS_CONNECTION_DEBUG_L2
#endif

#ifndef _DEBUG
//#define __MESSAGE_DEBUG_L1
#endif


#ifdef _DEBUG
//#define __HKMJ_RULE_DEBUG_L1
#endif


#ifdef _DEBUG
#define __GDFK_RULE_DEBUG_L1
#define __GDFK_RULE_DEBUG_L2
#endif

#ifdef _DEBUG
//#define __FK_DEBUG_L1
//#define __FK_DEBUG_L2
#endif

#ifdef _DEBUG
//#define __GDFK_DATA_DEBUG_L1

//#define __DIR_RUNNER_DEBUG_L1

//#define __PLUG_IN_FACTORY_DEBUG_L1
//#define __PLUG_IN_FACTORY_DEBUG_L2

//#define __METER_RULE_FACTORY_DEBUG_L1
//#define __DEVICE_RULE_FACTORY_DEBUG_L1

//#define __GB645_RULE_DEBUG_L1
//#define __GB645_RULE_DEBUG_L2
//#define __LGR_RULE_DEBUG_L1
//#define  __AbbAin_RULE_DEBUG_L1
//#define  __AbbAin_RULE_DEBUG_L2
//#define  __MK3_RULE_DEBUG_L1
//#define  __MK3_RULE_DEBUG_L2


//#define __BIT_MASK_DEBUG
//#define __GATHER_GATHER_DATA_ITEM_DEBUG_L1
//#define __GATHER_ITEM_DEBUG_L1
//#define __METER_TASK_DEBUG_L1

//#define __SHARE_MEM_DEBUG_L1

//#define __EVENT_BASE_DEBUG_L1

//#define _GDFK_FUNC_DEBUG_L3
//#define _GDFK_FUNC_DEBUG_L2
//#define _GDFK_FUNC_DEBUG_L1

//#define __METER_RULE_DEBUG
//
//#define __METER_SERVER_DEBUG_L1
//#define __METER_SERVER_DEBUG_L2

//#define __PPP_SERVER_DEBUG
//#define __GATHER_CFG_DEBUG


//#define __TASK_SERVER_DEBUG
#endif


enum TraceCategory_Enum
{
	traceGeneral,
	traceDaemon,
	traceHttx,
	traceDbtx,
	traceLcd,
	traceTask,
	traceLoadCtrl,
	tracePPP
};

enum TraceLevel_Enum
{
	traceVerbose=1,
	traceInfo=2,
	traceWarn=3,
	traceError=4
};


#ifdef _DEBUG

#ifndef TRACE
#define TRACE TRACE_GENERAL
#endif  // KLTRACE

#define TRACE_GENERAL Trace("/mnt/update/general.log", __LINE__,traceGeneral)
#define TRACE_DAEMON Trace("/mnt/update/daemon.log", __LINE__,traceDaemon)
#define TRACE_HTTX Trace("/mnt/update/httx.log", __LINE__,traceHttx)
#define TRACE_DBTX Trace("/mnt/update/dbtx.log", __LINE__,traceDbtx)
#define TRACE_LCD Trace("/mnt/update/lcd.log", __LINE__,traceLcd)
#define TRACE_TASK Trace("/mnt/update/task.log", __LINE__,traceTask)
#define TRACE_LOAD_CTRL Trace("/mnt/update/loadctrl.log", __LINE__,traceLoadCtrl)
#define TRACE_PPP Trace("/mnt/update/ppp.log", __LINE__,tracePPP)
#define TRACE_CONN Trace("/mnt/update/conn.log", __LINE__,tracePPP)

#include <stdio.h>
#include <stdarg.h>
//#include "kl_type.h"


class Trace
{
public:
	Trace(const char *pszFileName, int nLineNo,TraceCategory_Enum enumCategory=traceGeneral)
		: m_pszFileName(pszFileName), m_nLineNo(nLineNo),m_enumCategory(enumCategory)
	{}


	inline void operator()(TraceLevel_Enum traceLevel,const char *pszFmt, ...) const
	{
		va_list ptr; va_start(ptr, pszFmt);
		TraceV(m_pszFileName,m_nLineNo,m_enumCategory,traceLevel,pszFmt,ptr);
		va_end(ptr);
	}
	inline void operator()(const char *pszFmt, ...) const
	{
		va_list ptr; va_start(ptr, pszFmt);
		TraceV(m_pszFileName,m_nLineNo,m_enumCategory,traceInfo,pszFmt,ptr);
		va_end(ptr);
	}
private:
	void TraceV(const char *pszFileName, int nLine,TraceCategory_Enum enumCategory, TraceLevel_Enum enumLevel, const char *pszFmt, va_list args) const
	{
#ifdef _LOGTOFILE
//		char  fileName[256];
//		sprintf(fileName,pszFileName,getpid());

		FILE* pFile=fopen(pszFileName,"r+");
		if (pFile==NULL) 
		{
			vprintf(pszFmt,args);
			return;
		}
		/////chh add begin  2015:8:19 13:49
#define D_LogFileMaxSize	50000000 //50000000
		fseek(pFile,0,SEEK_END);/////chh add   2015:8:20 10:30
		if (ftell(pFile) > D_LogFileMaxSize)
		{
			/* 打开可读写文件，若文件存在则文件长度清为零，
			即该文件内容会消失。若文件不存在则建立该文件，
			文件长度已经超过了maxSize,所以需要将原文件内容清空 */
			fclose(pFile);
			pFile = NULL;
			pFile = fopen(pszFileName, "w+");
			if (NULL == pFile)
			{
				printf("open file false! filename = %s\n", pszFileName);
				return ;
			}
		}
		/////chh add end   2015:8:19 13:49
		time_t tNow;
		time(&tNow);
		struct tm *time;
		time=localtime(&tNow);
		fseek(pFile,0,SEEK_END);
		fprintf(pFile,"%02d%02d-%02d-%02d %02d:%02d:%02d	",(time->tm_year+1900)/100,(time->tm_year+1900)%100,time->tm_mon+1,time->tm_mday,time->tm_hour,time->tm_min,time->tm_sec);
		fseek(pFile,0,SEEK_END);
		vfprintf(pFile,pszFmt,args);
		fclose(pFile);
#else
		time_t tNow;
		time(&tNow);
		struct tm *time;
		time=localtime(&tNow);
		printf("%02d-%02d %02d:%02d:%02d ",time->tm_mon+1,time->tm_mday,time->tm_hour,time->tm_min,time->tm_sec);
		vprintf(pszFmt,args);
#endif
	}
private:
	const char *const m_pszFileName;
	const int m_nLineNo;
	TraceCategory_Enum m_enumCategory;
};

#else

#ifndef TRACE
#define TRACE
#endif  // KLTRACE

#define TRACE_GENERAL
#define TRACE_DAEMON
#define TRACE_HTTX
#define TRACE_DBTX
#define TRACE_LCD
#define TRACE_TASK
#define TRACE_LOAD_CTRL
#define TRACE_PPP

#endif  // _DEBUG


#endif  // __KL_DEBUG_H__


