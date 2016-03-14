/***************************************
*	Description: class protect module: bid rate.   use by bid price module.
*	Author: wilson
*	Date: 2015/07/30
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <stdlib.h>
#include "AdRtbProtect.h"

#define  AD_PROTECT_ID_LEAGUEID           	0
#define  AD_PROTECT_ID_PID                      		1
#define  AD_PROTECT_ID_WEIGHT                      2
#define  AD_PROTECT_ID_MAX                     		3

AdRtbProtect::AdRtbProtect() 
{
}

AdRtbProtect::~AdRtbProtect()
{
	m_mProtect.clear();
}

int  AdRtbProtect::AdRtbProtectSearch(string  &key)
{
	map<string, AdRtbProtectData>::iterator iter = m_mProtect.find(key);
	if(iter != m_mProtect.end())
	{
		return iter->second.m_uiWeight;
	}
	return AD_FAILURE;
}


int AdRtbProtect::AdRtbProtectInit(string &sFile)
{
	char line[AD_BYTE512] ;
	char key[AD_BYTE128];
	string sStr;

	AdRtbProtectData sData;

	FILE * pFile = fopen(sFile.c_str(), "r");
	if (NULL == pFile) {
		AD_ERROR("%s fopen error\n", sFile.c_str());
		return AD_FAILURE;
	}

	m_mProtect.clear();
	/* LandingPage */
	while (NULL != fgets(line, AD_BYTE512, pFile))
	{
		sStr.clear();
		sStr = line;
		sData.m_vProtectInfo.clear();
		AdRtbProtectParse(sStr, sData.m_vProtectInfo);
		
		if(sData.m_vProtectInfo.size() >= AD_PROTECT_ID_MAX)
		{
			sData.m_uiWeight= atoi(sData.m_vProtectInfo[AD_PROTECT_ID_WEIGHT].c_str());
			snprintf(key, AD_BYTE128, "%s%s", sData.m_vProtectInfo[AD_PROTECT_ID_LEAGUEID].c_str(),\
				sData.m_vProtectInfo[AD_PROTECT_ID_PID].c_str());
			m_mProtect[key] = sData;
		}
	}

	fclose(pFile);

	cout<<"Protect: "<<m_mProtect.size()<<endl;

	return AD_SUCCESS;
}

void AdRtbProtect::AdRtbProtectParse(string & sStr, vector<string> & vResult)
{
	vector<string> vStr;
	AdString::StrSplit(sStr, vResult, "\t");
	if(vStr.size() == AD_PROTECT_ID_MAX)
	{
		AdString::StrSplit(vResult[AD_PROTECT_ID_MAX - 1], vStr, "\n");
		if(vStr.size() >= 1)
		{
			vResult[AD_PROTECT_ID_MAX - 1]= vStr[0];
		}
	}

	return;
}
