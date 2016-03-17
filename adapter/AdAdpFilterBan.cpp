/***************************************
*	Description: Ad filter for ban list.
*	Author: wilson
*	Date: 2015/07/25
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <stdlib.h>	
#include "AdAdpFilterBan.h"
#include "AdRedisCli.h"


AdAdpFilterBan::AdAdpFilterBan()
{
	m_sIp.clear();
	m_iPort = AD_FAILURE;
}

AdAdpFilterBan::~AdAdpFilterBan()
{
	m_mBan.Get().clear();
	m_mBan.GetBack().clear();
}

bool AdAdpFilterBan::Match(AdRtbRequest * pRequest)
{
	bool b_PID = false;
	bool b_IP = false;
	map<string, UINT> & mBan = m_mBan.Get();
	if (pRequest->m_sPId.size() > 0 )
	{
		char key[AD_BYTE256];
		snprintf(key, AD_BYTE256, "%d+%s", pRequest->m_uiAdx, pRequest->m_sPId.c_str());
		b_PID = (mBan.find(key) != mBan.end());
	}
	if (pRequest->m_sUserIP.size() > 0 )
	{
		b_IP = (mBan.find(pRequest->m_sUserIP) != mBan.end());
	}	
	return b_IP && b_IP;
}


int AdAdpFilterBan::Init(string & sServer, string passwd)
{
	int ret;
	vector<string> vStr;
	AdString::StrSplit(sServer, vStr, ":");
	if(vStr.size() != 2)
		return AD_FAILURE;
	
	m_sIp = vStr[0];
	m_iPort = atoi(vStr[1].c_str());
	m_sPasswd = passwd;
	ret = Update();
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Ban Init error\n");
		return ret;
	}
	return AD_SUCCESS;
}

int AdAdpFilterBan::Update()
{
	AdRedisCli ctRedis;
	int ret;
	
	list<string> lKeys;
	string sKey;
	list<string>::iterator iter;
	map<string, UINT> & mBan = m_mBan.GetBack();
	
	ret = ctRedis.AdRedisConnect(m_sIp, m_iPort, m_sPasswd);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Redis connect error\n");
		return ret;
	}

	//update  ban info
	sKey = "global:ban";
	ret = ctRedis.AdRedisHkeys(sKey, lKeys);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("redis error\n");
		goto RETURN;
	}
	
	//clear cache old data;
	mBan.clear();

	for(iter = lKeys.begin(); iter != lKeys.end(); iter ++)
	{
		mBan[*iter] = 1;
	}

RETURN:
	ctRedis.AdRedisClose();
	if(ret == AD_SUCCESS)
		m_mBan.Switch();
	return AD_SUCCESS;
}
