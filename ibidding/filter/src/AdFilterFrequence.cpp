/***************************************
*	Description: Ad filter for frequence
*	Author: wilson
*	Date: 2015/07/28
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "AdFilterFrequence.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"
#include "AdMd5.h"
#include "AdRedisCli.h"

AdFilterFrequence::AdFilterFrequence()
{
	m_sName = "Frequence";
}

int AdFilterFrequence::AdFilterFrequenceInit(map<string, string> & mServer)
{
	map<string, string>::iterator iter;
	vector<string> vStr;
	for(iter = mServer.begin(); iter != mServer.end(); iter ++)
	{
		AdString::StrSplit(iter->second, vStr,  ":");
		if(vStr.size() == 2)
		{
			m_vIp.push_back(vStr[0]);
			m_vPort.push_back(atoi(vStr[1].c_str()));
		}
		vStr.clear();
	}
	return AD_SUCCESS;
}

AdFilterFrequence::~AdFilterFrequence()
{
}

int AdFilterFrequence::match(void * data, AD_RESULT & mResult)
{

//ADD
	return AD_SUCCESS;

	AD_RESULT_ITER iter;
	AdPlanningUnit * pUnit;
	char sStr[AD_BYTE512];
	AdRedisCli  ctRedis;
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);

	if((mResult.empty()) && (m_vIp.empty()))
		return AD_SUCCESS;

	UINT id = pRequest->m_uiRequestId%((UINT)m_vIp.size());

	int ret;
	ret = ctRedis.AdRedisConnect(m_vIp[id], m_vPort[id]);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Redis Connect error\n");
		//mResult.clear();
		return AD_FAILURE;
	}

	map<int, int> mFre;
	vector<map<int, int> > vFre;
	vector<string> vStr;
	map<int, int>::iterator mIter;

	snprintf(sStr, AD_BYTE512, "user:pv:day:%s:%d", pRequest->m_sUserID.c_str(), pRequest->m_uiAdx);
	vStr.push_back(sStr);
	vFre.push_back(mFre);
	snprintf(sStr, AD_BYTE512, "user:pv:hour:%s:%d", pRequest->m_sUserID.c_str(), pRequest->m_uiAdx);
	vStr.push_back(sStr);
	vFre.push_back(mFre);
	snprintf(sStr, AD_BYTE512, "pid:pv:%s", pRequest->m_sPId.c_str());
	vStr.push_back(sStr);
	vFre.push_back(mFre);
	
	ret = ctRedis.AdRedisArrayHGetAll(vStr, vFre);
	if(ret == AD_FAILURE)
	{
		AD_ERROR("Redis Get error\n");
		//mResult.clear();
		return AD_FAILURE;
	}	

	for(iter = mResult.begin();  iter != mResult.end();)
	{
		pUnit = static_cast<AdPlanningUnit *>(iter->first);

		if(pUnit->m_pAdvancedPrice != NULL) 
		{
			map<string,  AdAdvancedPrice>::iterator  advIter = pUnit->m_pAdvancedPrice->find(pRequest->m_sPId);
			if((advIter !=  pUnit->m_pAdvancedPrice->end()) && ( advIter->second.m_iEpv > 0))
			{
				mIter = vFre[2].find(pUnit->m_stOrder.m_uiExecuteId);
				if((mIter !=  vFre[2].end()) &&  (mIter->second >= advIter->second.m_iEpv))
				{
					mResult.erase(iter ++);
					continue;
				}
			}
		}

		if(pUnit->m_stOrder.m_uiTimeType == AD_TIME_TYPE_NO)
		{
			iter ++;
			continue;
		}
		
		
		if( AD_TIME_TYPE_DAY == pUnit->m_stOrder.m_uiTimeType )
		{
			mIter =  vFre[0].find(pUnit->m_stOrder.m_uiExecuteId);
			if((mIter !=  vFre[0].end()) &&  ((UINT)(mIter->second) >= pUnit->m_stOrder.m_uiFrequency))
			{
				mResult.erase(iter ++);
				continue;
			}
		}
		else
		{
			mIter =  vFre[1].find(pUnit->m_stOrder.m_uiExecuteId);
			if((mIter !=  vFre[1].end()) && ((UINT)(mIter->second) >= pUnit->m_stOrder.m_uiFrequency))
			{
				mResult.erase(iter ++);
				continue;
			}
		}

		iter ++;
	}

	ctRedis.AdRedisClose();
	if(pRequest->checkLife())
	{
		mResult.clear();
	}
	return AD_SUCCESS;
}


int AdFilterFrequence::AddData(void * data)
{	
	return AD_SUCCESS;
}

