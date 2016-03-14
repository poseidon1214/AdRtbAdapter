/***************************************
*	Description: class Tanx Filter module.
*	Author: wilson
*	Date: 2015/12/09
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef	_AD_FILTER_TANX_H
#define	_AD_FILTER_TANX_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "AdGlobal.h"
#include "AdMap.h"

using namespace std;

#define AD_TANX_CATEGORY                			0
#define AD_TANX_LANDING_PAGE        		1
#define AD_TANX_CREATIVE_TYPE      		2
#define AD_TANX_ADVERTISE               			3
#define AD_TANX_CREATIVE_LEVEL    		4
#define AD_TANX_MAX                                                5

class AdTanxContent
{
public:
	AdTanxContent()
	{
		int i;
		list<string> ltfilte;
		for(i = 0; i < AD_TANX_MAX; i ++)
			m_vFilters.push_back(ltfilte);
	};
	~AdTanxContent()
	{
		m_vFilters.clear();
	};

	AdTanxContent& operator=(const AdTanxContent &a) 
	{
		this->m_vFilters = a.m_vFilters;
		return *this;
	}
	void clear()
	{
		int i;
		for(i = 0; i < AD_TANX_MAX; i ++)
			m_vFilters[i].clear();
	};
	void add(string & sStr, int iType)
	{
		if((iType < AD_TANX_MAX) && (iType >= 0))
			m_vFilters[iType].push_back(sStr);
	};
	list<string> & get(UINT uiType)
	{
		if(uiType < AD_TANX_MAX)
		{
			return m_vFilters[uiType];
		}
		return m_vFilters[AD_TANX_CATEGORY];
	};
private:
	vector<list<string> > m_vFilters;
};

class AdTanxDescription
{
public:
	AdTanxDescription(){};
	~AdTanxDescription(){};
	AdTanxContent m_ctContent;
	vector<string> m_vInterval;
	vector<string> m_vArea;
};


class AdFilterTanx:public AdMap
{
public:
	AdFilterTanx();
	~AdFilterTanx();
	virtual int match(void * data, AD_RESULT & mResult);
	virtual int AddData(void * data);
	virtual string & name()
	{
		return m_sName;
	};
	int AdFilterTanxInit(string &sFilter, string &sDes);

	AdFilterTanx& operator=(const AdFilterTanx &a) 
	{
		this->m_mPidDes = a.m_mPidDes;
		return *this;
	}
	
private:
	int ADFilterTanxParseContent(string & sStr, map<string, AdTanxContent>& mContent);
	int ADFilterTanxParseDescreption(string & sStr,  map<string, AdTanxContent>& mContent);
	map<string, list<AdTanxDescription> > m_mPidDes;
	string m_sName;
};


#endif /* _GTRTBLANP_H */

