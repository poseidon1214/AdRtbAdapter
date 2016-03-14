/***************************************
*	Description: Ad filter for Area, include transfer with ip and area
*	Author: wilson
*	Date: 2015/07/27
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_AREA_H
#define _AD_FILTER_AREA_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"

using namespace std;

class AdFilterArea:public AdMap
{
public:
	AdFilterArea();
	virtual ~AdFilterArea();
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

