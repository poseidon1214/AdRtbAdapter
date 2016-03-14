/***************************************
*	Description: Ad time filter.
*	Author: wilson
*	Date: 2015/07/20
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_RTB_FILTERS_H
#define	_AD_RTB_FILTERS_H
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"
#include "AdPlanningData.h"
#include "AdRtbConfig.h"

#include "AdFilterBase.h"
#include "AdFilterTime.h"
#include "AdFilterAdExchange.h"
#include "AdFilterPid.h"
#include "AdFilterArea.h"
#include "AdFilterSize.h"
#include "AdFilterMobileOperator.h"
#include "AdFilterMobileDevice.h"
#include "AdFilterMobileConnect.h"
#include "AdFilterBan.h"
#include "AdFilterLandingPage.h"
#include "AdFilterRealTime.h"
#include "AdFilterFrequence.h"
#include "AdFilterParameter.h"
#include "AdFilterMark.h"
#include "AdFilterTanx.h"
#include "AdFilterUserAgent.h"
#include "AdBuff.h"

using namespace std;

#define AD_FILTERS_TIME					0x00000001
#define AD_FILTERS_ADEXCHANGE			0x00000002
#define AD_FILTERS_PID					0x00000004
#define AD_FILTERS_AREA					0x00000008
#define AD_FILTERS_SIZE					0x00000010
#define AD_FILTERS_MOBILEOPERATOR		0x00000020
#define AD_FILTERS_MOBILEDEVICE			0x00000040
#define AD_FILTERS_MOBILECONNECT		0x00000080
#define AD_FILTERS_PARAMETER			0x00000100
#define AD_FILTERS_BAN					0x00000200
#define AD_FILTERS_LANDINGPAGE			0x00000400
#define AD_FILTERS_REALTIME				0x00000800
#define AD_FILTERS_FREQUENCE			0x00001000
#define AD_FILTERS_MARK					0x00002000
#define AD_FILTERS_TANX 				0x00004000
#define AD_FILTERS_AGENT				0x00008000
#define AD_FILTERS_ALL					0x0000ffff


class AdRtbFilters
{
public:
	AdRtbFilters();
	virtual ~AdRtbFilters();
	void FiltersMatch(void * data, AD_RESULT & mResult, bool bPrint = false);
	int FiltersUpdate(map<UINT, AdPlanningUnit*> & mData, AdRtbConfig& ctConf);
	int FiltersPrintRealTime(UINT iEid);

protected:
	void FiltersClear();
	int FiltersCreate(UINT uiFlags);
	int FiltersInit(UINT uiFlags, AdRtbConfig & ctConf);
private:
	AdPingPongBuffer<vector<AdMap *> > m_vFilters;

	template<class C>
	int FiltersCreateFilter(UINT flag)
	{
		//back cache:  clear and reinit;
		vector<AdMap *> & vBak = m_vFilters.GetBack();
		//main cache:  match filters use or not;
		vector<AdMap *> & vNow = m_vFilters.Get();
		if((flag != 0) || vNow.empty())
		{
			C * pFilter = new C;
			if(pFilter == NULL)
			{
				AD_ERROR("memory not enough\n");	
				FiltersClear();
				return AD_FAILURE;
			}
			vBak.push_back(static_cast<AdMap *>(pFilter));
		}
		else
		{
			vBak.push_back(static_cast<AdMap *>(NULL));
		}
		
		return AD_SUCCESS;
	}
};
#endif /* _GTMAP_H */
