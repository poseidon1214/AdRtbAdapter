/***************************************
*	Description: Ad filter mobile platform:   mobile Connect Type.
*	Author: wilson
*	Date: 2015/07/27
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <stdlib.h>
#include "AdFilterMobileConnect.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"

int AdFilterMobileConnect::match(void * data, AD_RESULT & mResult)
{
	AD_ITER  adIter;
	
	list<void *>::iterator lvIter;

	list<string>::iterator lsIter;

	string sAll = "0";

	char key[AD_BYTE128] = { 0 };
				
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);

	if(atoi(pRequest->m_sFlow.c_str()) != AD_PROTOCOL_FLOW_MOBILE)
		return AD_SUCCESS;

	adIter = find(sAll);
	if(end() != adIter)
	{
		list<void *> & list = adIter->second;
		for(lvIter = list.begin(); lvIter != list.end(); lvIter ++)
		{
			AdRtbFilterResult & sResult = mResult[*lvIter];
			sResult.m_uiMatch |= AD_FILTER_MOBILE_CONNECT;
		}
	}

	for(lsIter = pRequest->m_ltMobileConnect.begin();lsIter != pRequest->m_ltMobileConnect.end(); lsIter ++)
	{
		snprintf(key, AD_BYTE128, "%s",  lsIter->c_str());
		adIter = find(key);
		if(end() != adIter)
		{
			list<void *> & list = adIter->second;
			for(lvIter = list.begin(); lvIter != list.end(); lvIter ++)
			{
				AdRtbFilterResult & sResult = mResult[*lvIter];
				sResult.m_uiMatch |= AD_FILTER_MOBILE_CONNECT;
			}
		}
	}

	return AD_SUCCESS;
}


int AdFilterMobileConnect::AddData(void * data)
{
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	char key[AD_BYTE128] = { 0 };
	list<UINT>::iterator iter;	

	if ( true  != pUnit->m_stTime.bPush)
	{
		return AD_SUCCESS;
	}

	for(iter = pUnit->m_stMobile.m_ltConnectType.begin();iter!= pUnit->m_stMobile.m_ltConnectType.end();iter ++)
	{
		snprintf(key, AD_BYTE128, "%d",  *iter);
		update(key, static_cast<void *>(pUnit));
	}
	return AD_SUCCESS;
}



