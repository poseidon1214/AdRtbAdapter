/***************************************
*	Description: Ad filter:  popup result depand on Parameter
*	Author: wilson
*	Date: 2015/07/30
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_PARAMETER_H
#define _AD_FILTER_PARAMETER_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"

using namespace std;


class AdFilterParameter:public AdMap
{
public:
	AdFilterParameter()
	{
		m_sName = "Parameter";
	};
	virtual ~AdFilterParameter(){};
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

