/***************************************
*	Description: Ad filter for user agent.
*	Author: wilson
*	Date: 2015/07/25
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#include "AdFilterUserAgent.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"

int AdFilterUserAgent::match(void * data, AD_RESULT & mResult)
{
	AD_ITER  adIter;
	
	list<void *>::iterator lvIter;

	list<string>::iterator lsIter;

	string sAll = "0";

	char key[AD_BYTE128] = { 0 };
				
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);

	adIter = find(sAll);
	if(end() != adIter)
	{
		list<void *> & list = adIter->second;
		for(lvIter = list.begin(); lvIter != list.end(); lvIter ++)
		{
			AdRtbFilterResult & sResult = mResult[*lvIter];
			sResult.m_uiMatch |= AD_FILTER_UA;
		}
	}

	for(lsIter = pRequest->m_ltUalist.begin();lsIter != pRequest->m_ltUalist.end(); lsIter ++)
	{
		snprintf(key, AD_BYTE128,"%s",  lsIter->c_str());
		adIter = find(key);
		if(end() != adIter)
		{
			list<void *> & list = adIter->second;
			for(lvIter = list.begin(); lvIter != list.end(); lvIter ++)
			{
				AdRtbFilterResult & sResult = mResult[*lvIter];
				sResult.m_uiMatch |= AD_FILTER_UA;
			}
		}
	}

	return AD_SUCCESS;

}


int AdFilterUserAgent::AddData(void * data)
{
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	char key[AD_BYTE128] = { 0 };
	list<UINT>::iterator iter;	

	char sAll[AD_BYTE16] = "0";
	if( ! pUnit->m_stMobile.m_ltUserAgentType.empty() ){
		for(iter = pUnit->m_stMobile.m_ltUserAgentType.begin();iter!= pUnit->m_stMobile.m_ltUserAgentType.end();iter ++)
		{
			snprintf(key, AD_BYTE128, "%d",  *iter);
			update(key, static_cast<void *>(pUnit));
		}
	}
	else{
		update(sAll, static_cast<void *>(pUnit));
	}

	return AD_SUCCESS;
}
