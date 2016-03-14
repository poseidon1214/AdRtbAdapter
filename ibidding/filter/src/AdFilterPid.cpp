/***************************************
*	Description: Ad filter for Pid.
*	Author: wilson
*	Date: 2015/07/25
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#include "AdFilterPid.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"

int AdFilterPid::match(void * data, AD_RESULT & mResult)
{
	AD_ITER  adIter;
	list<void *>::iterator listIter;

	list<string>::iterator lsIter;
	char key[AD_BYTE128] = { 0 };
				
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);

	snprintf(key, AD_BYTE128, "%d_ALL",  pRequest->m_uiAdx);
	adIter = find(key);
	if(end() != adIter)
	{
		list<void *> & list = adIter->second;
		for(listIter = list.begin(); listIter != list.end(); listIter ++)
		{
			AdRtbFilterResult & sResult = mResult[*listIter];
			sResult.m_uiMatch |= AD_FILTER_PID;
		}
	}
	
	snprintf(key,  AD_BYTE128, "%d_%s",	pRequest->m_uiAdx, pRequest->m_sPId.c_str());
	adIter = find(key);
	if(end() != adIter)
	{
		list<void *> & list = adIter->second;
		for(listIter = list.begin(); listIter != list.end(); listIter ++)
		{
			AdRtbFilterResult & sResult = mResult[*listIter];
			sResult.m_uiMatch |= AD_FILTER_PID;
		}
	}

	for(lsIter = pRequest->m_ltSiteCategory.begin();lsIter != pRequest->m_ltSiteCategory.end();lsIter ++)
	{
		snprintf(key, AD_BYTE128, "OTHER_%d_%s",	pRequest->m_uiAdx, (*lsIter).c_str());
		adIter = find(key);
		if(end() != adIter)
		{
			list<void *> & list = adIter->second;
			for(listIter = list.begin(); listIter != list.end(); listIter ++)
			{
				AdRtbFilterResult & sResult = mResult[*listIter];
				sResult.m_uiMatch |= AD_FILTER_PID;
			}
		}
	}
	return AD_SUCCESS;
}


int AdFilterPid::AddData(void * data)
{
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	char key[AD_BYTE128] = { 0 };
	list<AdPidInfo>::iterator iter;	
	list<string>::iterator sIter;

	if ( true  != pUnit->m_stTime.bPush)
	{
		return AD_SUCCESS;
	}

	for(iter = pUnit->m_ltPidInfo.begin();iter!=pUnit->m_ltPidInfo.end();iter ++)
	{
		switch ( (*iter).m_uiMode)
		{				
			case AD_PID_MODE_ALL:
				snprintf(key, AD_BYTE128, "%d_ALL", (*iter).m_uiAdxId);
				update(key, static_cast<void *>(pUnit));
				break;
			case AD_PID_MODE_FIXED:
				for(sIter = (*iter).m_ltPidList.begin(); sIter != (*iter).m_ltPidList.end(); sIter ++)
				{
					snprintf(key, AD_BYTE128, "%d_%s", (*iter).m_uiAdxId, (*sIter).c_str());
					update(key, static_cast<void *>(pUnit));
				}
				break;
			case AD_PID_MODE_OTHER:
				for(sIter = (*iter).m_ltPidList.begin(); sIter != (*iter).m_ltPidList.end(); sIter ++)
				{
					snprintf(key, AD_BYTE128, "OTHER_%d_%s", (*iter).m_uiAdxId, (*sIter).c_str());
					update(key, static_cast<void *>(pUnit));
				}
				break;
			default:
				break;
		}
	}
	return AD_SUCCESS;
}
