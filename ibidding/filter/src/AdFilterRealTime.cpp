/***************************************
*	Description: Ad filter for Real time info check.
*	Author: wilson
*	Date: 2015/07/28
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "AdFilterRealTime.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"

#define AD_REALTIME_STATE_OK				0x14	// push ok
#define AD_REALTIME_STATE_OVER				0x15 	// today budget money over		
#define AD_REALTIME_STATE_CONTROL			0x16	// hour control 
#define AD_REALTIME_STATE_SHARE				0x17	// hour share control
#define AD_REALTIME_STATE_NORANGE			0x18	// not have today range
#define AD_REALTIME_STATE_NOREMAIN			0x19	// advertise has no remain money

#define	AD_REALTIME_USING_TIME				30


void* AdFilterRealTimeTask(void * data);

AdFilterRealTime::AdFilterRealTime()
{
	m_sName = "RealTime";
	m_bTaskRun = false;
	m_uiLastDay = 0;
}

int AdFilterRealTime::AdFilterRealTimeInit(string & sIp, int iPort)
{
	int ret;
	m_sIp = sIp;
	m_iPort = iPort;
	m_bTaskRun = true;
	ret = pthread_create(&m_pId,  NULL, AdFilterRealTimeTask, static_cast<void *>(this));
	if (ret != AD_SUCCESS) 
	{
		AD_ERROR("Task create error\n");
		return AD_FAILURE;
	}
	return AD_SUCCESS;
}

AdFilterRealTime::~AdFilterRealTime()
{
	if(m_bTaskRun == true)
	{
		m_bTaskRun = false;
		pthread_join(m_pId, NULL);
	}
}

int AdFilterRealTime::AdFilterRealTimePrint(UINT iEid)
{
	map<ULONG, AdRealTimeInfo>::iterator mIter;
	int i;
	m_ctLock.lockRead();
	mIter = m_mInfo.find(iEid);
	AD_PRINT("Real Time Info  %d-----------------\n", iEid);
	if(mIter != m_mInfo.end())
	{
		AD_PRINT("flags: %d   ", mIter->second.m_uiPushFlag);
		switch(mIter->second.m_uiPushFlag)
		{
			case AD_REALTIME_STATE_OK:
				AD_PRINT("OK\n");
				break;
			case AD_REALTIME_STATE_OVER:
				AD_PRINT("Over\n");
				break;
			case AD_REALTIME_STATE_CONTROL:
				AD_PRINT("Control\n");
				break;
			case AD_REALTIME_STATE_SHARE:
				AD_PRINT("Hour Share \n");
				break;
			case AD_REALTIME_STATE_NORANGE:
				AD_PRINT("No Range\n");
				break;
			case AD_REALTIME_STATE_NOREMAIN:
				AD_PRINT("No Remain\n");
				break;
			default:
				AD_PRINT("No Init\n");
				break;
		}
			
		AD_PRINT("Advertiser:			%d		HourShare:			%s\n", mIter->second.m_uiAdv, (mIter->second.m_bHourShare == true)?"yes":"no");
		AD_PRINT("AdvsType:			%d		AdvsTime:			0x%x \n", mIter->second.m_uiAdvsType, mIter->second.m_uiAdvsTime);
		AD_PRINT("EidTodayBudget:	%d		EidTodaySpend:		%lld \n", mIter->second.m_uiWillSpend, mIter->second.m_llHaveSpend);
		AD_PRINT("AdvTodaySpend:	%lld		AdvRemainBudget:	%lld \n", mIter->second.m_llAdvToday, mIter->second.m_llAdvRemain);
		for (i = 0; i  < AD_DAY_HOURS; i ++) 
		{
			if (0 !=mIter->second.m_stClock[i].m_uiTotal)
			{
				AD_PRINT("%02d:00-%02d:00   ", i, (i != (AD_DAY_HOURS - 1))?(i + 1):0);
				AD_PRINT("Budget: %08ld,   ", mIter->second.m_stClock[i].m_uiTotal);
				AD_PRINT("RealSpend: %08ld\n", mIter->second.m_stClock[i].m_uiNumber);
			}
		}	
	}
	else
		AD_PRINT("No Found\n");
	m_ctLock.unlock();
	return AD_SUCCESS;
}


int AdFilterRealTime::AdFilterGetWeekDay()
{
	char pszDayTime[16] = {0};
	time_t ulTime;
   	struct tm  stTime;

	time(&ulTime);
	localtime_r(&ulTime, &stTime);
	strftime(pszDayTime, 16, "%w", &stTime);
	return atoi(pszDayTime);
}


int AdFilterRealTime::match(void * data, AD_RESULT & mResult)
{
	AD_RESULT_ITER iter;
	AdPlanningUnit * pUnit;
	
	time_t ulTime;
	map<ULONG, AdRealTimeInfo>::iterator mIter;

	m_ctLock.lockRead();
	for(iter = mResult.begin();  iter != mResult.end();)
	{
		pUnit = static_cast<AdPlanningUnit *>(iter->first);
		mIter = m_mInfo.find(pUnit->m_stOrder.m_uiExecuteId);
		if(mIter != m_mInfo.end())
		{
			time(&ulTime) ;
			ulTime -= mIter->second.m_ulTime;
			if ((ulTime > AD_REALTIME_USING_TIME)  || (  mIter->second.m_uiPushFlag != AD_REALTIME_STATE_OK))
			{
				mResult.erase(iter ++);
				continue;
			}
		}
		iter ++;
	}
	m_ctLock.unlock();

	return AD_SUCCESS;
}


int AdFilterRealTime::AddData(void * data)
{
	UINT  uiDay, uiWeek;
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	
	
	uiWeek = (UINT)AdFilterGetWeekDay();
	if(uiWeek != m_uiLastDay)
	{
		AdFilterClear();
		m_uiLastDay = uiWeek;
	}

	m_ctLock.lockWrite();
	AdRealTimeInfo & sInfo = m_mInfo[pUnit->m_stOrder.m_uiExecuteId];
	
	sInfo.m_uiExecuteId = pUnit->m_stOrder.m_uiExecuteId;
	sInfo.m_uiAdv = pUnit->m_stOrder.m_uiAdvertiser;
	sInfo.m_uiAdvsType = pUnit->m_stOrder.m_uiTargetPriceType;
	for(uiDay = 0; uiDay < AD_WEEK_DAYS; uiDay++)
	{
		if( uiWeek == pUnit->m_stTime.m_stDay[uiDay].m_uiWeek )
		{		
			sInfo.m_uiWillSpend = pUnit->m_stTime.m_stDay[uiDay].m_uiBudget;
			sInfo.m_uiAdvsTime = pUnit->m_stTime.m_stDay[uiDay].m_uiHour;
			break;
		}
	}
	sInfo.m_bHourShare = (0 != pUnit->m_stOrder.m_uiBidmode) ? true : false;

	if (true  != pUnit->m_stTime.bPush)
	{
		sInfo.m_uiPushFlag = AD_REALTIME_STATE_NORANGE;
	}
	m_ctLock.unlock();
	return AD_SUCCESS;
}

void AdFilterRealTime::AdFilterClear()
{
	m_ctLock.lockWrite();
	m_mInfo.clear();
	m_ctLock.unlock();
}


int AdFilterRealTime::AdFilterRealTimeUpdate(ADRTDATA_S* pstCtl)
{
	UINT iCount = 0;
	UINT uiHour = 0;
	UINT uiNumber = 0;

	m_ctLock.lockWrite();
	AdRealTimeInfo & sInfo = m_mInfo[pstCtl->m_uiExecuteId];
	
	time(&sInfo.m_ulTime);
	sInfo.m_uiHaveShow  = pstCtl->m_uiShow;
	sInfo.m_uiHaveClick = pstCtl->m_uiClick;
	sInfo.m_llHaveSpend = pstCtl->m_llHaveSpend;
	sInfo.m_llAdvRemain = pstCtl->m_llAdvRemain;
	sInfo.m_llAdvToday  = pstCtl->m_llAdvToday;

	/* number */
	sInfo.m_stClock[pstCtl->m_uiHou].m_uiNumber = pstCtl->m_llHourSpend;

	if( sInfo.m_uiPushFlag == AD_REALTIME_STATE_NORANGE)
	{
		goto RETURN;
	}

	/* ctl today budget */
	if( sInfo.m_uiWillSpend > 0 && (sInfo.m_llHaveSpend >= sInfo.m_uiWillSpend))
	{
		sInfo.m_uiPushFlag = AD_REALTIME_STATE_OVER;
		goto RETURN;
	}

	/* ctl advertiser budget */
	if( sInfo.m_llAdvRemain <= 0)
	{			
		sInfo.m_uiPushFlag = AD_REALTIME_STATE_NOREMAIN;
		goto RETURN;
	}

	/* total */
	if (0 != (sInfo.m_uiAdvsTime & (1 << pstCtl->m_uiHou)))
	{
		uiNumber = sInfo.m_llHaveSpend - pstCtl->m_llHourSpend;
		if (sInfo.m_uiWillSpend > uiNumber) 
		{
			for (iCount = pstCtl->m_uiHou; iCount < AD_DAY_HOURS; iCount ++) 
			{
				if (0 != (sInfo.m_uiAdvsTime & (1 << iCount))) 
				{
					uiHour += 1;
				}
			}
			switch (uiHour) 
			{
				case 0:
					break;
				case 1:
					sInfo.m_stClock[pstCtl->m_uiHou].m_uiTotal = sInfo.m_uiWillSpend - uiNumber;
					break;
				default:
					sInfo.m_stClock[pstCtl->m_uiHou].m_uiTotal = (sInfo.m_uiWillSpend - uiNumber) / uiHour;
					break;
			}
		}
	}
	else
	{
		/* ctl hour */
		sInfo.m_stClock[pstCtl->m_uiHou].m_uiTotal = 0;
		sInfo.m_uiPushFlag = AD_REALTIME_STATE_CONTROL; 
		goto RETURN;
	}
	
#if 0
	/* adver today spend vs remain */
	if( sInfo.m_uiAdvToday > sInfo.m_uiAdvRemain)
	{
		sInfo.m_uiPushFlag = AD_REALTIME_STATE_NOREMAIN;
		goto RETURN;
	}
#endif
	
	/* ctl hour budget */
	if(sInfo.m_uiWillSpend > 0 && true == sInfo.m_bHourShare)
	{
		if( sInfo.m_stClock[pstCtl->m_uiHou].m_uiTotal <= \
				sInfo.m_stClock[pstCtl->m_uiHou].m_uiNumber)
		{
			sInfo.m_uiPushFlag = AD_REALTIME_STATE_SHARE;
			goto RETURN;
		}
	}

	sInfo.m_uiPushFlag = AD_REALTIME_STATE_OK;

RETURN: 
	m_ctLock.unlock();
	return sInfo.m_uiPushFlag;
}

int AdFilterRealTime::AdFilterPushData(char* psPush)
{
/*
{
"type":"1",
"eid":"xxx",
"adv":"",
"hour":"xxx",
"show":"xxx",
"click":"xxx",
"advtoday":"xxx",
"advremain":"xxx",
"havespend":"xxx",
"hourspend":"xxx"
}
*/
	ADRTDATA_S stRTData;
	Json::Reader reader;
	Json::Value value;
	
	if (true == reader.parse((char *)psPush, value))
	{
		if( true == value.isMember("type") )
		{
			if( 0 == strncmp("1", value["type"].asString().c_str(),\
				value["league"].asString().length()) )
			{
				if( true == value.isMember("eid") )
				{
					stRTData.m_uiExecuteId = atoi(value["eid"].asString().c_str());
                }
				if( true == value.isMember("adv") )
				{
               		stRTData.m_uiAdv = atoi(value["adv"].asString().c_str());
                }
				if( true == value.isMember("hour") )
				{
					stRTData.m_uiHou = atoi(value["hour"].asString().c_str());
               	}
				if( true == value.isMember("show") )
				{
               		stRTData.m_uiShow = atoi(value["show"].asString().c_str());
               	}
                if( true == value.isMember("click") )
				{
					stRTData.m_uiClick = atoi(value["click"].asString().c_str());
                }
                if( true == value.isMember("advtoday") )
				{
					stRTData.m_llAdvToday = strtoul(value["advtoday"].asString().c_str(), 0, 10);
               	}
				if( true == value.isMember("advremain") )
				{
                	stRTData.m_llAdvRemain = strtoul(value["advremain"].asString().c_str(), 0, 10);
               	}
                if( true == value.isMember("havespend") )
				{
                	stRTData.m_llHaveSpend = strtoul(value["havespend"].asString().c_str(), 0, 10);
                }
                if( true == value.isMember("hourspend") )
				{
               		stRTData.m_llHourSpend = strtoul(value["hourspend"].asString().c_str(), 0, 10);
               	 }                
                
           	 }
		}
	}
	AdFilterRealTimeUpdate(&stRTData);
	return AD_SUCCESS;
}


void* AdFilterRealTimeTask(void * data)
{
 	int iLength = 0;
 	char szPacket[AD_KBYTES8] = { 0 };
	int ret;

	AdFilterRealTime * pInfo = static_cast<AdFilterRealTime *>(data);
 	AdUdp   cUdp;

	//UINT uiDay;
	//UINT uiLastDay = (UINT)(pInfo->AdFilterGetWeekDay());

	ret = cUdp.UdpMultiInit(const_cast<char *>(pInfo->m_sIp.c_str()), pInfo->m_iPort);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Udp init error\n");
		return NULL;
	}
	
	while (pInfo->m_bTaskRun)
	{
	  	iLength = cUdp.UdpRev(szPacket, AD_KBYTES8, 100000);
		if(iLength  > 0)
		{
		  	pInfo->AdFilterPushData(szPacket);
		  	memset(szPacket, '\0', AD_KBYTES8);
		}
	}	  
 	pthread_exit(NULL);
	return NULL;
}


