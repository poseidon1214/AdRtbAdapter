/***************************************
*	Description: class Admap.
*	Author: wilson
*	Date: 2015/07/20
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_BASE_H
#define	_AD_FILTER_BASE_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdPlanningData.h"
#include "AdUtil.h"


using namespace std;

#define	AD_FILTER_TIME			0x8000
#define	AD_FILTER_ADX			0x4000
#define	AD_FILTER_PID			0x2000
#define	AD_FILTER_AREA			0x1000
#define AD_FILTER_SIZE			0x0800
#define AD_FILTER_UA			0x0400

#define	AD_FILTER_MOBILE_OPERATER       0x0080
#define	AD_FILTER_MOBILE_DEVICE         0x0040
#define	AD_FILTER_MOBILE_CONNECT		0x0020

#define   AD_FILTER_NORMAL       (AD_FILTER_TIME | AD_FILTER_ADX | AD_FILTER_PID | AD_FILTER_AREA | AD_FILTER_SIZE | AD_FILTER_UA)
#define   AD_FILTER_MOBILE	      (AD_FILTER_MOBILE_OPERATER | AD_FILTER_MOBILE_DEVICE | AD_FILTER_MOBILE_CONNECT)

class AdFilterMarkInfo
{
public:
	AdFilterMarkInfo()
	{
		clear();
	};
	virtual ~AdFilterMarkInfo()
	{
		clear();
	};
	void clear()
	{
		m_uiType = 0;
		m_sMark.clear();
	}
	UINT m_uiType;
	string m_sMark;
};

class AdRtbFilterResult
{
public:
	AdRtbFilterResult()
	{
		clear();
	};
	virtual ~AdRtbFilterResult()
	{
		clear();
	};
	void clear()
	{
		m_bVip = false;
		m_uiMatch = 0;
		m_ltMarkInfo.clear();
	}
	bool  m_bVip;
	UINT  m_uiMatch;
	list<AdFilterMarkInfo > m_ltMarkInfo;
	
};

class ResultSort
{
public:
	bool operator() (void * const _A, void * const _B) const
	{
		AdPlanningUnit * pDataA = static_cast<AdPlanningUnit * >(_A);
		AdPlanningUnit * pDataB = static_cast<AdPlanningUnit * >(_B);
		return pDataA->m_stOrder.m_uiExecuteId < pDataB->m_stOrder.m_uiExecuteId;
	} 
};

typedef std::map<void *, AdRtbFilterResult, ResultSort> 	AD_RESULT;
typedef std::map<void *, AdRtbFilterResult >::iterator	  AD_RESULT_ITER;

class Result
{	
public:
	static void display(AD_RESULT & mResult)
	{
		AD_RESULT_ITER iter;
		AdPlanningUnit * pUnit;
		int i = 0;
		for(iter = mResult.begin(); iter != mResult.end(); iter ++)
		{
			pUnit = static_cast<AdPlanningUnit *>(iter->first);
			AD_PRINT("[%05d]:%04x ",pUnit->m_stOrder.m_uiExecuteId,iter->second.m_uiMatch);
			i ++;
			if(i  % 5 == 0)
				AD_PRINT("\n");
		}
		if(i%5 != 0)
			AD_PRINT("\n");
	}
};  


#endif 

