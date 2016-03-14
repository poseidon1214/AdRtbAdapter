/***************************************
*	Description: class Landing page module,  prohibit some landing page bidding.
*	Author: wilson
*	Date: 2015/07/20
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#include "AdFilterLandingPage.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"

AdFilterLandingPage::AdFilterLandingPage() 
{
	m_sName = "LandingPage";
}

AdFilterLandingPage::~AdFilterLandingPage()
{
	map<string, list<string> >::iterator iter;
	for(iter = m_mLandPages.begin(); iter != m_mLandPages.end(); iter ++)
	{
		iter->second.clear();
	}
	m_mLandPages.clear();
}

list<string>* AdFilterLandingPage::AdFilterLandingPageSearch(string key)
{
	map<string, list<string> >::iterator iter = m_mLandPages.find(key);
	if(iter != m_mLandPages.end())
	{
		return &(iter->second);
	}
	return NULL;
}


int AdFilterLandingPage::AdFilterLandingPageInit(string &sFile)
{
	char line[AD_BYTE512] ;
	string sPid;
	string sUrl;
	string sTemp;
	string sStr;

	sPid.reserve(AD_BYTE64);
	sUrl.reserve(AD_BYTE512);
	sTemp.reserve(AD_BYTE512);
	sStr.reserve(AD_BYTE512);

	FILE * pFile = fopen(sFile.c_str(), "r");
	if (NULL == pFile) {
		AD_ERROR("%s fopen error\n", sFile.c_str());
		return AD_FAILURE;
	}
	

	/* LandingPage */
	while (NULL != fgets(line, AD_BYTE512, pFile))
	{
		sStr.clear();
		sStr = line;
		ADLandingPageParse(sStr, sPid, sUrl);
		
		list<string> & plLandp = m_mLandPages[sPid];
		
		plLandp.push_back(sUrl);
		sTemp = sPid;
	}

	fclose(pFile);

	cout<<"landing pages: "<<m_mLandPages.size()<<endl;

	return AD_SUCCESS;
}

void AdFilterLandingPage::ADLandingPageParse(string & sStr, string &sPid, string &sUrl)
{
	vector<string> vStr;
	AdString::StrSplit(sStr, vStr, "\t");
	if(vStr.size() == 2)
	{
		sPid = vStr[0];
		sStr = vStr[1];
		vStr.clear();
		AdString::StrSplit(sStr, vStr, "\n");
		if(vStr.size() >= 1)
		{
			sUrl = vStr[0];
		}
	}

	return;
}

void AdFilterLandingPage::display()
{
#if 0
	map<string, list<string> *>::iterator mIter;
	list<string>::iterator sIter;
	for(mIter = m_mLandPages.begin(); mIter != m_mLandPages.end(); mIter ++)
	{
		AD_PRINT("PID: %s, ", mIter->first);
		GT_PRINT("URL: %d \n", mIter->second->size());
		for(sIter = mIter->second->begin(); sIter != mIter->second->end(); sIter ++)
		{
			GT_PRINT("	%s\n", *sIter);
		}
	}
#endif

	return;
}

//void GtRtbUrlParse(char* pSrc, char* pDst){
void AdFilterLandingPage:: AdFilterLandingPageUrlParse(string &sSrc, vector<string> &vStr)
{
	vector<string> vStrTemp;
	vector<string>::iterator iter;
	string sTemp;
	AdString::StrSplit(sSrc, vStrTemp, "//");
	if(vStrTemp.size() >= 2)
	{
		sTemp = vStrTemp[1];
		vStrTemp.clear();
	}
	else
		return;
	AdString::StrSplit(sTemp, vStrTemp, "/");
	if(vStrTemp.size() >= 1)
	{
		sTemp = vStrTemp[0];
		vStrTemp.clear();
	}
	else
		return;

	AdString::StrSplit(sTemp, vStrTemp, "?");
	if(vStrTemp.size() >= 1)
	{
		sTemp = vStrTemp[0];
		vStrTemp.clear();
	}
	else
		return;

	AdString::StrSplit(sTemp, vStrTemp, ".");
	if(vStrTemp.empty())
		return;
	while(vStrTemp.size() > 1)
	{
		sTemp.clear();
		for(iter = vStrTemp.begin(); iter != vStrTemp.end(); iter ++)
		{
			if(sTemp.empty())
			{
				sTemp = *iter;
			}
			else
			{
				sTemp += "." + *iter;
			}
		}
		vStrTemp.erase(vStrTemp.begin());
		vStr.push_back(sTemp);
	}
}


int AdFilterLandingPage::match(void * data, AD_RESULT & mResult)
{
	list<string> * pVec;
	list<string>::iterator vIter;

	AD_ITER  adIter;
	
	list<void *>::iterator listIter;
				
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);
	
	if( pRequest->m_uiAdx == AD_PROTOCOL_KEY_ADX_BAIDU ) 
	{
		 pVec = AdFilterLandingPageSearch(pRequest->m_sPId);	
		if (pVec != NULL) 
		{
			for(vIter = pVec->begin(); vIter != pVec->end(); vIter++)
			{
				adIter = find(*vIter);
				if(end() != adIter)
				{
					list<void *> & list = adIter->second;
					for(listIter = list.begin(); listIter != list.end(); listIter ++)
					{
						mResult.erase(*listIter);
					}
				}
			}
		}
	}

	return AD_SUCCESS;
}
int AdFilterLandingPage::AddData(void * data)
{
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	map<UINT, AdCreativeInfo> ::iterator mIter;
	vector<string> vStr;
	vector<string>::iterator vIter;


	if ( true  != pUnit->m_stTime.bPush)
	{
		return AD_SUCCESS;;
	}

	for(mIter = pUnit->m_mCreativeInfo.begin();mIter!=pUnit->m_mCreativeInfo.end();mIter ++)
	{
		if(mIter->second.m_ltMaterials.empty()) 
			continue;
		else
		{
			vStr.clear();
			AdFilterLandingPageUrlParse((*(mIter->second.m_ltMaterials.begin())).m_sDestinationUrl, vStr);
			for(vIter = vStr.begin(); vIter != vStr.end(); vIter ++)
			{
				update(const_cast<char *>((*vIter).c_str()),static_cast<void *>(pUnit));
			}
		}
	}		
					
	return AD_SUCCESS;
}
