/***************************************
*	Description: Ad rtb Engine module,   system main module, include all bidding details.
*	Author: wilson
*	Date: 2015/07/25
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef	_AD_RTB_ENGINE_H
#define	_AD_RTB_ENGINE_H

#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include "AdGlobal.h"
#include "AdPlanningData.h"
#include "AdRtbFilters.h"
#include "AdRtbNetProtocol.h"
#include "AdRtbBase.h"
#include "AdRtbDealRank.h"
#include "AdLock.h"
#include "AdTask.h"
#include "AdBuff.h"
#include "AdUtil.h"
#include "AdNetTask.h"
#include "AdTime.h"

using namespace std;

#define AD_INTER_PACKET_FLAGS    0xaa559966

struct InternalPacket
{
	UINT uiFlags;
	AdRtbRequest * pRequest;
	AdRtbResponse * pResponse;
    void * pData;
};

#define AD_RTB_ENGINE_REINIT   0x44

#define AD_RTB_STATUS_UPDATE_TIME                                               10

class AdBiddingTaskStatus:public AdTaskState
{
public:
	AdBiddingTaskStatus()
	{
		m_uiRequest = 0;
		m_uiResponse= 0;
		m_uiRequestRate = 0;
		m_uiResponseRate= 0;
		m_uiRequestNum = 0;
		m_uiResponseNum= 0;
	};
	~AdBiddingTaskStatus()
	{
	};

	void AdRtbStatusResponseAdd()
	{
		m_uiResponse  ++;
		m_uiResponseNum ++;
	};

	void AdRtbStatusRequestAdd()
	{
		if(m_ctLastSecond.toNow() > 1000000)
		{
			m_uiRequestRate = m_uiRequestNum;
			m_uiResponseRate = m_uiResponseNum;
			m_uiRequestNum = 0;
			m_uiResponseNum = 0;
			m_ctLastSecond.update();
		}
		m_uiRequest++;
		m_uiRequestNum++;
	};
	
	void AdRtbStatusReadyAdd(UINT uiEid)
	{
		map<UINT,  INT32>::iterator  mIter;
		mIter = m_mReadyOrder.find(uiEid);
		if(mIter == m_mReadyOrder.end())
		{
			m_mReadyOrder[uiEid]  = 1;
		}
		else
		{
			(mIter->second) ++;
		}
	};

	INT32 AdRtbStatusUpdate( string & server, string passwd="")
	{
		if(server.empty())
			return AD_FAILURE;
		vector<string> vStr;
		AdString::StrSplit(server, vStr, ":");
		if(vStr.size() != 2)
			return AD_FAILURE;
		
		map<UINT,  INT32>::iterator  mIter;
		char str[AD_BYTE128];
		string sKey;
		string sFiled;
		string sDay;
		
		AdRedisCli   ctRedis(vStr[0], atoi(vStr[1].c_str()), password = passwd);
		int ret;

		
		if(m_mReadyOrder.empty())
			return AD_SUCCESS;

		if( m_ctLastTime.toNow() < AD_RTB_STATUS_UPDATE_TIME * 1000000)
			return AD_SUCCESS;
		m_ctLastTime.update();
		
		m_ctLastTime.getHour(sFiled);
		m_ctLastTime.getDay(sDay);
		
		ret = ctRedis.AdRedisConnect();
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("redis error\n");
			return ret;
		}

		for(mIter = m_mReadyOrder.begin();  mIter != m_mReadyOrder.end(); mIter ++)
		{
			snprintf(str, AD_BYTE128, "exec:response:%s:%d", sDay.c_str(), mIter->first);
			sKey = str;
			ret = ctRedis.AdRedisHIncRby(sKey, sFiled, mIter->second);
			if(ret != AD_SUCCESS)
			{
				AD_ERROR("redis error\n");
		 		break;
			}
			 mIter->second = 0;
		}

		ctRedis.AdRedisClose();
		return ret;
	}
	virtual void Print()
	{
		AD_PRINT("	Request:    %d(%d/sec)\n", m_uiRequest, m_uiRequestRate);
		AD_PRINT("	Response:   %d(%d/sec)\n", m_uiResponse, m_uiResponseRate);
	};

	UINT m_uiRequest;
	UINT  m_uiResponse;
	UINT m_uiRequestRate;
	UINT  m_uiResponseRate;
	UINT m_uiRequestNum;
	UINT  m_uiResponseNum;
	map<UINT,  INT32>  m_mReadyOrder;
	Time    m_ctLastTime;
	Time    m_ctLastSecond;
};

#define AD_IBIDDING_ENG_LOCAL    		  "IBIDDING(local)"
#define AD_IBIDDING_ENG_REMOTE      	"IBIDDING(remote)"

class AdRtbEngine:public AdNetTaskInterface
{
public:
	AdRtbEngine();
	virtual ~AdRtbEngine();
	int  AdRtbEngineInit(string & confFile);
	//for local mode only
	int  AdRtbEngineSetLocalMode(int iPort);
	//for remote mode only
	list<string> &  AdRtbEngineGetAdapter();
	int AdRtbEngineUpdate();
	//tcp tlv,  remote
	int AdRtbEngineRun(char * pPacket, int iLen, int iMaxLen, AdTask & ctTask);
	//tcp, internal packet,  local
	int  AdRtbEngineRun(AdRtbRequest & ctRequest, AdRtbResponse & ctResponse,AdTask & ctTask);
	//internal packet local
	int AdRtbEngineLoop(AdTask & ctTask);

	int AdRtbEnginePrintRealTime(UINT iEid)
	{
		return m_ctFilters.FiltersPrintRealTime(iEid);
	};

	virtual  int Run(AdCache * pCache, AdSession * pSession, AdTask & ctTask);

protected:
private:
	void AdRtbEngineAddStatus(AdTask & ctTask);
	AdMaterialInfo* AdRtbEngineGetMaterials(AdCreativeInfo & ctCreate, string & sSize);
	int AdRtbEngineBuild(AdRtbRequest & ctReq, AdRtbResponse & ctRes, AdUnitPrice & stPrice);
	AdPlanningData  	m_ctData;
	AdRtbFilters		m_ctFilters;
	AdPingPongBuffer<AdRtbConfig>	m_ctConfig;
	AdRtbDealRank		m_ctDealRank;

	int	  m_iPort;
	list<string> m_ltServer;
	string m_sConfigFile;
};

#endif /* _GTCONF_H */


