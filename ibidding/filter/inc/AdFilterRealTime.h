/***************************************
*	Description: Ad filter for Real time info check.
*	Author: wilson
*	Date: 2015/07/28
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_READTIME_H
#define _AD_FILTER_READTIME_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"
#include "AdUdp.h"
#include "AdPlanningData.h"
#include "AdLock.h"

using namespace std;

class AdRealTimeInfo
{
public:
	AdRealTimeInfo()
	{
		clear();
	};
	~AdRealTimeInfo()
	{
		clear();
	};
	void clear()
	{
		memset(this, 0, sizeof(AdRealTimeInfo));
	};
	UINT m_uiPushFlag;			// Push Flag
	UINT m_uiExecuteId;			// EID
	UINT m_uiAdv;			// Adver
	time_t m_ulTime;		// Timestamp
	bool m_bHourShare;		// EID HourShare
	UINT m_uiAdvsType;		// EID AdvsType
	UINT m_uiAdvsTime;		// EID AdvsTime
	UINT m_uiHaveShow;		// EID HaveShow
	UINT m_uiHaveClick;		// EID HaveClick
	UINT m_uiWillSpend;		// EID WillSpend
	long long m_llHaveSpend;		// EID HaveSpend
	long long m_llAdvToday;			// adver today spend
	long long m_llAdvRemain;		// advertiser remain
	//UINT m_uiHaveRes;			// EID Response
	struct {
		ULONG m_uiTotal;
		ULONG m_uiNumber;
	}m_stClock[AD_DAY_HOURS];
};

typedef struct AdRealTimeData
{
	UINT m_uiExecuteId;
	UINT m_uiAdv;
	UINT m_uiHou;
	UINT m_uiShow;
	UINT m_uiClick;
	long long m_llAdvToday;
	long long m_llAdvRemain;
	long long m_llHaveSpend;
	long long m_llHourSpend;
}ADRTDATA_S;

class AdFilterRealTime:public AdMap
{
public:
	AdFilterRealTime();
	virtual ~AdFilterRealTime();
	virtual int match(void * data, AD_RESULT & mResult);
	virtual int AddData(void * data);
	virtual string & name()
	{
		return m_sName;
	};

	int AdFilterRealTimePrint(UINT iEid);
	int AdFilterRealTimeInit(string & sIp, int iPort);
	int AdFilterPushData(char* psPush);
	void AdFilterClear();
	string m_sIp;
	int   m_iPort;
	bool m_bTaskRun;
	int AdFilterGetWeekDay();

private:
	int AdFilterRealTimeUpdate(ADRTDATA_S* pstCtl);
	
	map<ULONG,  AdRealTimeInfo> m_mInfo;
	pthread_t  m_pId;
	LockReadWrite m_ctLock;
	string m_sName;
	UINT  m_uiLastDay;
};

#endif

