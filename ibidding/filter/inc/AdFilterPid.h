/***************************************
*	Description: Ad Pid filter.
*	Author: wilson
*	Date: 2015/07/20
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_PID_H_
#define _AD_FILTER_PID_H_

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"

using namespace std;


class AdFilterPid:public AdMap
{
public:
	AdFilterPid()
	{
		m_sName = "Pid";
	};
	virtual ~AdFilterPid(){};
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
