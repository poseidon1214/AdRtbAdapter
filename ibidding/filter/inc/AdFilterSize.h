/***************************************
*	Description: Ad filter for Material size
*	Author: wilson
*	Date: 2015/07/25
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_SIZE_H
#define _AD_FILTER_SIZE_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdMap.h"

using namespace std;

class AdFilterSize:public AdMap
{
public:
	AdFilterSize()
	{
		m_sName = "Size";
	};
	virtual ~AdFilterSize(){};
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

