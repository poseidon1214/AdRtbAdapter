/***************************************
*	Description: class ctr module,  use by bid price module.
*	Author: wilson
*	Date: 2015/07/30
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/


#ifndef	_AD_RTB_CTR_H
#define	_AD_RTB_CTR_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <stdio.h>
#include "AdGlobal.h"
#include "AdUtil.h"

using namespace std;  

class AdRtbCtrData
{
public:
	AdRtbCtrData()
	{
		clear();
	};
	~AdRtbCtrData()
	{
		clear();
	};
	void clear()
	{
		//m_sPID.clear();
		//m_sMID.clear();
		//m_sImpression.clear();
		//m_sClick.clear();
		//m_sCtr.clear();
		//m_vCtrInfo.clear();
		m_fCtrVal = 0.0;
		m_uiClick = 0;
	};
	vector<string> m_vCtrInfo;
	float m_fCtrVal;
	UINT m_uiClick;
};

class AdRtbCtr
{
public:
	AdRtbCtr();
	~AdRtbCtr();
	int AdRtbCtrInit(string &sFile);
	AdRtbCtrData * AdRtbCtrSearch(string & key);

protected:
       void AdRtbCtrParse(string & sStr, vector<string> & vResult);
	
private:
	map<string, AdRtbCtrData> m_mCtr;
};


#endif /* _GTRTBLANP_H */

