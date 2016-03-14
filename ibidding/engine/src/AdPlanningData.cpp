/***************************************
*	Description: Rtb Planning Data, get from redis redis server.
*	Author: wilson
*	Date: 2015/07/17
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <stdlib.h>
#include <string.h>
#include "AdPlanningData.h"

AdPlanningData::AdPlanningData()
{
	m_iLastTimeStamp = 0;
}
AdPlanningData::~AdPlanningData()
{
	clear(m_mData.Get());
	clear(m_mData.GetBack());
}

void AdPlanningData::AdPlanningDataInit(ADCONFIG & config)
{
	ADCONFIG_ITER iter = config.find("REDIS");
	vector<string> vStr;
	if(iter != config.end())
	{
		AdString::StrSplit(iter->second, vStr, ":");
		if(vStr.size() == 2)
		{
			m_sIp = vStr[0];
			m_iPort = atoi(vStr[1].c_str());
		}
	}
}
int AdPlanningData::clear(map<UINT,AdPlanningUnit *> &mData)
{
	map<UINT,AdPlanningUnit *>::iterator iter;
	for(iter = mData.begin(); iter != mData.end(); iter ++)
	{
		delete iter->second;
	}
	mData.clear();
	return 0;
}

map<UINT,AdPlanningUnit *> & 
AdPlanningData::DataGetCache()
{
	return m_mData.Get();
}

AdPlanningUnit * AdPlanningData::DataSearch(UINT iEid)
{
	map<UINT,AdPlanningUnit *> & mData = m_mData.Get();
	if(mData.find(iEid) != mData.end())
	{
		return mData[iEid];
	}
	return NULL;
}

int AdPlanningData::DataParse(string & sJson, AdPlanningUnit  * pUnit)
{
	UINT uiCount = 0;
	UINT uiSubCt = 0;
	Json::Reader reader;
	Json::Value value;
	Json::Value subvalue;
	Json::Value grandvalue;
	int iReturn = AD_SUCCESS;

	if (true == reader.parse((char *)sJson.c_str(), value)){
		// order
		if (true == value.isMember("order")){
			//GT_PRINT("Order Parse!\n");
			subvalue = value["order"];
			pUnit->m_stOrder.m_uiNice = AD_ORDER_BASE;
			pUnit->m_stOrder.m_uiAdvertiser = subvalue["advertiser"].asInt();
			pUnit->m_stOrder.m_uiExecuteId = subvalue["execid"].asInt();
			pUnit->m_stOrder.m_uiTargetPrice = subvalue["target_price"].asInt();
			pUnit->m_stOrder.m_uiTargetPriceType = subvalue["target_price_type"].asInt();
			if( true == subvalue.isMember("limit_price")){
				pUnit->m_stOrder.m_uiLimitePrice = subvalue["limit_price"].asInt();
			}
			pUnit->m_stOrder.m_uiBidmode = subvalue["hour_share"].asInt();
			if( true == subvalue.isMember("screen"))
			{
				for(uiCount = 0; uiCount < subvalue["screen"].size(); uiCount++)
				{
					pUnit->m_stOrder.m_ltAdViewScreen.push_back(subvalue["screen"][uiCount].asInt());
				}
			}
			if( true == subvalue.isMember("space_type"))
			{
				for(uiCount = 0; uiCount < subvalue["space_type"].size(); uiCount++)
				{
					pUnit->m_stOrder.m_ltAdViewType.push_back(subvalue["space_type"][uiCount].asInt());
				}
			}
			if( true == subvalue.isMember("ord_ctl"))
			{
				pUnit->m_stOrder.m_uiTimeType = subvalue["ord_ctl"].asInt();
			}
			if( true == subvalue.isMember("ord_freq"))
			{
				pUnit->m_stOrder.m_uiFrequency = subvalue["ord_freq"].asInt();
			}
			if( true == subvalue.isMember("advertiser_tb"))
			{
				pUnit->m_stOrder.m_sTanxAdId = subvalue["advertiser_tb"].asString();
			}
			if( true == subvalue.isMember("ctr_min")){
				pUnit->m_stOrder.m_dMinCtr = subvalue["ctr_min"].asDouble();
			}
			if( true == subvalue.isMember("ctr_type")){
				pUnit->m_stOrder.m_uiCtrType = subvalue["ctr_type"].asInt();
			}
		}
		else{
			iReturn = AD_FAILURE;
		}
		// pid
		if (true == value.isMember("pid"))
		{
			AdPidInfo  sPidInfo;
			//GT_PRINT("Pid Parse!\n");
			for(uiCount = 0; uiCount < value["pid"].size(); uiCount++ )
			{
				subvalue = value["pid"][uiCount];
				sPidInfo.m_uiAdxId = atoi(subvalue["adx_id"].asString().c_str());
				sPidInfo.m_uiMode = subvalue["select_type"].asInt();
				if( AD_PID_MODE_FIXED == sPidInfo.m_uiMode || AD_PID_MODE_OTHER == sPidInfo.m_uiMode)
				{
					for(uiSubCt = 0; uiSubCt < subvalue["spid"].size(); uiSubCt++ )
					{
						sPidInfo.m_ltPidList.push_back(subvalue["spid"][uiSubCt].asString());
					}
				}
				pUnit->m_ltPidInfo.push_back(sPidInfo);
				sPidInfo.clear();
			}
		}
		else{
			iReturn = AD_FAILURE;
		}
		// range
		if (true == value.isMember("range"))
		{
			//GT_PRINT("Range Parse!\n");
			DataParseTime(value, pUnit->m_stTime);
		}
		else{
			iReturn = AD_FAILURE;
		}

		// area
		if (true == value.isMember("area"))
		{
			//GT_PRINT("Area Parse!\n");
			for(uiCount = 0; uiCount < value["area"].size(); uiCount++)
			{
				pUnit->m_stTargetAudience.m_ltArea.push_back(value["area"][uiCount].asString());
			}
		}
		else{
			iReturn = AD_FAILURE;
		}

		// marks
		if (true == value.isMember("marks"))
		{
			if( value["marks"].size() > 0) 
				pUnit->m_stOrder.m_uiNice = AD_ORDER_AUDIENCE;
		}

		DataParseAudience(value, pUnit->m_ltAudience);
		if(!pUnit->m_ltAudience.empty())
			pUnit->m_stOrder.m_uiNice = AD_ORDER_AUDIENCE;

		DataParseMark(value, pUnit->m_ltKeyWord);
		if(!pUnit->m_ltKeyWord.empty())
			pUnit->m_stOrder.m_uiNice = AD_ORDER_NICE;
		
		// device
		if (true == value.isMember("device"))
		{
			subvalue = value["device"];
			//pUnit->m_stMobil.m_bEnable = true;
			if(true ==  subvalue.isMember("operator"))
			{
				for(uiCount = 0; uiCount < subvalue["operator"].size(); uiCount++)
				{
					pUnit->m_stMobile.m_ltOperator.push_back(subvalue["operator"][uiCount].asInt());
				}
			}
			if(true ==  subvalue.isMember("dev"))
			{
				for(uiCount = 0; uiCount < subvalue["dev"].size(); uiCount++)
				{
					pUnit->m_stMobile.m_ltDeviceType.push_back(subvalue["dev"][uiCount].asInt());
				}
			}
			if(true ==  subvalue.isMember("webcon"))
			{
				for(uiCount = 0; uiCount < subvalue["webcon"].size(); uiCount++)
				{
					pUnit->m_stMobile.m_ltConnectType.push_back(subvalue["webcon"][uiCount].asInt());
				}
			}
			
			if(true ==  subvalue.isMember("ua"))
			{
				for(uiCount = 0; uiCount < subvalue["ua"].size(); uiCount++)
				{
					pUnit->m_stMobile.m_ltUserAgentType.push_back(subvalue["ua"][uiCount].asInt());
				}
			}				

		}

		if( true == value.isMember("patform") )
		{
			if( 1 == atoi(value["patform"].asString().c_str()) )
			{
				pUnit->m_stMobile.m_bEnable = false;
			}
			if( 2 == atoi(value["patform"].asString().c_str()) )
			{
				pUnit->m_stMobile.m_bEnable = true;
			}			
		}
		else
		{
			pUnit->m_stMobile.m_bEnable = false;
		}
		
		// create
		if (true == value.isMember("create"))
		{
			DataParseCreate(value, pUnit->m_mCreativeInfo);
		}
		else
		{
			iReturn = AD_FAILURE;
		}

		// ban list
		if (true == value.isMember("ban_list"))
		{
			if( value["ban_list"].size() > 0)
			{
				for( uiCount = 0; uiCount < value["ban_list"].size(); uiCount++)
				{
					pUnit->m_stOrder.m_ltBanId.push_back(value["ban_list"][uiCount].asInt());
				}
			}
		}
		
		// timestamp
		if (true == value.isMember("timestamp")){
			pUnit->m_stOrder.m_uiTimeStamp = atoi( value["timestamp"].asString().c_str());
		}
	}
	else{
		return AD_FAILURE;
	}
	return iReturn;
}
#define AD_DAYTIME  32
int AdPlanningData::DataJsonCheck(char *stBuf)
{
	time_t ulTime;
	Json::Reader reader;
	Json::Value value;
	Json::Value subvalue;
	int iReturn = AD_SUCCESS;

	struct tm tm_time;
	char szStar[AD_DAYTIME] = {0};
	char szEnd[AD_DAYTIME] = {0};

	if (NULL != (char*)stBuf && stBuf[0] != '\0') {
		if (true == reader.parse((char *)stBuf, value)){
			/* date */
			if(true == value.isMember("range")){
				subvalue = value["range"];
				snprintf(szStar, AD_DAYTIME, "%s 00:00:00", subvalue["start"].asString().c_str());
				strptime(szStar, "%Y-%m-%d %H:%M:%S", &tm_time);
				time(&ulTime);
				if( ulTime > mktime(&tm_time)){
					if( 0 != strncmp( "\0", subvalue["end"].asString().c_str(), 1) ){
						snprintf(szEnd, AD_DAYTIME,"%s 23:59:59", subvalue["end"].asString().c_str());
						strptime(szEnd, "%Y-%m-%d %H:%M:%S", &tm_time);
						if(ulTime > mktime(&tm_time) ){
							/* out of date */
							if(true == value.isMember("order")){
								#ifdef _GT_DEBUG_
									GT_WARN("ExecID:%5d is Out Of Date Order!\n", value["order"]["execid"].asInt());
								#endif
							}
							iReturn = AD_FAILURE;
						}
					}
					else{
						/* no end date */
					}
				}
				else{
					/* future order */
					#ifdef _GT_DEBUG_
						GT_WARN("ExecID:%5d is Future Order!\n" , value["order"]["execid"].asInt());
					#endif
					iReturn = AD_FAILURE;
				}
			}
		}
		else{
			AD_ERROR("CGtRtbAdvs::GtRtbBaseInput Json Reader Parser Error!\n");
		}
	}
	return iReturn;
}

void AdPlanningData::DataParseTime(Json::Value &value, AdTime & time)
{
	UINT uiNcout = 0;
	UINT uiCount = 0;
	Json::Value subvalue;
	Json::Value grandvalue;

	subvalue = value["range"];
	time.bPush = false;
	time.m_sStart = subvalue["start"].asString();
	time.m_sStop = subvalue["end"].asString();

	for(uiCount = 0; uiCount < subvalue["week"].size(); uiCount++)
	{
		grandvalue = subvalue["week"][uiCount];
		time.m_stDay[uiCount].m_uiWeek = grandvalue["weekday"].asInt();
		time.m_stDay[uiCount].m_uiBudget  = grandvalue["budget"].asInt();
//		if( time.m_stDay[uiCount].m_uiWeek == (UINT)DataGetWeekDay() )
		{
				time.bPush = true;
		}
		for(uiNcout = 0; uiNcout < grandvalue["hour"].size(); uiNcout++)
		{
			//GT_PRINT("%d\n",grandvalue["hour"][uiNcout].asInt());
			time.m_stDay[uiCount].m_uiHour |= 1<<grandvalue["hour"][uiNcout].asInt();
		}

	}
	return;
}

void AdPlanningData:: DataParseMark(Json::Value &value,  list<AdMarkNode>& ltMark)
{
	UINT uiCount;
	AdMarkNode ctNode;
	
	if (true == value.isMember("mark"))
	{
		ctNode.m_uiType = AD_MARK_PUBLIC;
		for(uiCount = 0; uiCount < value["mark"].size(); uiCount++)
		{
			ctNode.m_sKey = value["mark"][uiCount].asString();
			ltMark.push_back(ctNode);
		}
	}
	if ( true == value.isMember("acc_cus_keyword"))
	{
		ctNode.m_uiType = AD_KEYS_ACC_CUS;
		for(uiCount = 0; uiCount < value["acc_cus_keyword"].size(); uiCount++)
		{
			ctNode.m_sKey = value["acc_cus_keyword"][uiCount].asString();
			ltMark.push_back(ctNode);
		}
	}
	if ( true == value.isMember("dim_cus_keyword"))
	{
		ctNode.m_uiType = AD_KEYS_DIM_CUS;
		for(uiCount = 0; uiCount < value["dim_cus_keyword"].size(); uiCount++)
		{
			ctNode.m_sKey = value["dim_cus_keyword"][uiCount].asString();
			ltMark.push_back(ctNode);
		}
	}

	if ( true == value.isMember("private_mark"))
	{
		ctNode.m_uiType = AD_MARK_PRIVATE;
		for(uiCount = 0; uiCount < value["private_mark"].size(); uiCount++)
		{
			ctNode.m_sKey = value["private_mark"][uiCount].asString();
			ltMark.push_back(ctNode);
		}
	}

}

void AdPlanningData::DataParseAudience(Json::Value &value, list<AdAudienceNode> &ltAudi)
{
	UINT uiCMark;
	UINT uiCount;
	UINT uiTmpCt;

	Json::Value subvalue;
	Json::Value grandvalue;

	AdAudienceNode ctAudi;
	
	if (true == value.isMember("marks"))
	{
		for(uiCMark = 0; uiCMark < value["marks"].size(); uiCMark++)
		{
			subvalue = value["marks"][uiCMark];
			if (true == subvalue.isMember("mark_id"))
			{
				ctAudi.m_sAudienceID = subvalue["mark_id"].asString();
			}
			if (true == subvalue.isMember("merge_type"))
			{
				ctAudi.m_uiType = subvalue["merge_type"].asInt();
			}
			if (true != subvalue.isMember("tag_datas"))
			{
				continue;
			}
				//AD_PRINT("size:%d\n",subvalue["tag_datas"].size());
			for(uiCount = 0; uiCount < subvalue["tag_datas"].size(); uiCount++)
			{
				grandvalue = subvalue["tag_datas"][uiCount];
				if( true != grandvalue.isMember("datas") )
				{
					continue;
				}

				UINT uiTime = grandvalue["date_limit"].asInt();
				if(uiTime == 0)
					uiTime = 30;
				for( uiTmpCt = 0; uiTmpCt < grandvalue["datas"].size(); uiTmpCt++)
				{
					switch(grandvalue["visit_type"].asInt())
					{
						case AD_AUDIENCE_UNVISIT:
							ctAudi.m_mUnVisit[grandvalue["datas"][uiTmpCt].asString()]  = uiTime;
							break;
						case AD_AUDIENCE_VISIT:
							ctAudi.m_mVisit[grandvalue["datas"][uiTmpCt].asString()]  = uiTime;
							break;
						case AD_AUDIENCE_SEARCH:
							ctAudi.m_mSearch[grandvalue["datas"][uiTmpCt].asString()]  = uiTime;
							break;
						default:
							break;
					}
				}
			}
			ltAudi.push_back(ctAudi);
			ctAudi.clear();
		}
	}
}


void AdPlanningData::DataParseCreate(Json::Value &value, map<UINT, AdCreativeInfo>& pMap)
{
	UINT uiCCout = 0;
	UINT uiCount = 0;
	UINT uiSCout = 0;
	UINT uiAdx = 0;
	Json::Value subvalue;
	Json::Value grandvalue;
	AdMaterialInfo sMaterial;
	char buf[AD_BYTE64];

	/* create */
	for( uiCCout = 0; uiCCout < value["create"].size(); uiCCout++)
	{
		subvalue = value["create"][uiCCout];
		uiAdx = atoi(subvalue["adx_id"].asString().c_str());

		AdCreativeInfo& sCreate = pMap[uiAdx];

		sCreate.m_sCreativeGroupId = subvalue["gid"].asString();
		sCreate.m_sAdxId = subvalue["adx_id"].asString();
//		sCreate.m_sSensitiveCategory = subvalue["sensitive"].asString();
//		sCreate.m_sIndustryCategory = subvalue["ad_type"].asString();

		/* material */
		for(uiCount = 0; uiCount < subvalue["material"].size(); uiCount++)
		{
			grandvalue = subvalue["material"][uiCount];

			if( true == grandvalue.isMember("cid"))	
			{
				snprintf(buf, AD_BYTE64, "%d",grandvalue["cid"].asInt());
				sMaterial.m_sCreateId = buf;
			}
			sMaterial.m_sMaterialId = grandvalue["mid"].asString();
			sMaterial.m_sUuId = grandvalue["uuid"].asString();
			sMaterial.m_uiFormat = grandvalue["format"].asInt();
			sMaterial.m_uiCreativeType = grandvalue["creative_type"].asInt();
			sMaterial.m_uiWidth = grandvalue["width"].asInt();
			sMaterial.m_uiHeight = grandvalue["height"].asInt();

			if( true == grandvalue.isMember("duration"))
			{
				sMaterial.m_uiVideoTime = grandvalue["duration"].asInt();
			}
			if( true == grandvalue.isMember("click_through_url"))
			{
				sMaterial.m_sClickUrl = grandvalue["click_through_url"].asString();
			}
			if( true == grandvalue.isMember("destination_url"))
			{
				sMaterial.m_sDestinationUrl = grandvalue["destination_url"].asString();
			}
			if( true == grandvalue.isMember("material_url"))
			{
				sMaterial.m_sMaterialUrl = grandvalue["material_url"].asString();
			}
			if( true == grandvalue.isMember("material_url_mraid"))
			{
				sMaterial.m_sMraidMaterialUrl = grandvalue["material_url_mraid"].asString();
			}
			if( true == grandvalue.isMember("ad_type"))
			{
				sMaterial.m_sCategory = grandvalue["ad_type"].asString();
			}
			if( true == grandvalue.isMember("monitor_url"))
			{
				for(uiSCout = 0; uiSCout < grandvalue["monitor_url"].size(); uiSCout++)
				{
					sMaterial.m_ltMonitorUrl.push_back(grandvalue["monitor_url"][uiSCout].asString());
				}
			}
			sCreate.m_ltMaterials.push_back(sMaterial);
			sMaterial.clear();
		}
	}
	return ;
}

int AdPlanningData::DataPush(string & sJson,  map<UINT,AdPlanningUnit *> & mMap)
{
	int iReturn = DataJsonCheck((char *)sJson.c_str());
	if(iReturn != AD_SUCCESS)
		return AD_SUCCESS;
	AdPlanningUnit  * pUnit  = new AdPlanningUnit;
	if(pUnit == NULL)
	{
		AD_ERROR("mem not enough\n");
		return AD_FAILURE;
	}

	if(iReturn == AD_SUCCESS)
	{
		if (AD_SUCCESS == DataParse(sJson, pUnit))
		{
			mMap[pUnit->m_stOrder.m_uiExecuteId] = pUnit;
		}
		else{
			AD_ERROR("Parse Error!\n");
			iReturn = AD_FAILURE;
			delete pUnit;
		}
	}
	else
	{
		delete pUnit;
	}
	return iReturn;
}


int AdPlanningData::DataUpdateBanList()
{
	string sValue;
	string sAdx;
	string sPid;
	UINT uiCount;
	UINT uiTmp;
	int ret = AD_SUCCESS;	
	char key[AD_BYTE128];

	Json::Reader reader;
	Json::Value value;
	Json::Value subvalue;

	string sHash = "adv:ban";
	map<UINT,AdPlanningUnit *> & mMap =  m_mData.GetBack();
	map<UINT,AdPlanningUnit *>::iterator   mIter;
	
	list<UINT>::iterator lIter;

	for(mIter = mMap.begin();  mIter != mMap.end(); mIter ++)
	{
		for(lIter = (mIter->second)->m_stOrder.m_ltBanId.begin(); \
			lIter != (mIter->second)->m_stOrder.m_ltBanId.end(); lIter ++)
		{
			snprintf(key, AD_BYTE128, "%d", *lIter);

			ret = AdRedisHGet((char *)"adv:ban", key, sValue);
			if(ret != AD_SUCCESS)
			{
				AD_ERROR("redis Error!\n");
				goto RETURN;
			}

			if(sValue.empty())
			{
//				AD_ERROR("Redis Get Nil  %s\n", key);
				continue;
			}

			if (true == reader.parse(sValue.c_str(), value))
			{
				if ( value.size() > 0 )
				{
					for(uiCount = 0; uiCount < value.size(); uiCount++)
					{
						subvalue = value[uiCount];
						sAdx.clear();
						if( true == subvalue.isMember("adx_id"))
						{
							sAdx = subvalue["adx_id"].asString();
						}
						else
							continue;
						if( true == subvalue.isMember("spid"))
						{
							for(uiTmp = 0; uiTmp < subvalue["spid"].size(); uiTmp++)
							{
								sPid.clear();
								sPid = subvalue["spid"][uiTmp].asString();
								snprintf(key, AD_BYTE128, "%s_%s", sAdx.c_str(), sPid.c_str());
								(mIter->second)->m_stOrder.m_ltBanContent.push_back(key);
							}
						}					
					}
				}
			}
			else
			{
				ret  = AD_FAILURE;
				goto RETURN;
			}
		}
	}
RETURN:
	return ret;
}

int AdPlanningData::DataUpdateForSystem()
{
	map<UINT,AdPlanningUnit *>::iterator   mIter;
	map<UINT,AdPlanningUnit *> & mMap =  m_mData.GetBack();
	int ret;
	char str[AD_BYTE128] = {0}; 
	string sKey;
	string sFiled;

	if(mMap.empty())
		return AD_FAILURE;
	
	ret = AdRedisConnect();
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("redis error\n");
		 goto RETURN;
	}

	for(mIter = mMap.begin();  mIter != mMap.end(); mIter ++)
	{
		sKey = "exec:today";
		snprintf(str, AD_BYTE128, "%d+%d", (mIter->second)->m_stOrder.m_uiExecuteId, (mIter->second)->m_stOrder.m_uiAdvertiser);
		sFiled = str;
		ret = AdRedisSadd(sKey, sFiled);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("redis error\n");
		 	goto RETURN;
		}
	}

RETURN:
	AdRedisClose();
	 return ret;
}


int AdPlanningData::DataUpdate(UINT * uiData)
{
	list<string> lKeys;
	list<string>::iterator iIter;
	string sValue;
	string sKey;
	int ret;
	int iTimeStamp = 0;

	map<UINT,AdPlanningUnit *> & mMap = m_mData.GetBack();

	ret = AdRedisConnect();
	if(ret != AD_SUCCESS)
	{
		return ret;
	}

	sKey = "exec:timestamp";
	ret = AdRedisGet(sKey,  &iTimeStamp);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Redis error\n");
		goto RETURN;
	}

	if(iTimeStamp == m_iLastTimeStamp)
	{
		goto RETURN;
	}

	//update Filter info
	sKey = "exec:raw";
	ret = AdRedisHkeys(sKey, lKeys);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("redis error\n");
		goto RETURN;
	}
	//clear cache old data;
	clear(mMap);
	
	for(iIter = lKeys.begin(); iIter != lKeys.end(); iIter ++)
	{
		ret = AdRedisHGet(sKey, *iIter, sValue);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("redis Error!\n");
			goto RETURN;
		}
		ret = DataPush(sValue, mMap);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("Data push error!\n");
			goto RETURN;
		}
	}

	lKeys.clear();
	
	ret = DataUpdateBanList();
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Data update Ban content error!\n");
		goto RETURN;
	}

RETURN:
	AdRedisClose();
	if((ret == AD_SUCCESS) && (iTimeStamp != m_iLastTimeStamp))
	{
		ret = DataUpdateForSystem();
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("Data update for Systerm error!\n");
		}
		AD_INFO("    Engine: Order Data Updated  %d\n", static_cast<int>(mMap.size()));
		m_iLastTimeStamp = iTimeStamp;
		m_mData.Switch();
		*uiData = mMap.size();
	}
	else
	{
		*uiData = 0;
		//AD_ERROR("Data Update Fail  no switch\n");
	}
	return ret;
}

int AdPlanningData::DataUpdateAdvancedFilterResult()
{
	char str[AD_BYTE128] = {0}; 
	string sKey;
	int ret;

	map<UINT,AdPlanningUnit *> & mMap = m_mData.Get();
	map<UINT,AdPlanningUnit *>::iterator   mIter;

	if(!m_mBanPidList.empty())
		m_mBanPidList.clear();

	if(mMap.empty())
		return AD_FAILURE;
	
	ret = AdRedisConnect();
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("redis error\n");
		 goto RETURN;
	}

	for(mIter = mMap.begin();  mIter != mMap.end(); mIter ++)
	{
		snprintf(str, AD_BYTE128, "exec:senior:filter:pid:%d", (mIter->second)->m_stOrder.m_uiExecuteId);
		sKey = str;
		list<string> & pidlist = m_mBanPidList[(mIter->second)->m_stOrder.m_uiExecuteId];
		ret = AdRedisHkeys(sKey, pidlist);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("redis error\n");
		 	goto RETURN;
		}
		(mIter->second)->m_pltBanPidList = &pidlist;
	}

RETURN:
	AdRedisClose();
	 return ret;
}


/*******************************Advanced Price Info***************************/
int AdPlanningData::DataParseAdvancedPriceData(string & sData, UINT uiEid, 
	map<UINT, map<string,  AdAdvancedPrice> > & mAdvanced )
{
	UINT iCount = 0;
	int iReturn = AD_SUCCESS;
	
	Json::Reader reader;
	Json::Value value;
	Json::Value subvalue;
	AdAdvancedPrice  sPrice;

	if (true == reader.parse(sData.c_str(), value))
	{
		if( value.size() > 0)
		{
			for( iCount = 0; iCount < value.size(); iCount++)
			{
				subvalue = value[iCount];
				
				sPrice.m_iExecId= uiEid;
				sPrice.m_iAdxId = atoi(subvalue["adx_id"].asString().c_str());
				sPrice.m_iType = atoi(subvalue["type"].asString().c_str());
				sPrice.m_iEpv= subvalue["epv"].asInt() * 1000;
				sPrice.m_iEprice= subvalue["eprice"].asInt();
				sPrice.m_sPID = subvalue["spid"].asString();
				mAdvanced[uiEid][sPrice.m_sPID] = sPrice;
				sPrice.clear();
			}
		}
	}
	else
	{
		iReturn = AD_FAILURE;
	}

	return iReturn;
}

int AdPlanningData::DataUpdateAdvancedPriceData()
{
	string sKey;
	int ret;
	UINT eId;
	list<string> lKeys;
	list<string>::iterator lIter;
	string sData;
	
	map<UINT,AdPlanningUnit *> & mData = m_mData.Get();
	map<UINT,AdPlanningUnit *>::iterator   dataIter;

	map<UINT, map<string,  AdAdvancedPrice> > & mAdvanced = m_mAdvancePrice.GetBack();
	map<UINT, map<string,  AdAdvancedPrice> >::iterator   advancedIter;

	
	if(!mAdvanced.empty())
		mAdvanced.clear();
	
	ret = AdRedisConnect();
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("redis error\n");
		 goto RETURN;
	}
	sKey = "exec:senior";
	ret = AdRedisHkeys(sKey, lKeys);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("redis error\n");
		 goto RETURN;
	}
	for(lIter = lKeys.begin(); lIter != lKeys.end(); lIter ++)
	{
		sData.clear();
		eId = atoi((*lIter).c_str());
		ret = AdRedisHGet(sKey, *lIter,  sData);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("redis error\n");
		 	goto RETURN;
		}
		DataParseAdvancedPriceData(sData, eId, mAdvanced);
	}

	for(dataIter = mData.begin();  dataIter != mData.end(); dataIter ++)
	{
		advancedIter = mAdvanced.find((dataIter->second)->m_stOrder.m_uiExecuteId);
		if(advancedIter != mAdvanced.end())
		{
			(dataIter->second)->m_pAdvancedPrice = &(advancedIter->second);
		}
		else
		{
			(dataIter->second)->m_pAdvancedPrice = NULL;
		}
	}
	
RETURN:
	AdRedisClose();
	m_mAdvancePrice.Switch();
	return ret;
}
