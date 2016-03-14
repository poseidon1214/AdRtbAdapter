/***************************************
*	Description: define Rtb Struct,  include rtb request and response
*	Author: wilson
*	Date: 2015/12/11
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_RTB_BASE_H
#define	_AD_RTB_BASE_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>

#include "AdGlobal.h"
#include "AdTime.h"

using namespace std;

//info type
#define AD_DEBUG_PRINT_REALTIME                     1
#define AD_DEBUG_PRINT_PROCESS                      2
#define AD_DEBUG_PRINT_INFO                         3

//print Process flags
#define AD_ADAPTER_PRINT_INPUT					    0x00000001
#define AD_ADAPTER_PRINT_REQUEST               			    0x00000002
#define AD_ENGINE_PRINT_REQUEST                                     0x00000002
#define AD_ENGINE_PRINT_FILTER                                      0x00000004
#define AD_ENGINE_PRINT_RESULT                                      0x00000008
#define AD_ENGINE_PRINT_RANK                                        0x00000010
#define AD_ENGINE_PRINT_RESPONSE                                    0x00000020
#define AD_ADAPTER_PRINT_RESPONSE                                   0x00000020
#define AD_ADAPTER_PRINT_OUTPUT                                     0x00000040


/* adx */
#define	AD_PROTOCOL_KEY_ADX_TANX						1			// tanx
#define AD_PROTOCOL_KEY_ADX_MENLO						2			// menlo
#define	AD_PROTOCOL_KEY_ADX_ALLYES						3			// allyes
#define AD_PROTOCOL_KEY_ADX_SINA						4			// sina
#define	AD_PROTOCOL_KEY_ADX_YUKU						5			// youku
#define AD_PROTOCOL_KEY_ADX_BAIDU						6			// baidu
#define	AD_PROTOCOL_KEY_ADX_TENCENT						7			// tencent			
#define AD_PROTOCOL_KEY_ADX_SOHU						8			// sohu
#define	AD_PROTOCOL_KEY_ADX_MOGO						9			// mogo
#define AD_PROTOCOL_KEY_ADX_INMOBI						10              	// inmobi
#define AD_PROTOCOL_KEY_ADX_JUXIAO                 		11              	// juxiao
#define AD_PROTOCOL_KEY_ADX_MIAOZHEN              		12              	// miaozhen
#define AD_PROTOCOL_KEY_ADX_GUANGDIANTONG           		13      // guang dian tong
#define AD_PROTOCOL_KEY_ADX_GUANGGAOJIA          			14      // guang gao jia    
//#define AD_PROTOCOL_KEY_ADX_MANGGUO					15		// mango tv (no use) 
#define AD_PROTOCOL_KEY_ADX_GOOGLE					15		// google 
#define AD_PROTOCOL_KEY_ADX_WEIBO					16 // weibo

#define AD_PROTOCOL_MOBILE_DEV_IPHONE				"1"
#define AD_PROTOCOL_MOBILE_DEV_ANDROID				"2"
#define AD_PROTOCOL_MOBILE_DEV_IPAD					"3"
#define AD_PROTOCOL_MOBILE_DEV_WPHONE				"4"                   //windows phone
#define AD_PROTOCOL_MOBILE_DEV_APAD					"5"                  //android pad
#define AD_PROTOCOL_MOBILE_DEV_TV                                         "6"

#define AD_PROTOCOL_MOBILE_OPERATE_YIDONG			"46000"
#define AD_PROTOCOL_MOBILE_OPERATE_LIANTONG		"46001"
#define AD_PROTOCOL_MOBILE_OPERATE_DIANXIN		"46003"
#define AD_PROTOCOL_MOBILE_OPERATE_TIETONG		"46020"

#define AD_PROTOCOL_MOBILE_CON_WIFI                                   "1"
#define AD_PROTOCOL_MOBILE_CON_NOWIFI                            "2"
#define AD_PROTOCOL_MOBILE_CON_2G						"3"
#define AD_PROTOCOL_MOBILE_CON_3G						"4"
#define AD_PROTOCOL_MOBILE_CON_4G						"5"
#define AD_PROTOCOL_MOBILE_CON_OTHER                               "6"

#define AD_PROTOCOL_VIEW_TYPE_NO						"0"
#define AD_PROTOCOL_VIEW_TYPE_FIXED				        "1"
#define AD_PROTOCOL_VIEW_TYPE_POPUP				        "2"
#define AD_PROTOCOL_VIEW_TYPE_POP          			        "3"                      //背投
#define AD_PROTOCOL_VIEW_TYPE_VIDEOPOUSE			"4"
#define AD_PROTOCOL_VIEW_TYPE_VIDEOPATCH			"5"                     //视频贴片


#define AD_PROTOCOL_VIEW_TYPE_WIRELESS_OPENSCREEN		"11"     //无线开屏
#define AD_PROTOCOL_VIEW_TYPE_WIRELESS_POPUP				"12"     //无线弹窗
#define AD_PROTOCOL_VIEW_TYPE_WIRELESS_BANNER			"13"     //无线横幅
#define AD_PROTOCOL_VIEW_TYPE_WIRELESS_VIDEO				"14"     //无线视频贴片 无线视频暂停
#define AD_PROTOCOL_VIEW_TYPE_WIRELESS_NATIVE			"16"     //无线native


#define AD_PROTOCOL_VIEW_TYPE_OTHRE				        "9"


//1:png, 2:jpg, 3:gif, 4:flv, 5:flash, 6:swf 7:hmtl 8:MP4 
#define AD_PROTOCOL_MAT_FORMAT_PNG			1 // png
#define AD_PROTOCOL_MAT_FORMAT_JPG			2 // jpg
#define AD_PROTOCOL_MAT_FORMAT_GIF			3 // gif
#define AD_PROTOCOL_MAT_FORMAT_FLV			4 // flv
#define AD_PROTOCOL_MAT_FORMAT_SWF			5 // swf
#define AD_PROTOCOL_MAT_FORMAT_HMTL		7 // html
#define AD_PROTOCOL_MAT_FORMAT_MP4			8 // mp4

#define AD_RESPONSE_MAX_TIME                                      15000
#define  AD_DEFAULT_NUM    32

#define AD_PROTOCOL_FLOW_PC                             0
#define AD_PROTOCOL_FLOW_VIDEO                          1
#define AD_PROTOCOL_FLOW_MOBILE                         2
/* flow */

class AdRtbRequest
{
public:
	AdRtbRequest()
	{
		m_sBidID.reserve(AD_BYTE256);
		m_sUserID.reserve(AD_BYTE128);
		m_sUserIP.reserve(AD_BYTE32);
		m_sArea.reserve(AD_BYTE32);
		m_sPId.reserve(AD_BYTE256);
		m_sSize.reserve(AD_BYTE32);
		m_sUserAgent.reserve(AD_KBYTES8);
		m_sUserUrl.reserve(AD_KBYTES8);
		m_sDevUniqueType.reserve(AD_BYTE32);
		clear();
	};
	virtual ~AdRtbRequest() 
	{
		clear();
	};
	void clear()
	{
		m_uiAdx = 0;											// league
		m_sFlow.clear();											// flow (0:pc, 1:video, 2:mobile)
		m_sAdViewScreen.clear();									// adspace view screen 1: first screen 2: no first screen 
		m_sAdViewType.clear();									// adspace view type 1: 
		m_sBidID.clear();											// bid
		m_sUserID.clear();											// user id
		m_sUserIP.clear();											// user ip
		m_sArea.clear();												// user area
		m_sPId.clear();												// pid
		m_sSlotID.clear();
		m_ltSiteCategory.clear();				    				// site category
		m_sSize.clear();												// size
		m_sUserAgent.clear();										// user agent
		m_sUserUrl.clear();											// user url
		m_ltUalist.clear();
		m_ltCreate.clear();										// creative
		m_ltCategory.clear();									// category
		m_ltSensitive.clear();									// sensitive
		m_sMinPrice.clear();										// min_cpm_price
		/* video */
		m_sVideoMinTime.clear();									// video min time
		m_sVideoMaxTime.clear();									// video max time
		m_sLinearity.clear();										// linearity (0:all, 1:in-stream, 2:overlay)
		m_ltMaterialFormat.clear();							// allow material format (1:png, 2:jpg, 3:gif, 4:flv, 5:flash, 6:swf 7:hmtl 8:MP4 )
		/* mobil */
		m_ltMobileOperator.clear();							// mobile operator
		m_ltMobileDevice.clear();									// DEV_TYPE 0:unknown 1:iPhone 2:Android 3:iPad 4:Windows Phone 5:Android Pad
		m_ltMobileConnect.clear();								// CONN_TYPE
		m_sIDFA.clear();
		m_sIMEI.clear();
		m_sAndroidID.clear();
		/* yichuanmei */
		m_sTransaction.clear();									// Transaction
		m_sDevUniqueType.clear();		   							// device unique type
		/* geo */
		m_sLat.clear();
		m_sLon.clear();

		m_ltCrowdCategory.clear();   

		m_bMobile = false;
		
		m_sPara1.clear();
		m_sPara2.clear();
		m_sPara3.clear();
		m_sPara4.clear();
		m_sPara5.clear();

		m_uiRequestId = 0;
		m_cTime.update();
	};

	bool checkLife()
	{
		UINT uiTime = m_cTime.toNow();
		if(uiTime > AD_RESPONSE_MAX_TIME)
			return true;
		return false;
	};
	
	UINT getLife()
	{
		return m_cTime.toNow();
	};

	ULONG  getTime()
	{
		return m_cTime.getTime();
	};

	void display()
	{
		AD_PRINT("Request Info(%d ns)--------------------\n", getLife());
		AD_PRINT("adx: %d\n",m_uiAdx);
		AD_PRINT("Flow: %s\n",m_sFlow.c_str());
		AD_PRINT("ViewScreen: %s\n",m_sAdViewScreen.c_str());
		AD_PRINT("ViewScreenType: %s\n",m_sAdViewType.c_str());
		AD_PRINT("BidId: %s\n",m_sBidID.c_str());
		AD_PRINT("UserId: %s\n",m_sUserID.c_str());
		AD_PRINT("UserIP: %s\n",m_sUserIP.c_str());
		AD_PRINT("Area: %s\n",m_sArea.c_str());
		AD_PRINT("Pid: %s\n",m_sPId.c_str());
		AD_PRINT("Size: %s\n",m_sSize.c_str());
		AD_PRINT("UserAgent: %s\n",m_sUserAgent.c_str());
		AD_PRINT("Latitude: %s\n", m_sLat.c_str());
		AD_PRINT("Longitude: %s\n", m_sLon.c_str());
		if( !m_ltUalist.empty()){
			list<string>::iterator iter;
			AD_PRINT("UserAgentID:");
			for( iter = m_ltUalist.begin(); iter != m_ltUalist.end(); iter++)
			{
				AD_PRINT("%s ", (*iter).c_str());
			}
			AD_PRINT("\n");
		}
		if( !m_ltCrowdCategory.empty()){
			list<string>::iterator iter;
			AD_PRINT("UserMarks:");
			for( iter = m_ltCrowdCategory.begin(); iter != m_ltCrowdCategory.end(); iter++)
			{
				AD_PRINT("%s ", (*iter).c_str());
			}
			AD_PRINT("\n");
		}		
		AD_PRINT("UserUrl: %s\n",m_sUserUrl.c_str());
		AD_PRINT("MinPrice: %s\n", m_sMinPrice.c_str());
	}

	UINT m_uiAdx;											// league
	string m_sFlow;											// flow (0:pc, 1:video, 2:mobile)
	string m_sAdViewScreen;									// adspace view screen 1: first screen 2: no first screen 
	string m_sAdViewType;									// adspace view type 1: 
	string m_sBidID;											// bid
	string m_sUserID;											// user id
	string m_sUserIP;											// user ip
	string m_sArea;												// user area
	string m_sPId;												// pid
	string m_sSlotID;
	list<string>  m_ltSiteCategory;				    				// site category
	string m_sSize;												// size
	string m_sUserAgent;										// user agent
	string m_sUserUrl;											// user url
	list<string> m_ltUalist;									// user agent ID list
	list<string> m_ltCreate;										// creative
	list<string> m_ltCategory;									// ban category
	list<string> m_ltSensitive;									// sensitive
	string m_sMinPrice;										// min_cpm_price
	/* video */
	string m_sVideoMinTime;									// video min time
	string m_sVideoMaxTime;									// video max time
	string m_sLinearity;										// linearity (0:all, 1:in-stream, 2:overlay)
	list<string> m_ltMaterialFormat;							// allow material format (1:png, 2:jpg, 3:gif, 4:flv, 5:flash, 6:swf 7:hmtl 8:MP4 )
	/* mobil */
	list<string> m_ltMobileOperator;							// mobile operator
	list<string> m_ltMobileDevice;									// DEV_TYPE 0:unknown 1:iPhone 2:Android 3:iPad 4:Windows Phone 5:Android Pad
	list<string> m_ltMobileConnect;								// CONN_TYPE
	string m_sIDFA;												// idfa
	string m_sIMEI;												// imei
	string m_sAndroidID;											// android id
	/* yichuanmei */
	string m_sTransaction;									// Transaction
	string m_sDevUniqueType;		   							// device unique type
	/* geo */
	string m_sLat;											// wei du
	string m_sLon;											// jing du

	
	list<string> m_ltCrowdCategory;                                                    //use for capture data for icache, no for ibid system, need rebuild

	//Request Get time
	Time m_cTime;
	UINT   m_uiRequestId;

	bool m_bMobile;

	string m_sPara1;
	string m_sPara2;
	string m_sPara3;
	string m_sPara4;
	string m_sPara5;
};

class AdRtbResponse
{
public:
	AdRtbResponse()
	{
		m_sDuration.reserve(AD_BYTE16);
		m_sAdvertiserId.reserve(AD_BYTE16);
		m_sTanxID.reserve(AD_BYTE16);
		m_sRegion.reserve(AD_BYTE32);
		m_sArea.reserve(AD_BYTE32);
		m_sHotKey.reserve(AD_BYTE64);
		m_sHotUrl.reserve(AD_BYTE64);
		m_sHotTag.reserve(AD_BYTE64);
		m_sHotTime.reserve(AD_BYTE32);
		m_sBidID.reserve(AD_BYTE64);
		m_sExecID.reserve(AD_BYTE32);
		m_sUserID.reserve(AD_BYTE256);
		m_sUserIP.reserve(AD_BYTE32);
		m_sPId.reserve(AD_BYTE64);
		m_sCreateGID.reserve(AD_BYTE32);
		m_sSize.reserve(AD_BYTE32);
		m_sOrderID.reserve(AD_BYTE32);
		m_sCreativeID.reserve(AD_BYTE32);
		m_sMaterialID.reserve(AD_BYTE32);
		m_sUud.reserve(AD_BYTE32);
		m_sCreative.reserve(AD_BYTE32);
		m_sTargetPriceType.reserve(AD_BYTE32);
		m_sTargetPrice.reserve(AD_BYTE32);
		m_sMaxPrice.reserve(AD_BYTE32);
		m_sCategory.reserve(AD_BYTE32);
		m_sHtml.reserve(AD_KBYTES4);
		m_sHtmlMraid.reserve(AD_KBYTES1);
		m_sDestUrl.reserve(AD_KBYTES4);
		m_sClickUrl.reserve(AD_KBYTES4);
		m_sMaterailFormat.reserve(AD_BYTE16);
		m_sAdirectID.reserve(AD_BYTE64);
		m_sDevType.reserve(AD_BYTE16);
		m_sCookID.reserve(AD_BYTE64);
		clear();
	};
	virtual ~AdRtbResponse() 
	{
		clear();
	};

	void clear()
	{
		m_uiAdx = 0;						// league
		m_sFlow.clear();						// flow (0:pc, 1:video, 2:mobile)
		m_sDuration.clear();					// Flash Duration
		m_sAdvertiserId.clear();					// advertiser id
		m_sTanxID.clear();						// tanx ad id
		m_sRegion.clear();						// region
		m_sArea.clear();							// user area
		m_sHotKey.clear();						// hot key
		m_sHotUrl.clear();						// hot url
		m_sHotTag.clear();						// hot tag
		m_sKeyWord.clear();                                        	//keyword
		m_sHotTime.clear();						// hot time
		m_sBidID.clear();						// bid
		m_sExecID.clear();						// executeID
		m_sUserID.clear();						// user id
		m_sUserIP.clear();						// user ip
		m_sPId.clear();							// pid
		m_sCreateGID.clear();					// create group id
		m_sSize.clear();							// size
		m_sOrderID.clear();						// orderID
		m_sCreativeID.clear();					// creative_id
		m_sMaterialID.clear();					// materialID
		m_sUud.clear();							// allyes creative id
		m_sCreative.clear();						// creative
		m_sTargetPriceType.clear();				// billing_mode
		m_sTargetPrice.clear();					// target price
		m_sMaxPrice.clear();					// max_cpm_price
		m_sCategory.clear();						// category
		m_sHtml.clear();						// html_snippet
		m_sWidth.clear();
		m_sHeight.clear();
		m_sHtmlMraid.clear();					// html_snippet_mraid
		m_sDestUrl.clear();						// destination_url
		m_sClickUrl.clear();						// click_through_url
		m_sMaterailFormat.clear();				// mateformat(1:png, 2:jpg, 3:gif, 4:flv, 5:flash, 6:swf 7:hmtl 8:MP4 )
		m_sAdirectID.clear();					// geo uid
		m_sDevType.clear();						// DEV_TYPE
		m_sCookID.clear();						// cookie id
		m_ltMonitorUrl.clear();			// monitor url	
	}

	void display()
	{
		AD_PRINT("Response Info--------------------\n");
		AD_PRINT("Adx: %d\n", m_uiAdx);
		AD_PRINT("Flow: %s\n", m_sFlow.c_str());
		AD_PRINT("Duration: %s\n", m_sDuration.c_str());
		AD_PRINT("Advertiser: %s\n", m_sAdvertiserId.c_str());
		AD_PRINT("TanxId: %s\n", m_sTanxID.c_str());
		AD_PRINT("Region: %s\n", m_sRegion.c_str());
		AD_PRINT("Area: %s\n", m_sArea.c_str());
		AD_PRINT("HotKey: %s\n", m_sHotKey.c_str());
		AD_PRINT("HotUrl: %s\n", m_sHotUrl.c_str());
		AD_PRINT("HotTag: %s\n", m_sHotTag.c_str());
		AD_PRINT("HotTime: %s\n", m_sHotTime.c_str());
		AD_PRINT("BidId: %s\n", m_sBidID.c_str());
		AD_PRINT("ExecuteId: %s\n", m_sExecID.c_str());
		AD_PRINT("UserId: %s\n", m_sUserID.c_str());
		AD_PRINT("UserIP: %s\n", m_sUserIP.c_str());
		AD_PRINT("PID: %s\n", m_sPId.c_str());
		AD_PRINT("CreateGID: %s\n", m_sCreateGID.c_str());
		AD_PRINT("CreateID: %s\n", m_sCreativeID.c_str());
		AD_PRINT("Size: %s\n", m_sSize.c_str());
		AD_PRINT("OrderId: %s\n", m_sOrderID.c_str());
		AD_PRINT("MaterialId: %s\n", m_sMaterialID.c_str());
		AD_PRINT("Uud: %s\n", m_sUud.c_str());
		AD_PRINT("Creative: %s\n", m_sCreative.c_str());
		AD_PRINT("PriceType: %s\n", m_sTargetPriceType.c_str());
		AD_PRINT("Price: %s\n", m_sTargetPrice.c_str());
		AD_PRINT("MaxPrice: %s\n", m_sMaxPrice.c_str());
		AD_PRINT("Category: %s\n", m_sCategory.c_str());
		AD_PRINT("Html: %s\n", m_sHtml.c_str());
		AD_PRINT("Html Mraid: %s\n", m_sHtmlMraid.c_str());
		AD_PRINT("DestUrl: %s\n", m_sDestUrl.c_str());
		AD_PRINT("ClickUrl: %s\n", m_sClickUrl.c_str());
		AD_PRINT("Material Format: %s\n", m_sMaterailFormat.c_str());
		AD_PRINT("AdirectsID: %s\n", m_sAdirectID.c_str());
		AD_PRINT("DevType: %s\n", m_sDevType.c_str());
		AD_PRINT("Cookie: %s\n", m_sCookID.c_str());
	}
	
	/* rtb */
	UINT m_uiAdx;						// league
	string m_sFlow;						// flow (0:pc, 1:video, 2:mobile)
	string m_sDuration;					// Flash Duration
	string m_sAdvertiserId;					// advertiser id
	string m_sTanxID;						// tanx ad id
	string m_sRegion;						// region
	string m_sArea;							// user area
	string m_sHotKey;						// hot key
	string m_sHotUrl;						// hot url
	string m_sHotTag;						// hot tag
	string m_sKeyWord;
	string m_sHotTime;						// hot time
	string m_sBidID;						// bid
	string m_sExecID;						// executeID
	string m_sUserID;						// user id
	string m_sUserIP;						// user ip
	string m_sPId;							// pid
	string m_sCreateGID;					// create group id
	string m_sSize;							// size
	string m_sOrderID;						// orderID
	string m_sCreativeID;					// creative_id
	string m_sMaterialID;					// materialID
	string m_sUud;							// allyes creative id
	string m_sCreative;						// creative
	string m_sTargetPriceType;				// billing_mode
	string m_sTargetPrice;					// target price
	string m_sMaxPrice;					// max_cpm_price
	string m_sCategory;						// category
	string m_sHtml;						// html_snippet
	string m_sWidth;					// material width
	string m_sHeight;					// materail height
	string m_sHtmlMraid;					// html_snippet_mraid
	string m_sDestUrl;						// destination_url
	string m_sClickUrl;						// click_through_url
	string m_sMaterailFormat;				// mateformat(1:png, 2:jpg, 3:gif, 4:flv, 5:flash, 6:swf 7:hmtl 8:MP4 )
	string m_sAdirectID;					// geo uid
	string m_sDevType;						// DEV_TYPE
	string m_sCookID;						// cookie id
	list<string> m_ltMonitorUrl;			// monitor url	
};

#endif

