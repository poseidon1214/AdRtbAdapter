/***************************************
*	Description: Rtb Filters.
*	Author: wilson
*	Date: 2015/07/21
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <vector>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include "AdRtbFilters.h"
#include "AdMap.h"

#define AD_FILTERID_TIME				0
#define AD_FILTERID_ADEXCHANGE			1
#define AD_FILTERID_PID					2
#define AD_FILTERID_AREA				3	
#define AD_FILTERID_SIZE				4	
#define AD_FILTERID_AGENT				5

#define AD_FILTERID_MOBILEOPERATOR		6
#define AD_FILTERID_MOBILEDEVICE		7
#define AD_FILTERID_MOBILECONNECT		8
#define AD_FILTERID_PARAMETER			9
#define AD_FILTERID_BAN					10
#define AD_FILTERID_LANDINGPAGE			11
#define AD_FILTERID_TANX				12
#define AD_FILTERID_REALTIME			13
#define AD_FILTERID_FREQUENCE			14
#define AD_FILTERID_MARK				15


#define AD_FILTERID_MAX                 16


/***********************basic func****************/
AdRtbFilters::AdRtbFilters()
{
}

AdRtbFilters::~AdRtbFilters()
{
	FiltersClear();
	m_vFilters.Switch();
	FiltersClear();
}

void AdRtbFilters::FiltersClear()
{
	vector<AdMap *>::iterator iter;
	vector<AdMap *> & vBak = m_vFilters.GetBack();
	vector<AdMap *> & vNow = m_vFilters.Get();
	if(vBak.empty())
	{
		return;
	}

	for(iter = vBak.begin(); iter != vBak.end(); iter ++ )
	{
		if(*iter != static_cast<AdMap *> (NULL))
		{
			if(find(vNow.begin(), vNow.end(), *iter) == vNow.end())
			{
				delete *iter;
			}
		}
	}
	vBak.clear();
}


int AdRtbFilters::FiltersPrintRealTime(UINT iEid)
{
	AdMap * pMap =m_vFilters.Get()[AD_FILTERID_REALTIME];
	return (static_cast<AdFilterRealTime *>(pMap))->AdFilterRealTimePrint(iEid);
}

/***********************Filters Update ****************/

int AdRtbFilters::FiltersCreate(UINT uiFlags)
{
	int ret;
	//Filter 0  time 
	ret = FiltersCreateFilter<AdFilterTime>(uiFlags & AD_FILTERS_TIME);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterTime create Fail\n");
		return AD_FAILURE;
	}
	//Filter 1 adexchange
	ret = FiltersCreateFilter<AdFilterAdExchange>(uiFlags & AD_FILTERS_ADEXCHANGE);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterAdExchange create Fail\n");
		return AD_FAILURE;
	}
	//Filter 2 Pid
	ret = FiltersCreateFilter<AdFilterPid>(uiFlags & AD_FILTERS_PID);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("FAdFilterPid create Fail\n");
		return AD_FAILURE;
	}
	//Filter 3 Area
	ret = FiltersCreateFilter<AdFilterArea>(uiFlags & AD_FILTERS_AREA);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterArea create Fail\n");
		return AD_FAILURE;
	}
	
	//Filter 4 size
	ret = FiltersCreateFilter<AdFilterSize>(uiFlags & AD_FILTERS_SIZE);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("FAdFilterSize create Fail\n");
		return AD_FAILURE;
	}
	
	//Filter 5 useragent
	ret = FiltersCreateFilter<AdFilterUserAgent>(uiFlags & AD_FILTERS_AGENT);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("FAdFilterAgent create Fail\n");
		return AD_FAILURE;
	}
	
	//Filter 6 mobile operator
	ret = FiltersCreateFilter<AdFilterMobileOperator>(uiFlags & AD_FILTERS_MOBILEOPERATOR);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("FAdFilterMobileOperator create Fail\n");
		return AD_FAILURE;
	}
	//Filter 7 mobile device
	ret = FiltersCreateFilter<AdFilterMobileDevice>(uiFlags & AD_FILTERS_MOBILEDEVICE);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterMobileDevice create Fail\n");
		return AD_FAILURE;
	}
	//Filter 8 mobile connect type
	ret = FiltersCreateFilter<AdFilterMobileConnect>(uiFlags & AD_FILTERS_MOBILECONNECT);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterMobileConnect create Fail\n");
		return AD_FAILURE;
	}
	//Filter 9 Parameter
	ret = FiltersCreateFilter<AdFilterParameter>(uiFlags & AD_FILTERS_PARAMETER);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterMobileConnect create Fail\n");
		return AD_FAILURE;
	}

	//Filter 10 ban list
	ret = FiltersCreateFilter<AdFilterBan>(uiFlags & AD_FILTERS_BAN);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterBan create Fail\n");
		return AD_FAILURE;
	}
	//Filter 11  landing page
	ret = FiltersCreateFilter<AdFilterLandingPage>(uiFlags & AD_FILTERS_LANDINGPAGE);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterLandingPage create Fail\n");
		return AD_FAILURE;
	}

	//Filter 12  Tanx
	ret = FiltersCreateFilter<AdFilterTanx>(uiFlags & AD_FILTERS_TANX);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterTanx create Fail\n");
		return AD_FAILURE;
	}
	
	//Filter 13  RealTime
	ret = FiltersCreateFilter<AdFilterRealTime>(0);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterRealTime create Fail\n");
		return AD_FAILURE;
	}

	
	//make sure filter exist, update
	if(uiFlags & AD_FILTERS_REALTIME)
	{
		vector<AdMap *> & vBack = m_vFilters.GetBack();
		vector<AdMap *> & vNow = m_vFilters.Get();
		if((vBack[AD_FILTERID_REALTIME]  == NULL) && (vNow[AD_FILTERID_REALTIME]  != NULL) )
			vBack[AD_FILTERID_REALTIME]  = vNow[AD_FILTERID_REALTIME];
	}
	
	//Filter 14 frequence
	ret = FiltersCreateFilter<AdFilterFrequence>(uiFlags & AD_FILTERS_FREQUENCE);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterFrequence create Fail\n");
		return AD_FAILURE;
	}

	//Filter 15 mark
	ret = FiltersCreateFilter<AdFilterMark>(uiFlags & AD_FILTERS_MARK);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("AdFilterFrequence create Fail\n");
		return AD_FAILURE;
	}

	return AD_SUCCESS;
}

int AdRtbFilters::FiltersInit(UINT uiFlags, AdRtbConfig & ctConf)
{
	int i;
	vector<AdMap *> & vBack = m_vFilters.GetBack();
	vector<AdMap *> & vNow = m_vFilters.Get();
	int ret;
	string sFile;
	string sSec = "[CONFIG]";
	ADCONFIG &  config = ctConf.Get(sSec);
	sSec = "[FREQUENCE]";
	ADCONFIG &  freq = ctConf.Get(sSec);
	sSec = "[ICACHE]";
	ADCONFIG &  cache = ctConf.Get(sSec);

	//init Filter landingpage, which need configure file, and if config file no change,  
	//if(uiFlags & AD_FILTERS_LANDINGPAGE)
	if(0)
	{
		AdFilterLandingPage * pLp = static_cast<AdFilterLandingPage *>(vBack[AD_FILTERID_LANDINGPAGE]);
		if(pLp != NULL)
		{
			sFile = "LANDINGPAGE";
			if(ctConf.FileIsChanged(sFile))
			{
				ret = pLp->AdFilterLandingPageInit(config[sFile]);
				if(ret != AD_SUCCESS)
				{
					AD_ERROR("AdFilterLandingPage Init Fail\n");
					return AD_FAILURE;
				}
			}
			else
			{
				//copy config data from old filter
				*pLp = *(static_cast<AdFilterLandingPage *>(vNow[AD_FILTERID_LANDINGPAGE]));
			}
		}
		else
		{
			AD_ERROR("AdFilterLandingPage create Fail\n");
			return AD_FAILURE;
		}
	}

	//init Filter landingpage, which need configure file, and if config file no change,  
	if(uiFlags & AD_FILTERS_TANX)
	{
		AdFilterTanx* pTanx = static_cast<AdFilterTanx*>(vBack[AD_FILTERID_TANX]);
		if(pTanx != NULL)
		{
			sFile = "TANXCONTENT";
			string sFile2 = "TANXDESCRIPTION";
			if(ctConf.FileIsChanged(sFile) || ctConf.FileIsChanged(sFile2))
			{
				ret = pTanx->AdFilterTanxInit(config[sFile], config[sFile2]);
				if(ret != AD_SUCCESS)
				{
					AD_ERROR("AdFilterTanx Init Fail\n");
					return AD_FAILURE;
				}
			}
			else
			{
				//copy config data from old filter
				*pTanx = *(static_cast<AdFilterTanx*>(vNow[AD_FILTERID_TANX]));
			}
		}
		else
		{
			AD_ERROR("AdFilterTanx create Fail\n");
			return AD_FAILURE;
		}
	}
	
	if(vNow.empty())
	{
		AdFilterRealTime * pRt = static_cast<AdFilterRealTime *>(vBack[AD_FILTERID_REALTIME]);
		if(pRt != NULL)
		{
			string sUrl = "238.2.20.128";
			ret = pRt->AdFilterRealTimeInit(sUrl, 14331);
			if(ret != AD_SUCCESS)
			{
				AD_ERROR("AdFilterRealTime Init Fail\n");
				return AD_FAILURE;
			}
		}
		else
		{
			AD_ERROR("AdFilterRealTime create Fail\n");
			return AD_FAILURE;
		}
	}

	
	if(uiFlags & AD_FILTERS_FREQUENCE)
	{
		AdFilterFrequence * pFre = static_cast<AdFilterFrequence *>(vBack[AD_FILTERID_FREQUENCE]);
		if(pFre != NULL)
		{
			if(ctConf.FileIsChanged())
			{
				ret = pFre->AdFilterFrequenceInit(freq);
				if(ret != AD_SUCCESS)
				{
					AD_ERROR("AdFilterFrequence Init Fail\n");
					return AD_FAILURE;
				}
			}
			else
			{
				//use old config data init
				*pFre = *(static_cast<AdFilterFrequence*>(vNow[AD_FILTERID_FREQUENCE]));
			}
		}
		else
		{
			AD_ERROR("AdFilterFrequence create Fail\n");
			return AD_FAILURE;
		}
	}
	
	//init Filter Mark, which need configure file
	if(uiFlags & AD_FILTERS_MARK)
	{
		AdFilterMark* pMark = static_cast<AdFilterMark*>(vBack[AD_FILTERID_MARK]);
		if(pMark != NULL)
		{
			if(ctConf.FileIsChanged())
			{
				ret = pMark->AdFilterMarkInit(cache);
				if(ret != AD_SUCCESS)
				{
					AD_ERROR("AdFilterMark Init Fail\n");
					return AD_FAILURE;
				}
			}
			else
			{
				//use old config data init
				ret = pMark->AdFilterMarkInit(static_cast<AdFilterMark *>(vNow[AD_FILTERID_MARK]));
				if(ret != AD_SUCCESS)
				{
					AD_ERROR("AdFilterMark Init Fail\n");
					return AD_FAILURE;
				}
			}
		}
		else
		{
			AD_ERROR("AdFilterMark create Fail\n");
			return AD_FAILURE;
		}
	}


	//update no reinit filter, copy from main cache
	for(i = 0; i < AD_FILTERID_MAX; i ++)
	{
		if(vBack[i] == NULL)
			vBack[i] = vNow[i];
	}

	return AD_SUCCESS;
}


int  AdRtbFilters::FiltersUpdate(map<UINT, AdPlanningUnit*> & mData, AdRtbConfig&  ctConf)
{
	vector<AdMap *> & filters = m_vFilters.GetBack();
	vector<AdMap *>::iterator fIter;
	map<UINT, AdPlanningUnit*>::iterator dIter;	
	int ret;

	//clear back cache,  for update for 
	FiltersClear();

	//create filter which define by flags, and add new filter for back cache, 
	ret = FiltersCreate(AD_FILTERS_ALL);
	if(ret != AD_SUCCESS)
		return ret;

	//update data for new filter
	for(dIter = mData.begin(); dIter != mData.end(); dIter ++)
	{
		for(fIter = filters.begin(); fIter != filters.end(); fIter ++)
		{
			if(*fIter != NULL)
			{
				(*fIter)->AddData(static_cast<void *>(dIter->second));
			}
		}
	}

	//update new filters other data, include file data, and reload no update filter
	ret = FiltersInit(AD_FILTERS_ALL, ctConf);
	if(ret != AD_SUCCESS)
		return ret;

	//switch back cache to main cache
	m_vFilters.Switch();
	return AD_SUCCESS;
}

/***********************Filters match ****************/
#include "AdRtbBase.h"

void AdRtbFilters::FiltersMatch(void * data, AD_RESULT & mResult, bool bPrint)
{
	vector<AdMap *> & filters = m_vFilters.Get();
	vector<AdMap *>::iterator fIter;
	AdRtbRequest *pRequest = static_cast<AdRtbRequest *>(data);
	for(fIter = filters.begin(); fIter != filters.end(); fIter ++)
	{
		if(*fIter != NULL)
        	{
				(*fIter)->match(data,mResult);
				if(bPrint == true)
				{
					AD_PRINT("Filter:  %s  %d(%d ns)\n", ((*fIter)->name()).c_str(),
							static_cast<int>(mResult.size()), pRequest->getLife());
					Result::display(mResult);
				}
				if(mResult.empty())
					break;
        	}
	}
}
