/***************************************
*	Description: Adapter madule, transfer protocol  send for ibiding
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef  __AD_ADAPTER_H__
#define __AD_ADAPTER_H__
#include <map>
#include <list>
#include <iostream>
#include "AdRtbConfig.h"
#include "AdProcessMsg.h"
#include "AdAdpFilterBan.h"
#include "AdConHash.h"
#include "AdSharedLib.h"
#include "AdProtocolBase.h"
#include "AdUtil.h"

#include "AdRtbEngine.h"
#include "AdSession.h"
#include "AdBuff.h"
#include "AdLock.h"
#include "AdTask.h"
#include "AdNetTask.h"
#include "AdAdapterMethod.h"
#include "AdTime.h"
#include "AdIpArea.h"


using namespace std;

#define AD_PROCESS_MSG_NUM       0x8
#define AD_PROCESS_MSG_KEY       'A'
#define AD_PROCESS_MSG_PATH   "/tmp"



class AdAdapterTaskStatus:public AdTaskState
{
public:
	AdAdapterTaskStatus()
	{
		//all count
		m_uiAll = 0;
		m_uiError = 0;
		m_uiRequest = 0;
		m_uiResponse = 0;
		m_uiTimeout = 0;
		//last second
		m_uiAllRate = 0;
		m_uiErrorRate = 0;
		m_uiRequestRate = 0;
		m_uiResponseRate = 0;
		m_uiTimeoutRate = 0;
		//temp
		m_uiAllNum = 0;
		m_uiErrorNum = 0;
		m_uiRequestNum = 0;
		m_uiResponseNum = 0;
		m_uiTimeoutNum = 0;
	};
	~AdAdapterTaskStatus(){};

	void StatusAdd()
	{
		if(m_ctLastTime.toNow() > 1000000)
		{
			m_uiRequestRate = m_uiRequestNum;
			m_uiResponseRate = m_uiResponseNum;
			m_uiAllRate = m_uiAllNum;
			m_uiErrorRate = m_uiErrorNum;
			m_uiTimeoutRate = m_uiTimeoutNum;

			m_uiAllNum = 0;
			m_uiErrorNum = 0;
			m_uiRequestNum = 0;
			m_uiResponseNum = 0;
			m_uiTimeoutNum = 0;

			m_ctLastTime.update();
		}
		m_uiAll ++;
		m_uiAllNum ++;
	};
	void StatusAddError()
	{
		m_uiError ++;
		m_uiErrorNum ++;
	};

	void StatusAddTimeout()
	{
		m_uiTimeout++;
		m_uiTimeoutNum++;
	};
	void StatusAddRequest()
	{
		m_uiRequest ++;
		m_uiRequestNum ++;
	};

	void StatusAddResponse()
	{
		m_uiResponse ++;
		m_uiResponseNum ++;
	};

	virtual void  Print()
	{
		AD_PRINT("	ALL:           %d(%d/sec)\n", m_uiAll, m_uiAllRate);
		AD_PRINT("	Error:         %d(%d/sec)\n", m_uiError, m_uiErrorRate);
		AD_PRINT("	Timeout:       %d(%d/sec)\n", m_uiTimeout, m_uiTimeoutRate);
		AD_PRINT("	Request:       %d(%d/sec)\n", m_uiRequest, m_uiRequestRate);
		AD_PRINT("	Response:      %d(%d/sec)\n", m_uiResponse, m_uiResponseRate);
	};

	UINT m_uiAll;
	UINT m_uiAllRate;
	UINT m_uiAllNum;
	UINT m_uiError;
	UINT m_uiErrorRate;
	UINT m_uiErrorNum;
	UINT m_uiRequest;
	UINT m_uiRequestRate;
	UINT m_uiRequestNum;
	UINT m_uiResponse;
	UINT m_uiResponseRate;
	UINT m_uiResponseNum;
	UINT m_uiTimeout;
	UINT m_uiTimeoutRate;
	UINT m_uiTimeoutNum;
	Time   m_ctLastTime;
};

class AdAdapter:public AdNetTaskInterface
{
public:
	AdAdapter();
	~AdAdapter();
	//init adapter conf, class
	int AdAdapterInit(string & sConf, bool m_bBidding = false);
	//init bidding engine
	int AdAdapterBiddingInit();
	//update config
	int AdAdapterUpdate(void);

	AdRtbEngine *AdAdapterGetEngine();	
	
	int SessionTimeout(AdAdapterPara & ctPara, AdTask & ctTask, AdSession * pSession = NULL);
	
	//Alloc Para, reuse policy
	AdAdapterPara * AllocPara();
	void  FreePara(AdAdapterPara *pPara);

	//net task interface;
	virtual  int Run(AdCache * pCache, AdSession * pSession, AdTask & ctTask);
	virtual  int PacketLen(AdCache* pCache);
	
private:
	//packet  transfer to internel struct
	int AdAdapterToInternal(AdAdapterPara & ctPara, AdTask & ctTaskt);
	//struct to http packet
	int AdAdapterToHttp(AdAdapterPara & ctPara, AdTask & ctTask, bool bTlv = false);
	//deal bidding response,  session,  timeout.
	int AdAdapterIBiddingResponse(AdCache * pCache, AdSession *pSession, AdTask & ctTask);
	//put msg for log process
	int AdAdapterPutMsg(AdAdapterPara& ctPara);
	
	void AdAdapterAddStatus(AdTask & ctTask);
	
	//adapter configs
	AdPingPongBuffer<AdRtbConfig>  m_ctConfs;
	//adapter config file name
	string m_sConfigFile;

	//Debug user ip map
	AdPingPongBuffer<map<string, int> > m_ctDebugUser;
	
	//adapter global ban
	AdAdpFilterBan m_ctBanFilter;

	//log msg
	AdProcessMsg  m_ctProcessMsg;
	//method: transfer protocol
	AdPingPongBuffer<AdAdapterMethod>   m_ctMethod;

	//bidding engine, depand on engine config
	AdRtbEngine * m_pEngine;
	//local biddiong session
	AdLRBuffer  m_ctLocalSession;
	//remote bidding session
	AdLRBuffer   m_ctNetSession;
	//Free Parameter list
	AdRingBufferMRMW<AdAdapterPara *> m_ctFreePara;

	AdPingPongBuffer<AdIpArea>   m_ctIpArea;
};

#endif
