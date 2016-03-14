/***************************************
*	Description:  ssp Data, store all Pid info
*	Author: wilson
*	Date: 2016/03/03
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef __AD_DATA_H__
#define __AD_DATA_H__
#include <string>
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include "AdGlobal.h"
#include "AdRtbConfig.h"
#include "AdBuff.h"
#include "AdPid.h"

using namespace std;

class AdData
{
public:
	AdData();
	virtual ~AdData();

	int Init(AdRtbConfig & cConfig);

	int Update();

	int GetPid(string & sId, AdPid & cPid);
	int Parse(string & sData, map<string, AdPid> & cPids);

private:
	string m_sIp;
	INT32  m_iPort;
	AdPingPongBuffer<map<string,AdPid> > m_cPids;
	int m_iLastTimeStamp;
};

#endif

