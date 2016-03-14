/***************************************
*	Description: Ad filter mobile platform:   mobile device type.
*	Author: wilson
*	Date: 2015/07/27
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_MOBILE_DEVICE_H
#define _AD_FILTER_MOBILE_DEVICE_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"

using namespace std;


class AdFilterMobileDevice:public AdMap
{
public:
	AdFilterMobileDevice()
	{
		m_sName = "MobileDeviceType";
	};
	virtual ~AdFilterMobileDevice(){};
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

