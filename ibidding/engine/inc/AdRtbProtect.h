/***************************************
*	Description: class protect module: bid rate.   use by bid price module.
*	Author: wilson
*	Date: 2015/07/30
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/


#ifndef	_AD_RTB_PROTECT_H
#define	_AD_RTB_PROTECT_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <stdio.h>
#include "AdGlobal.h"
#include "AdUtil.h"

using namespace std;  

class AdRtbProtectData
{
public:
	AdRtbProtectData()
	{
		clear();
	};
	~AdRtbProtectData()
	{
		clear();
	};
	void clear()
	{
		m_vProtectInfo.clear();
		m_uiWeight = 0;
	};
	vector<string> m_vProtectInfo;
	UINT m_uiWeight;
};

class AdRtbProtect
{
public:
	AdRtbProtect();
	~AdRtbProtect();
	int AdRtbProtectInit(string &sFile);
	int AdRtbProtectSearch(string & key);

protected:
        void AdRtbProtectParse(string & sStr, vector<string> & vResult);
	
private:
	map<string, AdRtbProtectData> m_mProtect;
};


#endif /* _GTRTBLANP_H */

