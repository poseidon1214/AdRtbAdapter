/***************************************
*	Description: Rtb protocol from OpenRtb
*	Author: wilson
*	Date: 2016/03/01
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef __AD_OPEN_RTB_H__
#define __AD_OPEN_RTB_H__
#include <string>
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include "AdGlobal.h"
#include "AdTime.h"
#include "json/json.h"
#include "AdSession.h"

using namespace std;

#define AD_RESPONSE_MAX_TIME    100000

#define TOJSON_STRING(json, name, str)        do{ \
	if(!str.empty())                                         \
	{                                                             \
		json[name] = str;                              \
	}                                                             \
}while(0)

#define TOJSON_OBJECT(json, name, obj)        do{ \
      	Json::Value sObj;                                              \
      	obj.toJson(sObj);                                               \
	if(!sObj.empty())										 \
	{															  \
		json[name] = sObj;							   \
	}															  \
}while(0)


#define TOJSON_ARRAYSTRING(json, name, array)        do{ \
	if(!array.empty())										 \
	{	                                                                                  \
		vector<string>::iterator iter;                                            \
		Json::Value  sArray;                                                       \
		for(iter = array.begin(); iter != array.end(); iter ++)         \
		{                                                                                     \
			sArray.append(*iter);                                                  \
		}                                                                                       \
		json[name] = sArray;                                                     \
	}													  \
}while(0)

#define TOJSON_ARRAYINT(json, name, array)        do{ \
			if(!array.empty())										 \
			{																					  \
				vector<INT32>::iterator iter;											  \
				Json::Value  sArray;													   \
				for(iter = array.begin(); iter != array.end(); iter ++) 		\
				{																					  \
					sArray.append(*iter);												   \
				}																						\
				json[name] = sArray;													 \
			}													  \
		}while(0)


#define TOJSON_ARRAYIMP(json, name, array)        do{ \
	if(!array.empty())										 \
	{																					  \
		vector<AdRtbImp>::iterator iter;											  \
		Json::Value  sArray;                                                             \
		Json::Value  sTemp;                                                             \
		for(iter = array.begin(); iter != array.end(); iter ++) 		\
		{                                								  \
			iter->toJson(sTemp);                                                  \
			sArray.append(sTemp);												   \
			sTemp.clear();                                                              \
		}														\
		json[name] = sArray;													 \
	}													  \
}while(0)



#define TOJSON_INT(json, name, num)        do{ \
	if(num >= 0)                                         \
	{                                                             \
		json[name] = num;                              \
	}                                                             \
}while(0)

#define TOJSON_FLOAT(json, name, num)        do{ \
		if(num > 0.0)										 \
		{															  \
			json[name] = num;							   \
		}															  \
	}while(0)



class AdRtbPMP
{
public:
	AdRtbPMP()
	{
		clear();
	};
	virtual ~AdRtbPMP()
	{
		clear();
	};
	void clear()
	{
	};
};


class AdRtbVideo
{
public:
	AdRtbVideo()
	{
		clear();
	};
	virtual ~AdRtbVideo()
	{
		clear();
	};
	void clear()
	{
		m_vMimes.clear();
		m_iLinearity = AD_FAILURE;
		m_iMinDuration = AD_FAILURE;
		m_iMaxDuration = AD_FAILURE;
		m_iWidth = AD_FAILURE;
		m_iHeight = AD_FAILURE;
		m_iPos = AD_FAILURE;
	};

	vector<string> m_vMimes;
	INT32 m_iLinearity;
	INT32 m_iMinDuration;
	INT32 m_iMaxDuration;
	INT32 m_iWidth;
	INT32 m_iHeight;
	INT32 m_iPos;

	void toJson(Json::Value & sJson)
	{
		TOJSON_INT(sJson, "w", m_iWidth);
		TOJSON_INT(sJson, "h", m_iHeight);
		TOJSON_INT(sJson, "pos", m_iPos);
		TOJSON_INT(sJson, "linearity", m_iLinearity);
		TOJSON_INT(sJson, "minduration", m_iMinDuration);
		TOJSON_INT(sJson, "maxduration", m_iMaxDuration);

		TOJSON_ARRAYSTRING(sJson,"mimes",m_vMimes);
	};
};


class AdRtbBanner
{
public:
	AdRtbBanner()
	{
		clear();
	};
	virtual ~AdRtbBanner()
	{
		clear();
	};
	void clear()
	{
		m_iWidth = AD_FAILURE;
		m_iHeight = AD_FAILURE;
		m_iPos = AD_FAILURE;
		m_vBType.clear();
	};

	INT32 m_iWidth;
	INT32 m_iHeight;
	INT32 m_iPos;
	vector<INT32> m_vBType;

	void toJson(Json::Value & sJson)
	{
		TOJSON_INT(sJson, "w", m_iWidth);
		TOJSON_INT(sJson, "h", m_iHeight);
		TOJSON_INT(sJson, "pos", m_iPos);
		TOJSON_ARRAYINT(sJson,"btype",m_vBType);
	};
};


class AdRtbImp
{
public:
	AdRtbImp()
	{
		clear();
	};
	virtual ~AdRtbImp()
	{
		clear();
	};
	void clear()
	{
		m_sId.clear();
		m_sTagId.clear();
		m_fBidFloor = 0.0;

		m_cBanner.clear();
		m_cVideo.clear();
		m_cPmp.clear();

//		m_iShowType = AD_FAILURE;
//		m_iHasWinnotice = AD_FAILURE;
//		m_iHasClickThrough = AD_FAILURE;
	};

	string m_sId;                          //impressId
	string m_sTagId;                     //pid
	FLOAT  m_fBidFloor;                 //cpm

	AdRtbBanner  m_cBanner;         
	AdRtbVideo    m_cVideo;
	AdRtbPMP      m_cPmp;

	//ext no use
//	INT32  m_iShowType;
//	INT32  m_iHasWinnotice;
//	INT32  m_iHasClickThrough;

	void toJson(Json::Value & sJson)
	{
		TOJSON_STRING(sJson, "id", m_sId);
		TOJSON_STRING(sJson, "tagid", m_sTagId);
		TOJSON_FLOAT(sJson, "bidfloor", m_fBidFloor);

		TOJSON_OBJECT(sJson, "banner", m_cBanner);
		TOJSON_OBJECT(sJson, "video", m_cVideo);
	};
};

class AdRtbSiteContent
{
public:
	AdRtbSiteContent()
	{
		clear();
	};
	virtual ~AdRtbSiteContent()
	{
		clear();
	};
	void clear()
	{
		m_sTitle.clear();
		m_vCat.clear();
		m_sUrl.clear();
		m_sKeywords.clear();
	};

	string m_sTitle;
	vector<string> m_vCat;
	string m_sUrl;
	string m_sKeywords;

	void toJson(Json::Value & sJson)
	{
		TOJSON_STRING(sJson, "title", m_sTitle);
		TOJSON_STRING(sJson, "url", m_sUrl);
		TOJSON_STRING(sJson, "keywords", m_sKeywords);
		TOJSON_ARRAYSTRING(sJson,"cat", m_vCat);
	};
};

class AdRtbSite
{
public:
	AdRtbSite()
	{
		clear();
	};
	virtual ~AdRtbSite()
	{
		clear();
	};
	void clear()
	{
		m_sName.clear();
		m_sPage.clear();
		m_sRefer.clear();
		m_vCat.clear();
		m_cContent.clear();
	};

	string m_sName;
	string m_sPage;
	string m_sRefer;
	vector<string> m_vCat;
	AdRtbSiteContent m_cContent;

	void toJson(Json::Value & sJson)
	{
		TOJSON_STRING(sJson, "name", m_sName);
		TOJSON_STRING(sJson, "page", m_sPage);
		TOJSON_ARRAYSTRING(sJson,"cat", m_vCat);

		TOJSON_OBJECT(sJson, "content", m_cContent);
	};
};

class AdRtbGeo
{
public:
	AdRtbGeo()
	{
		clear();
	};
	virtual ~AdRtbGeo()
	{
		clear();
	};
	void clear()
	{
		m_fLat = 0.0;
		m_fLon = 0.0;
	};

	FLOAT m_fLat;                   //Î³¶È£¨-90~90£©
	FLOAT m_fLon;                  //¾­¶È£¨-180~180£©

	void toJson(Json::Value & sJson)
	{
		TOJSON_FLOAT(sJson, "lat", m_fLat);
		TOJSON_FLOAT(sJson, "lon", m_fLon);
	};
};


class AdRtbDevice
{
public:
	AdRtbDevice()
	{
		clear();
	};
	virtual ~AdRtbDevice()
	{
		clear();
	};
	void clear()
	{
		m_sUserAgent.clear();
		m_sIp.clear();
		m_cGeo.clear();            //Éè±¸µÄµ±Ç°Î»ÖÃÐÅÏ¢
		m_sDidmd5.clear();       //Ê¹ÓÃmd5¹þÏ£µÄdeviceååID£(Ò»°ã¶ÔÓ¦AndroidÏµÍ³MMA×Ö¶ÎµÄIMEIÖµ,iOSÏµÍ³apiÏÞÖÆ»ñÈ¡²»µ½¸ÃÖµ)
		m_sDpidmd5.clear();     //Ê¹ÓÃMD5¹þÏ£µÄÆ½Ì¨Ïà¹ØID,²»Í¬µÄÏµÍ³»á´«²»Í¬µÄÖµ£¬¾ßÌå¶ÔÓ¦µÄÒÆ¶¯¶ËMMA×Ö¶Î£º AndroidÏµÍ³»á´«--Android ID£¬iOSÏµÍ³»á´«--openudid£¬Windows PhoneÏµÍ³»á´«--DUID
		m_sMake.clear();          //Éè±¸Éú²úÉÌ£¬Èç"Apple"
		m_sModel.clear();         // Éè±¸ÐÍºÅ,Èç"iPhone" 
		m_sOS.clear();
		m_sOSV.clear();            //operater systerm version
		m_sLanguage.clear();
		m_iJSEnable = 0;              //js enable or not,   1: enable, 0:disable
		m_iConnectionType = AD_FAILURE; 
		m_iDeviceType = AD_FAILURE;

#if 0
		//ext content
			string m_sIDFA;
			string m_sMAC;
			string m_sMACMd5;
			string m_sSsid; 			  //wifi
			INT32 m_iWidth; 		   //device screen width
			INT32 m_iHeight;		   //device screen height;
			INT32 m_ibrk;				//device  breakout,   1:active	0 unactive;
			INT32 m_iTS;				//timestamp that send request.
			INT32 m_iInterstitial;	   //ÊÇ·ñÊ¹ÓÃÈ«ÆÁ/»¥¶¯·½Ê½À´Õ¹ÏÖ¹ã¸æ¡£1¡ªÊÇ£¬0¡ª·ñ£¨Ä¬ÈÏÖµ£©¡£

		//ext content
		m_sIDFA;
		m_sMAC;
		m_sMACMd5;
		m_sSsid;               //wifi
		m_iWidth = AD_FAILURE;            //device screen width
		m_iHeight = AD_FAILURE;           //device screen height;
		m_ibrk = AD_FAILURE;               //device  breakout,   1:active  0 unactive;
		m_iTS = AD_FAILURE;                //timestamp that send request.
		m_iInterstitial = AD_FAILURE;     //ÊÇ·ñÊ¹ÓÃÈ«ÆÁ/»¥¶¯·½Ê½À´Õ¹ÏÖ¹ã¸æ¡£1¡ªÊÇ£¬0¡ª·ñ£¨Ä¬ÈÏÖµ£©¡£
#endif
	};

	string m_sUserAgent;
	string m_sIp;
	AdRtbGeo m_cGeo;            //Éè±¸µÄµ±Ç°Î»ÖÃÐÅÏ¢
	string m_sDidmd5;       //Ê¹ÓÃmd5¹þÏ£µÄdeviceååID£(Ò»°ã¶ÔÓ¦AndroidÏµÍ³MMA×Ö¶ÎµÄIMEIÖµ,iOSÏµÍ³apiÏÞÖÆ»ñÈ¡²»µ½¸ÃÖµ)
	string m_sDpidmd5;     //Ê¹ÓÃMD5¹þÏ£µÄÆ½Ì¨Ïà¹ØID,²»Í¬µÄÏµÍ³»á´«²»Í¬µÄÖµ£¬¾ßÌå¶ÔÓ¦µÄÒÆ¶¯¶ËMMA×Ö¶Î£º AndroidÏµÍ³»á´«--Android ID£¬iOSÏµÍ³»á´«--openudid£¬Windows PhoneÏµÍ³»á´«--DUID
	string m_sMake;          //Éè±¸Éú²úÉÌ£¬Èç"Apple"
	string m_sModel;         // Éè±¸ÐÍºÅ,Èç"iPhone" 
	string m_sOS;
	string m_sOSV;            //operater systerm version
	string m_sLanguage;
	INT32 m_iJSEnable;              //js enable or not,   1: enable, 0:disable
	INT32 m_iConnectionType; 
	INT32 m_iDeviceType;
	void toJson(Json::Value & sJson)
	{
		TOJSON_STRING(sJson, "ua", m_sUserAgent);
		TOJSON_STRING(sJson, "ip", m_sIp);
		TOJSON_OBJECT(sJson, "geo", m_cGeo);
		TOJSON_STRING(sJson, "didmd5", m_sDidmd5);
		TOJSON_STRING(sJson, "dpidmd5", m_sDpidmd5);
		TOJSON_STRING(sJson, "make", m_sMake);
		TOJSON_STRING(sJson, "model", m_sModel);
		TOJSON_STRING(sJson, "os", m_sOS);
		TOJSON_STRING(sJson, "osv", m_sOSV);
		TOJSON_STRING(sJson, "language", m_sLanguage);

		TOJSON_INT(sJson, "js", m_iJSEnable);
		TOJSON_INT(sJson, "connectiontype", m_iConnectionType);
		TOJSON_INT(sJson, "devicetype", m_iDeviceType);
	};
};

class AdRtbUser
{
public:
	AdRtbUser()
	{
		clear();
	};
	virtual ~AdRtbUser()
	{
		clear();
	};
	void clear()
	{
		m_sId.clear();
		m_sKeywords.clear();
	};

	string m_sId;
	string m_sKeywords;
	
	void toJson(Json::Value & sJson)
	{
		TOJSON_STRING(sJson, "id", m_sId);
		TOJSON_STRING(sJson, "keywords", m_sKeywords);
	};
};

class AdRtbApp
{
public:
	AdRtbApp()
	{
		clear();
	};
	virtual ~AdRtbApp()
	{
		clear();
	};

	void clear()
	{
		m_sName.clear();
		m_sBundle.clear();
		m_vCat.clear();
		m_cContent.clear();
	};


	string m_sName;
	string m_sBundle;                //app packet name ort bundleID
	AdRtbSiteContent m_cContent;
	vector<string> m_vCat;        //¹ã¸æÎ»ÄÚÈÝ·ÖÀà£¬¼æÈÝIAB·ÖÀà£¬·ûºÏopenRTB 2.2±í¸ñ6.1µÄ·ÖÀà·½·¨¡£

	void toJson(Json::Value & sJson)
	{
		TOJSON_STRING(sJson, "name", m_sName);
		TOJSON_STRING(sJson, "bundle", m_sBundle);
		TOJSON_OBJECT(sJson, "content", m_cContent);
		TOJSON_ARRAYSTRING(sJson, "cat", m_vCat);
	};
};


class AdRtbRequest
{
public:
	AdRtbRequest()
	{
		clear();
	};
	virtual ~AdRtbRequest()
	{
		clear();
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

	ULONG getTime()
	{
		return m_cTime.getTime();
	};
	
	void clear()
	{
		m_cTime.update();
		
		m_sId.clear();
		m_vImp.clear();
		m_cSite.clear();
		m_cDevice.clear();
		m_cUser.clear();
		m_cApp.clear();
	};

	string m_sId;
	vector<AdRtbImp> m_vImp;
	AdRtbSite m_cSite;
	AdRtbDevice m_cDevice;
	AdRtbUser  m_cUser;
	AdRtbApp  m_cApp;

	void toJson(Json::Value & sJson)
	{
		TOJSON_OBJECT(sJson, "site", m_cSite);
		TOJSON_OBJECT(sJson, "device", m_cDevice);
		TOJSON_OBJECT(sJson, "user", m_cUser);
		TOJSON_OBJECT(sJson, "app", m_cApp);
		TOJSON_ARRAYIMP(sJson, "imp", m_vImp);
	};
	
private:
	Time  m_cTime;
};

class AdRtbBid
{
public:
	AdRtbBid()
	{
		clear();
	};

	virtual ~AdRtbBid()
	{
		clear();
	};

	void clear()
	{
		m_sId.clear();
		m_sImpId.clear();
		m_fPrice = 0.0;
		m_sNUrl.clear();
		m_sCrId.clear();
		m_sAdm.clear();
		m_sAdomain.clear();
		m_sDealId.clear();
	};

	string m_sId;                             //price id
	string m_sImpId;                       //impresstion id
	DOUBLE m_fPrice;                        //cpm
	string  m_sNUrl;                     //openrtb nurl, win notice url
	string  m_sCrId;                     //creative ID
	string m_sAdm;
	string m_sAdomain;
	string  m_sDealId;                   //dsp deal id

};

class AdRtbSeatbid
{
public:
	AdRtbSeatbid()
	{
		clear();
	};

	virtual ~AdRtbSeatbid()
	{
		clear();
	};

	void clear()
	{
		m_vBid.clear();
	};

	vector<AdRtbBid> m_vBid;
};

class AdRtbResponse
{
public:
	AdRtbResponse()
	{
		clear();
	};

	virtual ~AdRtbResponse()
	{
		clear();
	};

	void clear()
	{
		m_vSeatBid.clear();
	};

	string m_sId;           //request ID
	vector<AdRtbSeatbid> m_vSeatBid;
	string m_sBidID;

	int  parseJson(Json::Value & sJson)
	{
		if(true == sJson.isMember("id"))
		{
			m_sId = sJson["id"].asString();
		}

		if(true == sJson.isMember("bidid"))
		{
			m_sBidID = sJson["bidid"].asString();
		}

		if(true == sJson.isMember("seatbid"))
		{
			UINT i;
			Json::Value  sValue;
			Json::Value  subValue;
			AdRtbSeatbid   cSeatBid ;
			
			//GT_PRINT("Pid Parse!\n");
			for(i = 0; i < sJson["seatbid"].size(); i++ )
			{
				sValue = sJson["seatbid"][i];
				if(true == sValue.isMember("bid"))
				{
					INT32 a;
					AdRtbBid  cBid;
					for(a = 0; a < (INT32)sValue["bid"].size(); a ++)
					{
						subValue = sValue["bid"][a];
		
						if(true == subValue.isMember("id"))
						{
							cBid.m_sId= sJson["id"].asString();
						}

						if(true == subValue.isMember("impid"))
						{
							cBid.m_sImpId= subValue["impid"].asString();
						}

						if(true == subValue.isMember("price"))
						{
							cBid.m_fPrice = subValue["price"].asDouble();
						}

						if(true == subValue.isMember("nurl"))
						{
							cBid.m_sNUrl = subValue["nurl"].asString();
						}

						if(true == subValue.isMember("crid"))
						{
							cBid.m_sCrId= subValue["crid"].asString();
						}

						if(true == subValue.isMember("adm"))
						{
							cBid.m_sAdm= subValue["adm"].asString();
						}

						if(true == subValue.isMember("adomain"))
						{
							cBid.m_sAdomain = subValue["adomain"].asString();
						}

						if(true == subValue.isMember("dealid"))
						{
							cBid.m_sDealId = subValue["dealid"].asString();
						}

						 cSeatBid.m_vBid.push_back(cBid);
						 cBid.clear();
					}
				}
				m_vSeatBid.push_back(cSeatBid);
				cSeatBid.clear();
			}
		}
		return AD_SUCCESS;
	};
};

class AdRtbPara
{
public:
	AdRtbPara()
	{
		clear();
	};
	virtual ~AdRtbPara(){};
	void clear()
	{
		m_pCacheIn= NULL;
		m_ctCacheOut.Clear();
		
		m_pSrcSession = NULL;
		m_pDstSession = NULL;
		
		m_ctRequest.clear();
		m_ctResponse.clear();
		
		m_sJsonCb.clear();
		
		m_cHttp.Clear();

		
		m_bTimeout = false;
	}
	AdCache * m_pCacheIn;		               // Buffer
	AdCache   m_ctCacheOut;
	AdSession * m_pSrcSession;
	AdSession * m_pDstSession;
	
	AdRtbRequest m_ctRequest;
	AdRtbResponse m_ctResponse;

	string m_sJsonCb;
	INT32  m_iMid;
	
	AdHttp  m_cHttp;
	bool m_bTimeout;
};

#endif
