/***************************************
*	Description: Rtb Planning Data, head file.
*	Author: wilson
*	Date: 2015/07/17
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#ifndef	_AD_PLANNING_DATA_H
#define	_AD_PLANNING_DATA_H

#include <map>
#include <string>
#include <list>
#include <iostream>
#include <string.h>
#include "json/json.h"
#include "AdRedisCli.h"
#include "AdGlobal.h"
#include "AdBuff.h"
#include "AdRtbConfig.h"

/* ORDER_INFO */
#define	AD_ORDER_CPM				0x01	// cpm order
#define	AD_ORDER_CPC				0x02	// cpc order

#define	AD_ORDER_NICE				0x01	// nice order
#define	AD_ORDER_BASE				0x02	// base order
#define	AD_ORDER_AUDIENCE			0x03	// audice order

#define AD_MARK_PUBLIC			0x01	// public mark flag
#define AD_MARK_PRIVATE		0x02	// private mark flag
#define AD_KEYS_ACC_CUS		0x03	// acc_cus flag
#define AD_KEYS_DIM_CUS		0x04	// dim_cus flag

#define	AD_PID_MODE_ALL				0x01
#define	AD_PID_MODE_FIXED			0x02
#define	AD_PID_MODE_OTHER			0x03

#define  AD_TIME_TYPE_NO                             0
#define  AD_TIME_TYPE_HOUR                       0x01
#define  AD_TIME_TYPE_DAY                           0x02

class AdOrderInfo
{
public:
	AdOrderInfo()
	{
		clear();
	};
	~AdOrderInfo()
	{
		clear();
	};
	void clear()
	{
		m_uiAdvertiser = 0;				// advertiser
		m_uiExecuteId = 0;				// execute_id
		m_uiTotalBudget = 0;				// total_budget
		m_uiTargetPriceType = 0;				// target_price_type
		m_uiTargetPrice = 0;				// target_price
		m_uiLimitePrice = 0;				// limit_price
		m_uiBidmode = 0;				// hour_share, average or fast 
		m_uiTimeType = AD_TIME_TYPE_NO;				// ord_ctl
		m_uiFrequency = 0;				// ord_freq
		m_uiTimeStamp = 0;				// time stamp
		m_uiNice = 0;				// nice order GT_RA_NCE/GT_RA_NOM
		m_ltAdViewScreen.clear();        //screen 
		m_ltAdViewType.clear();      //space_type
		m_sTanxAdId.clear();      // tanx ad id
		m_ltBanId.clear();;      // ban list
		m_ltBanContent.clear();;      // ban list
		m_uiCtrType = 0;                                    //used ctr model type
        m_dMinCtr   = 0.0;                                  // min  ctr
	};
	UINT m_uiAdvertiser;				// advertiser
	UINT m_uiExecuteId;				// execute_id
	UINT m_uiTotalBudget;				// total_budget
	UINT m_uiTargetPriceType;				// target_price_type
	UINT m_uiTargetPrice;				// target_price
	UINT m_uiLimitePrice;				// limit_price
	UINT m_uiBidmode;				// hour_share, average or fast 
	UINT m_uiTimeType;				// ord_ctl
	UINT m_uiFrequency;				// ord_freq
	UINT m_uiTimeStamp;				// time stamp
	UINT m_uiNice;				// nice order GT_RA_NCE/GT_RA_NOM
	list<UINT> m_ltAdViewScreen;        //screen 
	list<UINT> m_ltAdViewType;      //space_type
	string m_sTanxAdId;      // tanx ad id
	list<UINT> m_ltBanId;      // ban list
	list<string> m_ltBanContent;
    UINT m_uiCtrType;                                       //used ctr model type
    double m_dMinCtr;                                       // min  ctr
};


/* MOBILE_INFO */
class AdMobileInfo
{
public:
	AdMobileInfo()
	{
		clear();
	};
	~AdMobileInfo()
	{
		clear();
	};
	void clear()
	{
		m_bEnable = false;				// mobile
		m_ltOperator.clear();      //mobile operrate
		m_ltDeviceType.clear();     //mobile type
		m_ltConnectType.clear();     //mobile connnect type,  wifi and so on.
		m_ltUserAgentType.clear();
	};

	bool m_bEnable;				// mobile
	list<UINT> m_ltOperator;      //mobile operrate
	list<UINT> m_ltDeviceType;     //mobile type
	list<UINT> m_ltConnectType;     //mobile connnect type,  wifi and so on.
	list<UINT> m_ltUserAgentType;     // user agent IE/Chrome/ and so on,
};


class AdPidInfo
{
public:
	AdPidInfo()
	{
		clear();
	};
	~AdPidInfo()
	{
		clear();
	};
	void clear()
	{
		m_uiAdxId = 0;				// adx id
		m_uiMode = 0;				// select_type
		m_ltPidList.clear();;    //pid list
	};

	UINT m_uiAdxId;				// adx id
	UINT m_uiMode;				// select_type
	list<string> m_ltPidList;    //pid list
};


class AdTime
{
public:
	AdTime()
	{
		clear();
	};
	~AdTime()
	{
		clear();
	};
	void clear()
	{
		bPush = false;
		m_sStart.clear();
		m_sStop.clear();
		memset(m_stDay, 0, sizeof(AdTimeDay) * AD_WEEK_DAYS);
	};

	bool bPush;
	string m_sStart;
	string m_sStop;
	struct AdTimeDay{				
		UINT m_uiWeek;		// week
		UINT m_uiBudget;			// val
		UINT m_uiHour;		// time
	};
	AdTimeDay m_stDay[AD_WEEK_DAYS];
};

/* AUDIENCE */
#define GT_RA_MARK				5000
class AdTargetAudience
{
public:
	AdTargetAudience(){};
	~AdTargetAudience()
	{
		clear();
	};
	void clear()
	{
		m_ltArea.clear();
	}
	list<string> m_ltArea;
};

/* AD_MATERIAL */
class AdMaterialInfo
{				
public:
	AdMaterialInfo()
	{
		clear();
	};
	~AdMaterialInfo()
	{
		clear();
	};
	void clear()
	{
		m_uiFormat = 0;
		m_uiVideoTime = 0;
		m_uiCreativeType = 0;							// creative_type
		m_uiWidth = 0;							// width
		m_uiHeight = 0;							// height
		
		m_sCategory.clear(); 	
		m_sUuId.clear();	
		m_sCreateId.clear();	
		m_sMaterialId.clear();
		m_sMaterialUrl.clear();  	
		m_sMraidMaterialUrl.clear();
		m_sClickUrl.clear();			
		m_sDestinationUrl.clear();

		m_ltMonitorUrl.clear();
	};

	UINT m_uiFormat;							// mateformat(1:png, 2:jpg, 3:gif, 4:flv, 5:flash, 6:swf 7:hmtl 8:MP4 )
	UINT m_uiVideoTime;							// duration
	UINT m_uiCreativeType;							// creative_type
	UINT m_uiWidth;							// width
	UINT m_uiHeight;							// height

	string m_sCategory; 		// ad type
	string m_sUuId;					// uuid
	string m_sCreateId;			// create id
	string m_sMaterialId;	    // material_id
	string m_sMaterialUrl;  	// material_url
	string m_sMraidMaterialUrl;	// tanx mraid material_url
	string m_sClickUrl;				// click_through_url
	string m_sDestinationUrl;			// click_through_url
	list<string> m_ltMonitorUrl;
};


/* ADVERTISEMENTS */
#define	GT_RA_CATE				32
#define GT_RA_MATE				64
class AdCreativeInfo
{
public:
	AdCreativeInfo(){}; 
	~AdCreativeInfo()
	{
		clear();
	}; 
	void clear()
	{
		m_ltMaterials.clear(); 		
		m_sCreativeGroupId.clear();	
		m_sAdxId.clear();		
		m_sSensitiveCategory.clear();
		m_sCategory.clear();

	};
		// create
	string m_sCreativeGroupId;		// group id
	string m_sAdxId;			// adx id
	string m_sSensitiveCategory;  // sensitive_category
	string m_sCategory; 		// ad type
	list<AdMaterialInfo> m_ltMaterials;	// material 		
};

//Advanced Price info
class AdAdvancedPrice
{
public:
	AdAdvancedPrice()
	{
		clear();
	};
	void clear()
	{
		m_sPID.clear();
		m_iType = 0;
		m_iAdxId = 0;
		m_iEpv = 0;
		m_iEprice = 0;
		m_iExecId = 0;
	};
	string m_sPID;
	int m_iType;
	int m_iAdxId;
	int m_iEpv;
	int m_iEprice;
	int m_iExecId;
};


#define AD_AUDIENCE_UNVISIT 							0	                
#define AD_AUDIENCE_VISIT                 						1
#define AD_AUDIENCE_SEARCH            						2
#define AD_AUDIENCE_ELECTRONIC_COMMERCE		3

#define AD_AUDIENCE_MERGE_AND                                             1
#define AD_AUDIENCE_MERGE_OR                                                0


class AdAudienceNode
{
public:
	AdAudienceNode()
	{
		clear();
	};
	void clear()
	{
		m_uiType = 0;
		m_sAudienceID.clear();
		m_mSearch.clear();
		m_mUnVisit.clear();
		m_mVisit.clear();
	};

	UINT m_uiType;
	
	string m_sAudienceID;				// audience id
	map<string, UINT>  m_mSearch;	           // search marks
	map<string, UINT>  m_mVisit;	                  // visit marks
	map<string, UINT>  m_mUnVisit;	           // un visit marks
};

class AdMarkNode
{
public:
	AdMarkNode()
	{
		clear();
	};
	~AdMarkNode()
	{
		clear();
	};
	void clear()
	{
		m_uiType = 0;
		m_sKey.clear();
	}
	UINT  m_uiType;
	string m_sKey;
};

/* EXECUTE_ORDERS */
class AdPlanningUnit
{	
public:
	AdPlanningUnit()
	{
		clear();
	};
	~AdPlanningUnit()
	{
		clear();
	};
	void clear()
	{
		m_stOrder.clear();
		m_stMobile.clear();
		m_stTime.clear();
		m_stTargetAudience.clear();
		m_mCreativeInfo.clear();
		m_ltPidInfo.clear();
		m_ltKeyWord.clear();
		m_ltAudience.clear();
		m_pltBanPidList = NULL;
		m_pAdvancedPrice = NULL;
	};
	AdOrderInfo m_stOrder;
	AdMobileInfo m_stMobile;
	AdTime m_stTime;
	AdTargetAudience m_stTargetAudience;
	map<UINT, AdCreativeInfo> m_mCreativeInfo;
	list<AdPidInfo> m_ltPidInfo;
	list<AdMarkNode>  m_ltKeyWord;
	list<AdAudienceNode>  m_ltAudience;
	list<string> *m_pltBanPidList;
	map<string,  AdAdvancedPrice> * m_pAdvancedPrice;
};


class AdPlanningData:public AdRedisCli
{
public:
	AdPlanningData();
	virtual ~AdPlanningData();
	void AdPlanningDataInit(ADCONFIG & config);
	AdPlanningUnit * DataSearch(UINT iEid);
	int DataUpdate(UINT * uiData);
	int DataUpdateAdvancedFilterResult();
	int DataUpdateAdvancedPriceData();
	map<UINT,AdPlanningUnit *> & DataGetCache();

protected:
	int clear(map<UINT,AdPlanningUnit *> &mData);
	int DataPush(string & sJson,  map<UINT,AdPlanningUnit *> & mMap);

	int DataParseAdvancedPriceData(string & sData, UINT uiEid, map<UINT, map<string,  AdAdvancedPrice> > & mAdvanced );

private:
	int DataParse(string & sJson, AdPlanningUnit * pUnit);
	int DataJsonCheck(char *stBuf);
	void DataParseTime(Json::Value &value, AdTime & time);
	void DataParseCreate(Json::Value &value, map<UINT, AdCreativeInfo>& pMap);
	void DataParseMark(Json::Value &value,  list<AdMarkNode>& ltMark);
	void DataParseAudience(Json::Value &value, list<AdAudienceNode>& ltAudi);
	int DataUpdateBanList();
	int DataUpdateForSystem();

	AdPingPongBuffer<map<UINT,AdPlanningUnit *> > m_mData;
	
	int m_iLastTimeStamp;
	
	//advanced filter result,  prohibit pid list
	map<UINT, list<string> > m_mBanPidList;

	//advanced price info
	AdPingPongBuffer<map<UINT, map<string,  AdAdvancedPrice> > > m_mAdvancePrice;
};

#endif /* _GTRTBADVS_H */
