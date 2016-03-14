/***************************************
*	Description:  ssp Data, store all Pid info
*	Author: wilson
*	Date: 2016/03/03
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdData.h"
#include "AdRedisCli.h"
#include "json/json.h"

AdData::AdData()
{
	m_sIp.clear();
	m_iPort = AD_FAILURE;
	m_iLastTimeStamp = AD_FAILURE;
}

AdData::~AdData()
{
	m_sIp.clear();
	m_iPort = AD_FAILURE;
}

//init config
int AdData::Init(AdRtbConfig & cConfig)
{
	string sSec = "[CONFIG]";
	ADCONFIG & gConfig = cConfig.Get(sSec);
	ADCONFIG_ITER iter = gConfig.find("REDIS");
	vector<string> vStr;
	if(iter != gConfig.end())
	{
		AdString::StrSplit(iter->second, vStr, ":");
		if(vStr.size() == 2)
		{
			m_sIp = vStr[0];
			m_iPort = atoi(vStr[1].c_str());
		}
	}

	return AD_SUCCESS;
}

int AdData::Parse(string & sData, map<string, AdPid> & cPids)
{
	AdPid cPid;

	UINT uiCount = 0;
	Json::Reader reader;
	Json::Value value;
	Json::Value subvalue;
	Json::Value grandvalue;
	int iReturn = AD_SUCCESS;

	if (true == reader.parse((char *)sData.c_str(), value))
	{
		if(true == value.isMember("mid"))
		{
			cPid.m_iMid = value["mid"].asInt();
		}
		if(cPid.m_iMid <= 0)
		{
			iReturn = AD_FAILURE;
		}
		
		if (true == value.isMember("pid"))
		{
			cPid.m_sPid = value["pid"].asString();
		}
		if(cPid.m_sPid.empty())
		{
			iReturn = AD_FAILURE;
		}

		if (true == value.isMember("size"))
		{
			cPid.m_sSize= value["size"].asString();
			vector<string> vStr;
			AdString::StrSplit(cPid.m_sSize, vStr, "x");
			if(vStr.size() == 2)
			{
				cPid.m_iWidth = atoi(vStr[0].c_str());
				cPid.m_iHeight = atoi(vStr[1].c_str());
			}
			else
				iReturn = AD_FAILURE;
		}
		
		if(cPid.m_sSize.empty())
		{
			iReturn = AD_FAILURE;
		}

		if (true == value.isMember("bidfloor"))
		{
			cPid.m_fBidFloor= value["bidfloor"].asDouble();
		}

		if (true == value.isMember("pos"))
		{
			cPid.m_iPos = value["pos"].asInt();
		}

		if (true == value.isMember("class"))
		{
			cPid.m_sClass = value["class"].asString();
		}

		if (true == value.isMember("type"))
		{
			cPid.m_sAdType = value["type"].asString();
		}
		
		if (true == value.isMember("page"))
		{
			cPid.m_sPage= value["page"].asString();
		}

		if (true == value.isMember("keywords"))
		{
			cPid.m_sKeywords = value["keywords"].asString();
		}
		
		if (true == value.isMember("cat"))
		{
			for(uiCount = 0; uiCount < value["cat"].size(); uiCount++ )
			{
				cPid.m_vCat.push_back(value["cat"][uiCount].asString());
			}
		}

		if (true == value.isMember("ban"))
		{
			for(uiCount = 0; uiCount < value["ban"].size(); uiCount++ )
			{
				cPid.m_vBanCat.push_back(value["ban"][uiCount].asString());
			}
		}
		if(iReturn != AD_FAILURE)
			cPids[cPid.m_sPid] = cPid;
	}
	else{
		return AD_FAILURE;
	}
	return iReturn;
}

int AdData::Update()
{
	AdRedisCli cRedis(m_sIp, m_iPort);
	int ret;
	map<string, AdPid>&  cPids = m_cPids.GetBack();    //update cache

	//request redis key
	string sKey;
	int iTimeStamp = 0;

	//redis keys pid
	list<string> lKeys;
	list<string>::iterator lIter;
	string sValue;

	ret = cRedis.AdRedisConnect();
	if(ret != AD_SUCCESS)
	{
		return ret;
	}

	//get timestamp
	sKey = "exec:timestamp";
	ret = cRedis.AdRedisGet(sKey,  &iTimeStamp);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Redis error\n");
		goto RETURN;
	}

	//check update or not
	if(iTimeStamp == m_iLastTimeStamp)
	{
		goto RETURN;
	}

	//update pid info
	sKey = "exec:pid";
	ret = cRedis.AdRedisHkeys(sKey, lKeys);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("redis error\n");
		goto RETURN;
	}
	
	//clear cache old data;
	cPids.clear();
	
	for(lIter = lKeys.begin(); lIter != lKeys.end(); lIter ++)
	{
		ret = cRedis.AdRedisHGet(sKey, *lIter, sValue);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("redis Error!\n");
			goto RETURN;
		}
		ret = Parse(sValue, cPids);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("Data parse error!\n");
			//goto RETURN;
		}
		sValue.clear();
	}

	lKeys.clear();
	
	m_cPids.Switch();

RETURN:
	cRedis.AdRedisClose();
	return ret;
}

int AdData::GetPid(string & sId, AdPid & cPid)
{
	map<string, AdPid>::iterator iter;
	map<string, AdPid> mPids = m_cPids.Get();
	iter = mPids.find(sId);
	if(iter != mPids.end())
	{
		cPid = iter->second;
		return AD_SUCCESS;
	}
	return AD_FAILURE;
}
