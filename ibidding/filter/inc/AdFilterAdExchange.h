/***************************************
*	Description: Ad filter for adexchange.
*	Author: wilson
*	Date: 2015/07/20
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_ADEXCHANGE_H
#define _AD_FILTER_ADEXCHANGE_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"

using namespace std;


class AdFilterAdExchange:public AdMap
{
public:
	AdFilterAdExchange()
	{
		m_sName = "AdExchange";
	};
	virtual ~AdFilterAdExchange(){};
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
