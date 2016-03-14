/***************************************
*	Description: class Tanx Filter module.
*	Author: wilson
*	Date: 2015/12/09
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdFilterTanx.h"
#include "json/json.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"


AdFilterTanx::AdFilterTanx() 
{
	m_sName = "Tanx";
}

AdFilterTanx::~AdFilterTanx()
{
	m_mPidDes.clear();
}

#define LINE_MAX       (AD_KBYTES8 * 5)
int AdFilterTanx::AdFilterTanxInit(string &sFilter, string &sDes)
{
	char line[LINE_MAX] ;
	string sStr;
	map<string, AdTanxContent> mContent;
	int count;
	if(sFilter.empty() || sDes.empty())
		return AD_FAILURE;
	sStr.reserve(LINE_MAX);

	//Filter content
	FILE * pFile = fopen(sFilter.c_str(), "r");
	if (NULL == pFile) {
		AD_ERROR("%s fopen error\n", sFilter.c_str());
		return AD_FAILURE;
	}
	count = 0;
	while (NULL != fgets(line, LINE_MAX, pFile))
	{
		count ++;
		//if line > Max size,  ignore
		if(strlen(line) >= LINE_MAX - 1)
		{
			//read line tail 
			while (NULL != fgets(line, LINE_MAX, pFile))
			{
				AD_ERROR("Line  %d outbuff\n",count);
				if(strlen(line) < LINE_MAX - 1)
					break;
			}
			continue;
		}
		sStr.clear();
		sStr = line;
		if((sStr.size() == 2) && (sStr == "^D"))
			break;
		ADFilterTanxParseContent(sStr, mContent);
	}
	fclose(pFile);

	//Pid Descreption
	pFile = fopen(sDes.c_str(), "r");
	if (NULL == pFile) {
		AD_ERROR("%s fopen error\n", sDes.c_str());
		return AD_FAILURE;
	}
	count = 0;
	while (NULL != fgets(line, LINE_MAX, pFile))
	{
		count ++;
		//if line > Max size,  ignore
		if(strlen(line) >= LINE_MAX - 1)
		{
			//read line tail 
			while (NULL != fgets(line, LINE_MAX, pFile))
			{
				AD_ERROR("Line  %d outbuff\n",count);
				if(strlen(line) < LINE_MAX - 1)
					break;
			}
			continue;
		}
		sStr.clear();
		sStr = line;
		if((sStr.size() == 2) && (sStr == "^D"))
			break;
		ADFilterTanxParseDescreption(sStr, mContent);
	}

	fclose(pFile);

	cout<<"AdFilterTanx: "<<m_mPidDes.size()<<endl;
	return AD_SUCCESS;
}

int  AdFilterTanx::ADFilterTanxParseContent(string & sStr,map<string, AdTanxContent>& mContent)
{
	UINT uiCount = 0;
	Json::Reader reader;
	Json::Value value;
	string sFilter;
	char sValue[AD_BYTE128];

	if (true == reader.parse((char *)sStr.c_str(), value))
	{
		if (true == value.isMember("filter_id"))
		{
			sFilter = value["filter_id"].asString();
		}
		else
		{
			return AD_FAILURE;
		}

		AdTanxContent & ctContent = mContent[sFilter];
		if (true == value.isMember("excluded_ad_category"))
		{
			for(uiCount = 0; uiCount < value["excluded_ad_category"].size(); uiCount++)
			{
				int iVal = value["excluded_ad_category"][uiCount].asInt();
				snprintf(sValue, AD_BYTE128, "%d", iVal);
				string sKey = sValue;
				ctContent.add(sKey, AD_TANX_CATEGORY);
			}
		}

		if (true == value.isMember("excluded_click_through_url"))
		{
			for(uiCount = 0; uiCount < value["excluded_click_through_url"].size(); uiCount++)
			{
				string sKey = value["excluded_click_through_url"][uiCount].asString();
				ctContent.add(sKey, AD_TANX_LANDING_PAGE);
			}
		}

		if (true == value.isMember("excluded_filter"))
		{
			for(uiCount = 0; uiCount < value["excluded_filter"].size(); uiCount++)
			{
				int iVal = value["excluded_filter"][uiCount].asInt();
				snprintf(sValue, AD_BYTE128, "%d", iVal);
				string sKey = sValue;
				ctContent.add(sKey, AD_TANX_CREATIVE_TYPE);
			}
		}

		if (true == value.isMember("excluded_brand_advertiser_id"))
		{
			for(uiCount = 0; uiCount < value["excluded_brand_advertiser_id"].size(); uiCount++)
			{
				string sKey = value["excluded_brand_advertiser_id"][uiCount].asString();
				ctContent.add(sKey, AD_TANX_ADVERTISE);
			}
		}

		if (true == value.isMember("included_adboard_level"))
		{
			for(uiCount = 0; uiCount < value["included_adboard_level"].size(); uiCount++)
			{
				string sKey = value["included_adboard_level"][uiCount].asString();
				ctContent.add(sKey, AD_TANX_CREATIVE_LEVEL);
			}
		}
	}
	return AD_SUCCESS;
}

int  AdFilterTanx::ADFilterTanxParseDescreption(string & sStr,map<string, AdTanxContent>& mContent)
{
	UINT uiCount = 0;
	Json::Reader reader;
	Json::Value value;
	string sFilter;
	char sValue[AD_BYTE128];

	if (true == reader.parse((char *)sStr.c_str(), value))
	{
		if (true == value.isMember("filter_id"))
		{
			sFilter = value["filter_id"].asString();
		}
		else
		{
			return AD_FAILURE;
		}

		AdTanxDescription ctDes;
		ctDes.m_ctContent = mContent[sFilter];
		if (true == value.isMember("include_area"))
		{
			for(uiCount = 0; uiCount < value["include_area"].size(); uiCount++)
			{
				int iVal = value["include_area"][uiCount].asInt();
				snprintf(sValue, AD_BYTE128, "%d", iVal);
				string sKey = sValue;
				ctDes.m_vArea.push_back(sKey);
			}
		}

		if (true == value.isMember("include_interval"))
		{
			for(uiCount = 0; uiCount < value["include_interval"].size(); uiCount++)
			{
				int iVal = value["include_interval"][uiCount].asInt();
				snprintf(sValue, AD_BYTE128, "%d", iVal);
				string sKey = sValue;
				ctDes.m_vInterval.push_back(sKey);
			}
		}

		if (true == value.isMember("include_pid"))
		{
			for(uiCount = 0; uiCount < value["include_pid"].size(); uiCount++)
			{
				m_mPidDes[value["include_pid"][uiCount].asString()].push_back(ctDes);
			}
		}
	}
	return AD_SUCCESS;
}

int  AdFilterTanx::match(void * data, AD_RESULT & mResult)
{
	
	AD_ITER  adIter;
		
	list<void *>::iterator lvIter;
	
	char key[AD_BYTE128] = { 0 };
					
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);
	list<string>::iterator lsIter;
	list<AdTanxDescription>::iterator laIter;

	map<string, list<AdTanxDescription> >::iterator mIter;
	
		
	if( pRequest->m_uiAdx != AD_PROTOCOL_KEY_ADX_TANX) 
	{
		return AD_SUCCESS;
	}

	mIter = m_mPidDes.find(pRequest->m_sPId);
	if(mIter == m_mPidDes.end())
	{
		return AD_SUCCESS;
	}

	for(laIter = mIter->second.begin(); laIter != mIter->second.end();  laIter ++)
	{
		if((*laIter).m_vArea.empty() && (*laIter).m_vInterval.empty())
		{
			list<string> & ltCat = (*laIter).m_ctContent.get(AD_TANX_CATEGORY);
			for(lsIter = ltCat.begin(); lsIter != ltCat.end(); lsIter ++)
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
		}
	}
	
	return AD_SUCCESS;
}
int  AdFilterTanx::AddData(void * data)
{
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	char key[AD_BYTE128] = { 0 };

	if ( true  != pUnit->m_stTime.bPush)
	{
		return AD_SUCCESS;
	}

	list<string>::iterator  lsIter;
	
	map<UINT, AdCreativeInfo>::iterator mIter;	
	list<AdMaterialInfo>::iterator laIter;

	for(mIter = pUnit->m_mCreativeInfo.begin();mIter!=pUnit->m_mCreativeInfo.end();mIter ++)
	{
		if(mIter->second.m_sCreativeGroupId.empty())
			continue;
		for(laIter = mIter->second.m_ltMaterials.begin(); laIter != mIter->second.m_ltMaterials.end(); laIter ++)
		{
			//for media ban sensitivty category
			if(!(*laIter).m_sCategory.empty())
			{
				snprintf(key, AD_BYTE128,"CAT_%s_%s", mIter->second.m_sAdxId.c_str(), (*laIter).m_sCategory.c_str());
				update(key, static_cast<void *>(pUnit));
			}						
		}			
	}
	return AD_SUCCESS;
}
