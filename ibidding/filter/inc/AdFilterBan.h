/***************************************
*	Description: Ad filter for ban list.
*	Author: wilson
*	Date: 2015/07/25
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_BAN_H
#define _AD_FILTER_BAN_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"

using namespace std;

class AdFilterBan:public AdMap
{
public:
	AdFilterBan()
	{
		m_sName = "BanList";
	};
	virtual ~AdFilterBan(){};
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

