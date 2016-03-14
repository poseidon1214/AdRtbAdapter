/***************************************
*	Description: Ad filter for ban list.
*	Author: wilson
*	Date: 2015/07/25
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "stdlib.h"	
#include "AdFilterBan.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"
	
int AdFilterBan::match(void * data, AD_RESULT & mResult)
{
	AD_ITER  adIter;
	
	list<void *>::iterator lvIter;

	char key[AD_BYTE128] = { 0 };
				
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);
	list<string>::iterator lsIter;

	//popup ordre ban list
	adIter = find(pRequest->m_sPId);
	if(end() != adIter)
	{
		list<void *> & list = adIter->second;
		for(lvIter = list.begin(); lvIter != list.end(); lvIter ++)
		{
			mResult.erase(*lvIter);
		}
	}

	//popup  media sensitivty
	for(lsIter = pRequest->m_ltSensitive.begin(); lsIter != pRequest->m_ltSensitive.end(); lsIter ++)
	{
		snprintf(key, AD_BYTE128, "CAT_%d_%s",	pRequest->m_uiAdx, lsIter->c_str());
	
		adIter = find(key);
		if(end() != adIter)
		{
			list<void *> & list = adIter->second;
			for(lvIter = list.begin(); lvIter != list.end(); lvIter ++)
			{
				mResult.erase(*lvIter);
			}
		}
	}

	//popup  media category
	for(lsIter = pRequest->m_ltCategory.begin(); lsIter != pRequest->m_ltCategory.end(); lsIter ++)
	{
		snprintf(key, AD_BYTE128, "CAT_%d_%s",	pRequest->m_uiAdx, lsIter->c_str());
	
		adIter = find(key);
		if(end() != adIter)
		{
			list<void *> & list = adIter->second;
			for(lvIter = list.begin(); lvIter != list.end(); lvIter ++)
			{
				mResult.erase(*lvIter);
			}
		}
	}

	//popup  media creative
	for(lsIter = pRequest->m_ltCreate.begin(); lsIter != pRequest->m_ltCreate.end(); lsIter ++)
	{
		snprintf(key, AD_BYTE128, "CRT_%d_%s_%s", pRequest->m_uiAdx, pRequest->m_sSize.c_str(), lsIter->c_str());
	
		adIter = find(key);
		if(end() != adIter)
		{
			list<void *> & list = adIter->second;
			for(lvIter = list.begin(); lvIter != list.end(); lvIter ++)
			{
				mResult.erase(*lvIter);
			}
		}
	}

	// adv ban list
	snprintf(key, AD_BYTE128, "%d_%s",	pRequest->m_uiAdx, pRequest->m_sPId.c_str());
	adIter = find(key);
	if(end() != adIter)
	{
		list<void *> & list = adIter->second;
		for(lvIter = list.begin(); lvIter != list.end(); lvIter ++)
		{
			mResult.erase(*lvIter);
		}
	}
	
	// category ban list 
	for(lsIter = pRequest->m_ltSiteCategory.begin();lsIter != pRequest->m_ltSiteCategory.end();lsIter ++)
	{
		snprintf(key, AD_BYTE128, "%d_%s", pRequest->m_uiAdx, lsIter->c_str());
		adIter = find(key);
		if(end() != adIter)
		{
			list<void *> & list = adIter->second;
			for(lvIter = list.begin(); lvIter != list.end(); lvIter ++)
			{
				mResult.erase(*lvIter);
			}
		}
	}
	
	return AD_SUCCESS;
}


int AdFilterBan::AddData(void * data)
{
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	char key[AD_BYTE128] = { 0 };

	if ( true  != pUnit->m_stTime.bPush)
	{
		return AD_SUCCESS;
	}

	list<string>::iterator  lsIter;
	/* ban list   adx + pid*/
	for(lsIter = pUnit->m_stOrder.m_ltBanContent.begin(); lsIter != pUnit->m_stOrder.m_ltBanContent.end(); lsIter ++) 
	{
		update(*lsIter, static_cast<void *>(pUnit));
	}	

	/* advanced filter  create pid list */
	if(pUnit->m_pltBanPidList != NULL)
	{
		for(lsIter = pUnit->m_pltBanPidList->begin(); lsIter != pUnit->m_pltBanPidList->end(); lsIter ++) 
		{
			update(*lsIter, static_cast<void *>(pUnit));
		}
	}

	map<UINT, AdCreativeInfo>::iterator mIter;	
	list<AdMaterialInfo>::iterator laIter;

	for(mIter = pUnit->m_mCreativeInfo.begin();mIter!=pUnit->m_mCreativeInfo.end();mIter ++)
	{
		if(mIter->second.m_sCreativeGroupId.empty())
			continue;
		for(laIter = mIter->second.m_ltMaterials.begin(); laIter != mIter->second.m_ltMaterials.end(); laIter ++)
		{
			//for media ban creative
			if((*laIter).m_uiCreativeType)
			{
				snprintf(key, AD_BYTE128, "CRT_%s_%dx%d_%d", mIter->second.m_sAdxId.c_str(), \
							(*laIter).m_uiWidth, (*laIter).m_uiHeight, (*laIter).m_uiCreativeType);
				update(key, static_cast<void *>(pUnit));
			}

			//for media ban format
			if((*laIter).m_uiFormat)
			{
				snprintf(key, AD_BYTE128, "FMT_%s_%dx%d_%d", mIter->second.m_sAdxId.c_str(), \
							(*laIter).m_uiWidth, (*laIter).m_uiHeight, (*laIter).m_uiFormat);
				update(key, static_cast<void *>(pUnit));
			}

			//for media ban sensitivty category
			if(!(*laIter).m_sCategory.empty())
			{
				snprintf(key, AD_BYTE128,"CAT_%s_%s", mIter->second.m_sAdxId.c_str(), (*laIter).m_sCategory.c_str());
				update(key, static_cast<void *>(pUnit));
				if( AD_PROTOCOL_KEY_ADX_BAIDU == atoi(mIter->second.m_sAdxId.c_str()))
				{
					snprintf(key, AD_BYTE128, "CAT_%s_%c%c", mIter->second.m_sAdxId.c_str(), (*laIter).m_sCategory[0], \
						 (*laIter).m_sCategory[1]);
					update(key, static_cast<void *>(pUnit));
				}
			}						
		}			
	}

	
	return AD_SUCCESS;
}

