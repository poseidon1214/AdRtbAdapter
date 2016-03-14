/***************************************
*	Description: Ad filter for Material size
*	Author: wilson
*	Date: 2015/07/25
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#include "AdFilterSize.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"

int AdFilterSize::match(void * data, AD_RESULT & mResult)
{
	AD_ITER  adIter;
	
	list<void *>::iterator listIter;

	char key[AD_BYTE128] = { 0 };
				
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);
	
	snprintf(key, AD_BYTE128, "%d_%s",	pRequest->m_uiAdx, pRequest->m_sSize.c_str());
	
	adIter = find(key);
	if(end() != adIter)
	{
		list<void *> & list = adIter->second;
		for(listIter = list.begin(); listIter != list.end(); listIter ++)
		{
			AdRtbFilterResult & sResult = mResult[*listIter];
			sResult.m_uiMatch |= AD_FILTER_SIZE;
		}
	}

	return AD_SUCCESS;
}


int AdFilterSize::AddData(void * data)
{
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	char key[AD_BYTE128] = { 0 };
	map<UINT, AdCreativeInfo>::iterator iter;	
	list<AdMaterialInfo>::iterator lIter;

	if ( true  != pUnit->m_stTime.bPush)
	{
		return AD_SUCCESS;
	}

	for(iter = pUnit->m_mCreativeInfo.begin();iter!=pUnit->m_mCreativeInfo.end();iter ++)
	{
		if(iter->second.m_sCreativeGroupId.empty() || iter->second.m_sAdxId.empty())
			continue;
		for(lIter = iter->second.m_ltMaterials.begin(); lIter != iter->second.m_ltMaterials.end(); lIter ++)
		{
			snprintf(key, AD_BYTE128, "%s_%dx%d", iter->second.m_sAdxId.c_str(), (*lIter).m_uiWidth, (*lIter).m_uiHeight);
			update(key, static_cast<void *>(pUnit));
		}
				
	}
	return AD_SUCCESS;
}
