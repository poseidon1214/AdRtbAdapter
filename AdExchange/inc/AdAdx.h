/***************************************
*	Description: adx madule, transfer protocol  send for dsp
*	Author: wilson
*	Date: 2016/03/04
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef  __AD_ADX_H__
#define __AD_ADX_H__
#include <map>
#include <list>
#include <iostream>
#include "AdRtbConfig.h"
#include "AdUtil.h"

#include "AdSession.h"
#include "AdBuff.h"
#include "AdLock.h"
#include "AdNetTask.h"
#include "AdTime.h"
#include "AdOpenRtb.h"
#include "AdSsp.h"
#include "AdMult.h"
#include "AdRedisCli.h"


using namespace std;

//print Process flags
//info type
#define AD_DEBUG_PRINT_REALTIME                     1
#define AD_DEBUG_PRINT_PROCESS                      2
#define AD_DEBUG_PRINT_INFO                         3

//print Process flags
#define AD_ADX_PRINT_INPUT					    0x00000001
#define AD_ADX_PRINT_REQUEST               			    0x00000002
#define AD_ADX_PRINT_FILTER                                      0x00000004
#define AD_ADX_PRINT_RESULT                                      0x00000008
#define AD_ADX_PRINT_RANK                                        0x00000010
#define AD_ADX_PRINT_RESPONSE                                   0x00000020
#define AD_ADX_PRINT_OUTPUT                                     0x00000040

#define STATUS_UPDATE_TIME                                               10

class AdAdxTaskStatus:public AdTaskState
{
public:
	AdAdxTaskStatus()
	{
		//all count
		m_uiAll = 0;
		m_uiSysError = 0;
		m_uiReqError = 0;
		m_uiResError = 0;
		m_uiRequest = 0;
		m_uiResponse = 0;
		m_uiTimeout = 0;
		//last second
		m_uiAllRate = 0;
		m_uiSysErrorRate = 0;
		m_uiReqErrorRate = 0;
		m_uiResErrorRate = 0;
		m_uiRequestRate = 0;
		m_uiResponseRate = 0;
		m_uiTimeoutRate = 0;
		//temp
		m_uiAllNum = 0;
		m_uiSysErrorNum = 0;
		m_uiReqErrorNum = 0;
		m_uiResErrorNum = 0;
		m_uiRequestNum = 0;
		m_uiResponseNum = 0;
		m_uiTimeoutNum = 0;
	};
	~AdAdxTaskStatus(){};

	void StatusAdd()
	{
		if(m_ctLastSecond.toNow() > 1000000)
		{
			m_uiRequestRate = m_uiRequestNum;
			m_uiResponseRate = m_uiResponseNum;
			m_uiAllRate = m_uiAllNum;
			m_uiReqErrorRate = m_uiReqErrorNum;
			m_uiResErrorRate = m_uiResErrorNum;
			m_uiSysErrorRate = m_uiSysErrorNum;
			m_uiTimeoutRate = m_uiTimeoutNum;

			m_uiAllNum = 0;
			m_uiReqErrorNum = 0;
			m_uiResErrorNum = 0;
			m_uiSysErrorNum = 0;
			m_uiRequestNum = 0;
			m_uiResponseNum = 0;
			m_uiTimeoutNum = 0;

			m_ctLastSecond.update();
		}
		m_uiAll ++;
		m_uiAllNum ++;
	};
	void StatusAddReqError()
	{
		m_uiReqError ++;
		m_uiReqErrorNum ++;
	};

	void StatusAddResError()
	{
		m_uiResError ++;
		m_uiResErrorNum ++;
	};

	void StatusAddSysError()
	{
		m_uiSysError ++;
		m_uiSysErrorNum ++;
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
		m_uiResponse++;
		m_uiResponseNum++;
	};

	void StatusAddCharge(string & sDspId, DOUBLE fPrice)
	{
		m_mCharge[sDspId] += fPrice;
		m_mChargeNum[sDspId] += fPrice;
	};

	void StatusAddPV(INT32 iMid, string & sPid)
	{
		m_mPV[iMid][sPid] ++;
		m_mPVNum[iMid][sPid] ++;
	};

	void StatusAddClick(INT32 iMid, string & sPid)
	{
		m_mClick[iMid][sPid] ++;
		m_mClickNum[iMid][sPid] ++;
	};

	virtual void  Print()
	{
		AD_PRINT("	ALL:           %d(%d/sec)\n", m_uiAll, m_uiAllRate);
		AD_PRINT("	SysError:      %d(%d/sec)\n", m_uiSysError, m_uiSysErrorRate);
		AD_PRINT("	ReqError:      %d(%d/sec)\n", m_uiReqError, m_uiReqErrorRate);
		AD_PRINT("	ResError:      %d(%d/sec)\n", m_uiResError, m_uiResErrorRate);
		AD_PRINT("	Timeout:       %d(%d/sec)\n", m_uiTimeout, m_uiTimeoutRate);
		AD_PRINT("	Request:       %d(%d/sec)\n", m_uiRequest, m_uiRequestRate);
		AD_PRINT("	Response:      %d(%d/sec)\n", m_uiResponse, m_uiResponseRate);
		AD_PRINT("	Charge:\n");
		map<string, DOUBLE>::iterator iter1;
		for(iter1 = m_mCharge.begin(); iter1 != m_mCharge.end(); iter1 ++)
		{
			AD_PRINT("	    %s:      %lf\n", iter1->first.c_str(), iter1->second);
		}

		map<string, ULONG>::iterator iter;
		AD_PRINT("	PV:\n");
		map<INT32, map<string, ULONG> >::iterator mIter;
		for(mIter= m_mPV.begin(); mIter != m_mPV.end(); mIter ++)
		{
			AD_PRINT("	    Media:      %d\n", mIter->first);
			for(iter = (mIter->second).begin(); iter != (mIter->second).end(); iter ++)
			{
				AD_PRINT("	        %s:      %ld\n", iter->first.c_str(), iter->second);
			}
		}

		AD_PRINT("	Click:\n");
		for(mIter= m_mClick.begin(); mIter != m_mClick.end(); mIter ++)
		{
			AD_PRINT("	    Media:      %d\n", mIter->first);
			for(iter = (mIter->second).begin(); iter != (mIter->second).end(); iter ++)
			{
				AD_PRINT("	        %s:      %ld\n", iter->first.c_str(), iter->second);
			}
		}
	};

	INT32 StatusUpdate( string & server)
	{
		if(server.empty())
			return AD_FAILURE;
		vector<string> vStr;
		AdString::StrSplit(server, vStr, ":");
		if(vStr.size() != 2)
			return AD_FAILURE;
		
		char str[AD_BYTE128];
		string sKey;
		string sFiled;
		string sDay;
		
		AdRedisCli   ctRedis(vStr[0], atoi(vStr[1].c_str()));
		int ret;

		
		if(m_mPV.empty())
			return AD_SUCCESS;

		if( m_ctLastTime.toNow() < STATUS_UPDATE_TIME * 1000000)
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

		//update charge info to redis
		map<string, DOUBLE>::iterator iter1;
		for(iter1 = m_mChargeNum.begin(); iter1 != m_mChargeNum.end(); iter1 ++)
		{
			snprintf(str, AD_BYTE128, "ssp:charge:%s:%s", sDay.c_str(), iter1->first.c_str());
			sKey = str;
			ret = ctRedis.AdRedisHIncRby(sKey, sFiled, (INT32)iter1->second);
			if(ret != AD_SUCCESS)
			{
				AD_ERROR("redis error\n");
			 	break;
			}
		}
		m_mChargeNum.clear();

		//update PV info to redis
		map<string, INT32>::iterator iter;
		map<INT32, map<string, INT32> >::iterator mIter;
		for(mIter= m_mPVNum.begin(); mIter != m_mPVNum.end(); mIter ++)
		{
			for(iter = (mIter->second).begin(); iter != (mIter->second).end(); iter ++)
			{
				snprintf(str, AD_BYTE128, "ssp:pv:%s:%s", sDay.c_str(), iter->first.c_str());
				sKey = str;
				ret = ctRedis.AdRedisHIncRby(sKey, sFiled, iter->second);
				if(ret != AD_SUCCESS)
				{
					AD_ERROR("redis error\n");
			 		break;
				}
			}
		}
		m_mPVNum.clear();

		//update Click Info ( no use)
		for(mIter= m_mClickNum.begin(); mIter != m_mClickNum.end(); mIter ++)
		{
			for(iter = (mIter->second).begin(); iter != (mIter->second).end(); iter ++)
			{
				snprintf(str, AD_BYTE128, "ssp:click:%s:%s", sDay.c_str(), iter->first.c_str());
				sKey = str;
				ret = ctRedis.AdRedisHIncRby(sKey, sFiled, iter->second);
				if(ret != AD_SUCCESS)
				{
					AD_ERROR("redis error\n");
			 		break;
				}
			}
		}
		m_mClickNum.clear();

		//close redis
		ctRedis.AdRedisClose();
		return ret;
	}

private:
	UINT m_uiAll;
	UINT m_uiAllRate;
	UINT m_uiAllNum;
	UINT m_uiSysError;
	UINT m_uiSysErrorRate;
	UINT m_uiSysErrorNum;
	UINT m_uiReqError;
	UINT m_uiReqErrorRate;
	UINT m_uiReqErrorNum;
	UINT m_uiResError;
	UINT m_uiResErrorRate;
	UINT m_uiResErrorNum;
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
	Time   m_ctLastSecond;
	map<string, DOUBLE> m_mCharge;
	map<INT32, map<string, ULONG> > m_mPV;
	map<INT32, map<string, ULONG> > m_mClick;
	map<string, DOUBLE> m_mChargeNum;
	map<INT32, map<string, INT32> > m_mPVNum;
	map<INT32, map<string, INT32> > m_mClickNum;
};

class AdAdx:public AdNetTaskInterface
{
public:
	AdAdx(AdMult * pMult);
	~AdAdx();
	//init adx conf, class
	int Init(string & sConf);
	//update config
	int Update(void);
	
	//Alloc Para, reuse policy
	AdRtbPara * AllocPara();
	void  FreePara(AdRtbPara *pPara);

	//response timeout
	int SessionTimeout(AdRtbPara & ctPara, AdTask & ctTask, AdSession * pSession = NULL);

	//net task interface;
	virtual  int Run(AdCache * pCache, AdSession * pSession, AdTask & ctTask);
	virtual  int PacketLen(AdCache* pCache);
	
private:
	//check click uri
	int RedirectLandingPage(AdRtbPara & ctPara, AdSession * pSession, AdTask & ctTask);
	//struct to http packet for dsp
	int SendDsp(AdRtbPara * pPara, AdSession * pSession, AdTask & ctTask);
	//deal bidding response,  session,  timeout.
	int DspResponse(AdCache * pCache, AdSession *pSession, AdTask & ctTask);
	int SendResponse(AdRtbPara & ctPara, AdAdxTaskStatus * pState);

	unsigned char ToHex(unsigned char x);

	unsigned char FromHex(unsigned char x) ;

	void UrlEncode(const std::string& str, std::string & sDst);

	void UrlDecode(const std::string& str,  std::string & sDst);
	
	void AddStatus(AdTask & ctTask);
	
	//adapter configs
	AdPingPongBuffer<AdRtbConfig>  m_ctConfs;
	//adapter config file name
	string m_sConfigFile;

	//Debug user ip map
	AdPingPongBuffer<map<string, int> > m_ctDebugUser;

	//Dsp session
	AdLRBuffer  m_ctDspSession;

	//Free Parameter list
	AdRingBufferMRMW<AdRtbPara *> m_ctFreePara;

	AdPingPongBuffer<ADCONFIG> m_cDspServer;
	bool m_bRemote;
	AdMult * m_pMult;
	AdSsp m_cSsp;
};

#endif

