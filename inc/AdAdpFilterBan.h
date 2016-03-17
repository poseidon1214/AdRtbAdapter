/***************************************
*	Description: Ad filter for ban list.
*	Author: wilson
*	Date: 2015/07/25
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_ADP_FILTER_BAN_H
#define _AD_ADP_FILTER_BAN_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdBuff.h"
#include "AdUtil.h"
#include "AdRtbBase.h"

using namespace std;

class AdAdpFilterBan
{
public:
	AdAdpFilterBan();
	virtual ~AdAdpFilterBan();

	int Init(string & sServer, string passwd = "");
	bool Match(AdRtbRequest * pRequest);
	int Update();
	
private:
	AdPingPongBuffer<map<string, UINT> >m_mBan;
	string  m_sIp;
	string m_sPasswd;
	int  m_iPort;
};

#endif

