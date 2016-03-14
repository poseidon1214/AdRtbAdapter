/***************************************
*	Description: Ad time filter.
*	Author: wilson
*	Date: 2015/07/20
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdFilterTime.h"
#include "AdPlanningData.h"

int AdFilterTime::match(void * data, AD_RESULT & mResult)
{

	time_t ulTime;
	struct tm stTime;
	char szKey[AD_BYTE16] = { 0 };

	time(&ulTime);
	localtime_r(&ulTime, &stTime);
	strftime(szKey, AD_BYTE16, "%w%H", &stTime);
	string sKey = szKey;
	
	AD_ITER mapIter = find(sKey);
	list<void *>::iterator listIter;
	if(end() != mapIter)
	{
		list<void *> & list = mapIter->second;
		for(listIter = list.begin(); listIter != list.end(); listIter ++)
		{
			AdRtbFilterResult & sResult = mResult[*listIter];
			sResult.m_uiMatch |= AD_FILTER_TIME;
		}
	}

	return AD_SUCCESS;
}

int AdFilterTime::AddData(void * data)
{
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	UINT i;
	UINT uiDay;
	UINT uiTime;
	UINT uiHour;
	char key[AD_BYTE16] = { 0 };

	if ( true  != pUnit->m_stTime.bPush) 
	{
		return AD_SUCCESS;
	}
	
	for(i = 0; i < AD_WEEK_DAYS; i++)
	{
		uiTime = pUnit->m_stTime.m_stDay[i].m_uiHour;
		if (0 == uiTime) 
		{
			continue;	
		}

		uiDay = pUnit->m_stTime.m_stDay[i].m_uiWeek;
		for (uiHour = 0; uiHour < AD_DAY_HOURS; uiHour ++) 
		{
			if (uiTime & (1 << uiHour)) 
			{
				snprintf(key, AD_BYTE128, "%d%02d", uiDay,uiHour);
				update(key, static_cast<void *>(pUnit));
			}
		}
	}	
	
	return AD_SUCCESS;
}
