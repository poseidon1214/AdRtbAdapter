/***************************************
*	Description: class ctr module,  use by bid price module.
*	Author: wilson
*	Date: 2015/07/30
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <stdlib.h>
#include "AdRtbCtr.h"

#define  AD_CTR_ID_PID           		0
#define  AD_CTR_ID_MID           		1
#define  AD_CTR_ID_IMPRESS              2
#define  AD_CTR_ID_CLICK                3
#define  AD_CTR_ID_CTR                  4
#define  AD_CTR_ID_MAX                  5

AdRtbCtr::AdRtbCtr() 
{
}

AdRtbCtr::~AdRtbCtr()
{
	m_mCtr.clear();
}

AdRtbCtrData * AdRtbCtr::AdRtbCtrSearch(string& key)
{
	map<string, AdRtbCtrData>::iterator iter = m_mCtr.find(key);
	if(iter != m_mCtr.end())
	{
		return &(iter->second);
	}
	return NULL;
}


int AdRtbCtr::AdRtbCtrInit(string &sFile)
{
	char line[AD_BYTE512] ;
	string sStr;

	AdRtbCtrData  sData;

	FILE * pFile = fopen(sFile.c_str(), "r");
	if (NULL == pFile) {
		AD_ERROR("%s fopen error\n", sFile.c_str());
		return AD_FAILURE;
	}

	m_mCtr.clear();
	/* LandingPage */
	while (NULL != fgets(line, AD_BYTE512, pFile))
	{
		sStr.clear();
		sStr = line;
		sData.m_vCtrInfo.clear();
		AdRtbCtrParse(sStr, sData.m_vCtrInfo);
		
		if(sData.m_vCtrInfo.size() >= AD_CTR_ID_MAX)
		{
			sData.m_fCtrVal = atof(sData.m_vCtrInfo[AD_CTR_ID_CTR].c_str());
			sData.m_uiClick= atoi(sData.m_vCtrInfo[AD_CTR_ID_CLICK].c_str());
			m_mCtr[sData.m_vCtrInfo[AD_CTR_ID_PID]] = sData;
		}
	}

	fclose(pFile);

	cout<<"CTR: "<<m_mCtr.size()<<endl;

	return AD_SUCCESS;
}

void AdRtbCtr::AdRtbCtrParse(string & sStr, vector<string> & vResult)
{
	vector<string> vStr;
	AdString::StrSplit(sStr, vResult, "\t");
	if(vStr.size() == AD_CTR_ID_MAX)
	{
		AdString::StrSplit(vResult[AD_CTR_ID_MAX - 1], vStr, "\n");
		if(vStr.size() >= 1)
		{
			vResult[AD_CTR_ID_MAX - 1]= vStr[0];
		}
	}

	return;
}
