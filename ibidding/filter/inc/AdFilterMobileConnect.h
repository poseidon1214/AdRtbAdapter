/***************************************
*	Description: Ad filter mobile platform:   mobile connect type.
*	Author: wilson
*	Date: 2015/07/27
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_MOBILE_CONNECT_H
#define _AD_FILTER_MOBILE_CONNECT_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"

using namespace std;


class AdFilterMobileConnect:public AdMap
{
public:
	AdFilterMobileConnect()
	{
		m_sName = "MobileConnectType";
	};
	virtual ~AdFilterMobileConnect(){};
	virtual int match(void * data, AD_RESULT & mResult);
	virtual int AddData(void * data);
	virtual string & name()
	{
		return m_sName;
	};

private:
	string m_sName;
};

#endif

