/***************************************
*	Description: Ad filter, select  Adexchange.
*	Author: wilson
*	Date: 2015/07/20
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#include "AdFilterAdExchange.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"

int AdFilterAdExchange::match(void * data, AD_RESULT & mResult)
{
	AD_ITER  adIter;
	
	list<void *>::iterator listIter;

	char key[AD_BYTE32] = { 0 };
				
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);

	snprintf(key, AD_BYTE32, "%d",  pRequest->m_uiAdx);

	adIter = find(key);
	if(end() != adIter)
	{
		list<void *> & list = adIter->second;
		for(listIter = list.begin(); listIter != list.end(); listIter ++)
		{
			AdRtbFilterResult & sResult = mResult[*listIter];
			sResult.m_uiMatch |= AD_FILTER_ADX;
		}
	}

	return AD_SUCCESS;
}


int AdFilterAdExchange::AddData(void * data)
{
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	char key[AD_BYTE32] = { 0 };
	list<AdPidInfo>::iterator iter;	

	if ( true  != pUnit->m_stTime.bPush)
	{
		return AD_SUCCESS;
	}

	for(iter = pUnit->m_ltPidInfo.begin();iter!=pUnit->m_ltPidInfo.end();iter ++)
	{
		if( 0 == (*iter).m_uiAdxId) 
			continue;
		else
		{
			snprintf(key, AD_BYTE32,  "%d", (*iter).m_uiAdxId);
			update(key, static_cast<void *>(pUnit));
		}
	}
	return AD_SUCCESS;
}
