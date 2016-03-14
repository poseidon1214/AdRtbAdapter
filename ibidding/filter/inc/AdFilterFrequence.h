/***************************************
*	Description: Ad filter for frequence
*	Author: wilson
*	Date: 2015/07/28
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_FREQUENCE_H
#define _AD_FILTER_FREQUENCE_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"
#include "AdUdp.h"


using namespace std;

class AdFilterFrequence:public AdMap
{
public:
	AdFilterFrequence();
	virtual ~AdFilterFrequence();
	virtual int match(void * data, AD_RESULT & mResult);
	virtual int AddData(void * data);
	virtual string & name()
	{
		return m_sName;
	};
	
	int AdFilterFrequenceInit(map<string, string> & mServer);

	AdFilterFrequence& operator=(const AdFilterFrequence &a) 
	{
		this->m_vIp = a.m_vIp;
		this->m_vPort = a.m_vPort;
		return *this;
	};

private:
	string m_sName;
	vector<string> m_vIp;
	vector<int>      m_vPort;
};

#endif

