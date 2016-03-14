#include <map>
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <pthread.h>
#include <getopt.h>
#include <stdlib.h>
#include "AdGlobal.h"
#include "AdUtil.h"
#include "AdProcessMsg.h"
#include "AdRtbBase.h"
#include "AdProtocolBase.h"
#include "AdAdapter.h"
#include "AdTime.h"
#include "AdKafka.h"
#include "AdAvro.h"
#include "AdControlTask.h"

using namespace std;

#define AD_KK_TOPREQ                    (char*)"test-request"
#define AD_KK_TOPRES                    (char*)"test-response"
#define AD_KK_TOPNIL                    (char*)"test-request-noresponse" 


string m_ReqJsonFile = "/opt/AdAdpLogs/conf/request-v4.avsc";
string m_ResJsonFile = "/opt/AdAdpLogs/conf/response-v4.avsc";
string m_KafkaServer = "192.168.167.102:6667,192.168.167.103:6667,192.168.167.104:6667,192.168.167.105:6667,192.168.167.106:6667,192.168.167.107:6667";

class AdLogTask:public AdTask
{
public:
	AdLogTask(AdProcessMsg * pMsg, INT32 id)
	{
		m_pMsg = pMsg;
		m_iMsgId = id;
		m_sDescription = "Ad Adapter Log Syterm, Work Thread";
	};
	virtual ~AdLogTask(){};
	int GetReqAv(AdRtbRequest * ctReq, AdAvro & Avro, AdCache & ctCache)
	{
		int iAvroNum = 0;
		AdTime cTime;
		/* init */
		Avro.DatUmInit();

	#if 1
		/* tag */
		if(ctReq->m_ltCrowdCategory.size() > 0){
			list<string>::iterator iter;
			CHAR8 szTagBuf[AD_KBYTES4];
			for( iter = ctReq->m_ltCrowdCategory.begin(); iter != ctReq->m_ltCrowdCategory.end(); iter++)
			{
				sprintf(szTagBuf + strlen(szTagBuf), "%d_%s|", ctReq->m_uiAdx, (*iter).c_str());
			}	
			Avro.AvSetOptional((char *)"user_tags", (void *)&szTagBuf, AD_AV_STR);
		}
		/* category */
		if(ctReq->m_ltSiteCategory.size() > 0){
			list<string>::iterator iter;
			CHAR8 szTagBuf[AD_KBYTES4];
			for( iter = ctReq->m_ltSiteCategory.begin(); iter != ctReq->m_ltSiteCategory.end(); iter++)
			{
				sprintf(szTagBuf + strlen(szTagBuf), "%s|", (*iter).c_str());
			}	
			Avro.AvSetOptional((char *)"web_category", (void *)&szTagBuf, AD_AV_STR);
			Avro.AvSetOptional((char *)"url_category", (void *)&szTagBuf, AD_AV_STR);
		}	
		
		/* time */
		struct timeval stTime;
		gettimeofday(&stTime, NULL);
		ULONG m_ulTime = stTime.tv_sec * 1000 + stTime.tv_usec / 1000;

		/* avro */
		Avro.AvSetOptional((char *)"req_time", (void *)&m_ulTime, AD_AV_LNG);
		Avro.AvSetRequired((char *)"adx_id", (void *)&ctReq->m_uiAdx, AD_AV_INT);
		Avro.AvSetRequired((char *)"req_id", (void *)(ctReq->m_sBidID.c_str()), AD_AV_STR);
		Avro.AvSetOptional((char *)"ip", (void *)ctReq->m_sUserIP.c_str(), AD_AV_STR);
		Avro.AvSetOptional((char *)"adx_uid", (void *)ctReq->m_sUserID.c_str(), AD_AV_STR);
		Avro.AvSetOptional((char *)"ua", (void *)ctReq->m_sUserAgent.c_str(), AD_AV_STR);
		Avro.AvSetOptional((char *)"url", (void *)ctReq->m_sUserUrl.c_str(), AD_AV_STR);
		Avro.AvSetOptional((char *)"pid", (void *)ctReq->m_sPId.c_str(), AD_AV_STR);
		Avro.AvSetOptional((char *)"size", (void *)ctReq->m_sSize.c_str(), AD_AV_STR);
		// web_category url_category
		float m_price = atof(ctReq->m_sMinPrice.c_str());
		Avro.AvSetOptional((char *)"cash_min", (void *)&m_price, AD_AV_FLT);
		//Avro.AvSetOptional((char *)"req_adapter_id", (void *), AD_AV_STR);
		Avro.AvSetOptional((char *)"view_screen", (void *)ctReq->m_sAdViewScreen.c_str(), AD_AV_STR);
		Avro.AvSetOptional((char *)"region", (void *)ctReq->m_sArea.c_str(), AD_AV_STR);
		Avro.AvSetOptional((char *)"flow_type", (void *)ctReq->m_sFlow.c_str(), AD_AV_STR);

		if( ctReq->m_ltMobileDevice.size() > 0){
			Avro.AvSetOptional((char *)"os", (void *)(*(ctReq->m_ltMobileDevice.begin())).c_str(), AD_AV_STR);
		}
		if( ctReq->m_ltMobileConnect.size() > 0){
			Avro.AvSetOptional((char *)"netconn", (void *)(*(ctReq->m_ltMobileConnect.begin())).c_str(), AD_AV_STR);
		}
		// idfa
		Avro.AvSetOptional((char *)"idfa", (void *)ctReq->m_sIDFA.c_str(), AD_AV_STR);
		Avro.AvSetOptional((char *)"imei", (void *)ctReq->m_sIMEI.c_str(), AD_AV_STR);
		Avro.AvSetOptional((char *)"android_id", (void *)ctReq->m_sAndroidID.c_str(), AD_AV_STR);
		
		iAvroNum =  Avro.AdAvroWrite( ctCache.Get(), AD_KBYTES8);
		Avro.DatUmFree();
	#endif

		return iAvroNum;
	};
	int GetResAv(AdRtbResponse* ctRes, AdAvro & Avro, AdCache & ctCache)
	{
		int iAvroNum = 0;
		AdTime cTime;
		/* init */
		Avro.DatUmInit();

	#if 1
		/* time */
		struct timeval stTime;
		gettimeofday(&stTime, NULL);
		ULONG m_ulTime = stTime.tv_sec * 1000 + stTime.tv_usec / 1000;

		/* avro */
		Avro.AvSetOptional((char *)"resp_time", (void *)&m_ulTime, AD_AV_LNG);
		Avro.AvSetRequired((char *)"adx_id", (void *)&ctRes->m_uiAdx, AD_AV_INT);
		Avro.AvSetRequired((char *)"req_id", (void *)(ctRes->m_sBidID.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"creative_id", (void *)(ctRes->m_sCreativeID.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"execute_id", (void *)(ctRes->m_sExecID.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"pid", (void *)(ctRes->m_sPId.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"region", (void *)(ctRes->m_sArea.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"tag_cr", (void *)(ctRes->m_sHotTag.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"tag_kw", (void *)(ctRes->m_sKeyWord.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"hot_key", (void *)(ctRes->m_sHotKey.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"hot_url", (void *)(ctRes->m_sHotUrl.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"hot_time", (void *)(ctRes->m_sHotTime.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"adx_uid", (void *)(ctRes->m_sUserID.c_str()), AD_AV_STR);
		//Avro.AvSetRequired((char *)"bid_core_id", (void *), AD_AV_STR);
		Avro.AvSetRequired((char *)"bid_mode", (void *)(ctRes->m_sTargetPriceType.c_str()), AD_AV_STR);
		float price = atof(ctRes->m_sTargetPrice.c_str());
		Avro.AvSetRequired((char *)"bid_price", (void *)&price, AD_AV_FLT);
		Avro.AvSetRequired((char *)"ck_id", (void *)(ctRes->m_sTargetPriceType.c_str()), AD_AV_STR);
		Avro.AvSetRequired((char *)"geo_uid", (void *)(ctRes->m_sAdirectID.c_str()), AD_AV_STR);
		//Avro.AvSetRequired((char *)"lib_vers", (void *)(ctRes->.c_str()), AD_AV_STR);
		
		iAvroNum =  Avro.AdAvroWrite( ctCache.Get(), AD_KBYTES8);
		Avro.DatUmFree();
		return iAvroNum;
	};
	
	#endif	
	virtual void TaskLoop()
	{
		int ret;
		UINT iError = 0;
		CHAR8 cRecBuff[AD_KBYTES8 * 4];
		AdRtbRequest  ctRequest;
		AdRtbResponse  ctResponse;
		AdRtbProtocol   ctProto;
		AdLogMsg  ctMsg;

		//AdKafka  
		//AdAv
		AdAvro ReqAvro;
		AdAvro ResAvro;
		AdKafka Kafka;
		AdCache ctCache;

		ReqAvro.loadJsonFile((char*)m_ReqJsonFile.c_str());
		ResAvro.loadJsonFile((char*)m_ResJsonFile.c_str());
		Kafka.initProducer((char*)m_KafkaServer.c_str());
		
		while(m_bTaskRun)
		{
			SetTaskWaitting(true);
			ret = m_pMsg->AdProcessMsgGet(static_cast<void *>(&ctMsg),sizeof(ctMsg),true,m_iMsgId);
			SetTaskWaitting(false);
			if(ret <= 0)
			{
				continue;
			}
			
			int iAvLength = 0;
			ctCache.Clear();
			
			switch (ctMsg.m_ucType)
			{
				case AD_MSG_TYPE_REQUEST:
					// req with noresponse
					ret = ctProto.TlvToInternal(ctRequest, ctMsg.m_uMsg, ctMsg.m_iLength);
					if(ret != AD_SUCCESS)
						break;
					//deal
					
					iAvLength = GetReqAv(&ctRequest, ReqAvro, ctCache);
					if ( iAvLength > 0)
					{
						Kafka.ProduceMessage( AD_KK_TOPNIL, ctCache.Get(), iAvLength);
					}					
					
					ctRequest.clear();
					break;
				case AD_MSG_TYPE_REQUEST_BIDDING:
					// req with response
					ret = ctProto.TlvToInternal(ctRequest, ctMsg.m_uMsg, ctMsg.m_iLength);
					if(ret != AD_SUCCESS)
						break;
					iAvLength = GetReqAv(&ctRequest, ReqAvro, ctCache);
					if ( iAvLength > 0)
					{
						Kafka.ProduceMessage( AD_KK_TOPREQ, ctCache.Get(), iAvLength);
					}	

					ctRequest.clear();
					break;
				case AD_MSG_TYPE_RESPONSE:
					// response
					ret = ctProto.TlvToInternal(ctResponse, ctMsg.m_uMsg, ctMsg.m_iLength);
					if(ret != AD_SUCCESS)
						break;
					iAvLength = GetResAv(&ctResponse, ResAvro, ctCache);
					if ( iAvLength > 0)
					{
						Kafka.ProduceMessage( AD_KK_TOPRES, ctCache.Get(), iAvLength);
					}

					ctResponse.clear();
					break;
				default:
					break;
			}
			
			m_uiLoop ++;
		}
		return;
	};
private:
	AdProcessMsg * m_pMsg;
	INT32  m_iMsgId;
	
};

/**********************system main loop      **********************/

static void  AdSystemSignalHandle(int sig)
{
	if(sig == SIGHUP)
		AD_ERROR("Recive HUP Signal, Ignore\n");
	else if(sig == SIGPIPE)
		AD_ERROR("Recive PIPE Signal, Ignore\n");
	else 
		AD_ERROR("Recive Unknow Signal  %d, Ignore\n", sig);
}


static int AdSystemSetSignal()
{
	struct sigaction action;

	action.sa_handler = AdSystemSignalHandle;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	if(sigaction(SIGHUP, &action, NULL) < 0)
	{
		AD_ERROR("Set Signal Error\n");
		return AD_FAILURE;
	}

	if (sigaction(SIGPIPE, &action, 0) == -1) 
	{
		  AD_ERROR("Set Signal Error\n");
		  return AD_FAILURE;
	}
	return AD_SUCCESS;
}

int main(int argc, char ** argv)
{
	int ret;

	AdSystemSetSignal();

	cout<<"Log Init ProcessMsg  ...................................."<<endl;
	string path = AD_PROCESS_MSG_PATH;
	AdProcessMsg ctProcessMsg;
	ret = ctProcessMsg.AdProcessMsgInit(path, AD_PROCESS_MSG_KEY,  AD_PROCESS_MSG_NUM);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Process Msg Error\n");
		return ret;
	}
	int count = AD_PROCESS_MSG_NUM - 1;
	while(count >= 0)
	{
		AdLogTask * pTask = AdControlCreateTask<AdLogTask, AdProcessMsg *, INT32>(AD_TASK_OTHER+ count, &ctProcessMsg, count);
		if (pTask == NULL) 
		{
			AD_ERROR("Task create error\n");
			exit(AD_FAILURE);
		}
		count --;
	}

#if 0
	//init Control output
	AdControl &ctControl = AdControl::Instance();

	ret = ctControl.OutputInit(atoi(sDetectPort.c_str()));
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Control Output init error\n");
		return ret;
	}

	ctControl.TaskDetect(300);
#endif
	while(true)
	{
		Time::wait(10);
	}

	exit(AD_SUCCESS);
}


