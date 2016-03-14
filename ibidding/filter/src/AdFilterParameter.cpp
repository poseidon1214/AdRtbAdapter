/***************************************
*	Description: Ad filter:  popup result depand on Parameter
*	Author: wilson
*	Date: 2015/07/30
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <stdlib.h>	
#include "AdFilterParameter.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"
#include "AdFilterBase.h"

int AdFilterParameter::match(void * data, AD_RESULT & mResult)
{
	AD_RESULT_ITER iter;			
	vector<string>  vPara;
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);
	AdPlanningUnit * pUnit;
	UINT   height = 0;
	UINT   width = 0;
	ULONG ulMask = AD_FILTER_NORMAL;
	int flag;
	UINT uiFlow = atoi(pRequest->m_sFlow.c_str()) ;
	UINT uiView = atoi(pRequest->m_sAdViewScreen.c_str());
	UINT uiViewType = atoi(pRequest->m_sAdViewType.c_str());
	

	if(uiFlow   == AD_PROTOCOL_FLOW_MOBILE)
	{
		ulMask |= AD_FILTER_MOBILE;
	}
	
	iter = mResult.begin();
	
	if(uiFlow   == AD_PROTOCOL_FLOW_VIDEO)
	{
		AdString::StrSplit(pRequest->m_sSize, vPara, "x");
		if(vPara.size() == 2)
		{
			height = atoi(vPara[1].c_str());
			width= atoi(vPara[0].c_str());
		}
	}
	
	while(iter != mResult.end())
	{
		if(iter->second.m_uiMatch != ulMask)
		{
			mResult.erase(iter ++);
			continue;
		}
		
		pUnit=static_cast<AdPlanningUnit *>(iter->first);
		
		/* mobile */
		if ( ((uiFlow  == AD_PROTOCOL_FLOW_MOBILE)
					&& (pUnit->m_stMobile.m_bEnable== false)) 
				|| ((uiFlow  == AD_PROTOCOL_FLOW_PC) 
					&& (pUnit->m_stMobile.m_bEnable == true)))
		{
			
			mResult.erase(iter ++);
			continue;
		} 

#if 0
		/* price */
		if(pRequest->m_uiTransaction != AD_PID_MODE_FIXED)
		{
			mResult.erase(iter ++);
			continue;
		}
#endif

		if(!pUnit->m_stOrder.m_ltAdViewScreen.empty())
		{
			list<UINT>::iterator lIter;
			for(lIter = pUnit->m_stOrder.m_ltAdViewScreen.begin(); \
				lIter != pUnit->m_stOrder.m_ltAdViewScreen.end(); lIter ++)
			{
				if(*lIter == 0)
					break;
				if(*lIter >= uiView)
					break;
			}
			if(lIter == pUnit->m_stOrder.m_ltAdViewScreen.end())
			{
				mResult.erase(iter ++);
				continue;
			}
		}
		
		if(!pUnit->m_stOrder.m_ltAdViewType.empty())
		{
			list<UINT>::iterator lIter;
			for(lIter = pUnit->m_stOrder.m_ltAdViewType.begin(); \
				lIter != pUnit->m_stOrder.m_ltAdViewType.end(); lIter ++)
			{
				if(*lIter == 0)
					break;
				if(*lIter == uiViewType)
					break;
			}
			if(lIter == pUnit->m_stOrder.m_ltAdViewType.end())
			{
				mResult.erase(iter ++);
				continue;
			}
		}

		if(uiFlow  == AD_PROTOCOL_FLOW_VIDEO)
		{
			flag = 0;
			if((height > 0) && (width > 0))
			{
				map<UINT, AdCreativeInfo>::iterator mIter;
				mIter = pUnit->m_mCreativeInfo.find(pRequest->m_uiAdx);
				if(mIter != pUnit->m_mCreativeInfo.end())
				{
					list<AdMaterialInfo>::iterator lIter;
					for(lIter = mIter->second.m_ltMaterials.begin(); \
						lIter != mIter->second.m_ltMaterials.end();  lIter ++)
					{
						if((lIter->m_uiHeight == height) && (lIter->m_uiWidth== width))
						{
							flag = 1;
							break;
						}
					}
				}
			}
			if(flag == 0)
			{
				mResult.erase(iter ++);
				continue;
			}
		}
		iter ++;
	}

	return AD_SUCCESS;
}

int AdFilterParameter::AddData(void * data)
{
	return AD_SUCCESS;
}
