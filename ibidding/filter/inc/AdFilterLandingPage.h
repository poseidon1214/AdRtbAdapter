/***************************************
*	Description: class Landing page module,  prohibit some landing page bidding.
*	Author: wilson
*	Date: 2015/07/20
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/


#ifndef	_AD_FILTER_LANDING_PAGE_H
#define	_AD_FILTER_LANDING_PAGE_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "AdGlobal.h"
#include "AdMap.h"

using namespace std;  

class AdFilterLandingPage:public AdMap
{
public:
	AdFilterLandingPage();
	~AdFilterLandingPage();
	virtual void display();
	virtual int match(void * data, AD_RESULT & mResult);
	virtual int AddData(void * data);
	virtual string & name()
	{
		return m_sName;
	};
	int AdFilterLandingPageInit(string &sFile);

	AdFilterLandingPage& operator=(const AdFilterLandingPage &a) 
	{
		this->m_mLandPages = a.m_mLandPages;
		return *this;
	}
	

protected:
	list<string>* AdFilterLandingPageSearch(string key);
	
private:
	void ADLandingPageParse(string & sStr, string &sPid, string &sUrl);
	void AdFilterLandingPageUrlParse(string &sSrc, vector<string> &vStr);
	map<string, list<string> > m_mLandPages;
	string m_sName;
};


#endif /* _GTRTBLANP_H */

