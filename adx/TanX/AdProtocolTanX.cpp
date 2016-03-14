/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolTanX.h"
#include "tanx-bidding.pb.h"
#include <string.h>
#include <stdlib.h>

typedef Tanx::BidRequest_Mobile_Device 		TANXREQ_MOB_DEV;
typedef Tanx::BidRequest_Mobile 			TANXREQ_MOB;
typedef Tanx::BidRequest_Video  				TANXREQ_VIDEO;
typedef Tanx::BidRequest					TANXREQ;
typedef Tanx::BidResponse					TANXRES;
typedef Tanx::BidResponse_Ads				TANXRES_ADS;
typedef Tanx::BidRequest_AdzInfo			TANXRES_ADZ;
typedef Tanx::BidRequest_CrowdCategory          TANXRES_CROWD;

#define	AD_TANX_PRICE_CLICK		"p=%%SETTLE_PRICE%%&tanxclick=%%CLICK_URL_PRE_ENC%%"
#define   AD_TANX_IMGURL             		"http://cms.tanx.com/t.gif?tanx_nid=43124750&tanx_cm"																																 
#define	AD_TANX_PRICE				"p=%%SETTLE_PRICE%%"
#define AD_TANX_CLICK			"tanxclick=%%CLICK_URL_PRE_ENC%%"
#define	AD_TANX_HTML				"<html><head><style type=\"text/css\">*{margin: 0; padding: 0; border:none;}</style></head><body>%s</body></html>"
#define   AD_TANX_FEED          			"http://tanxlog.istreamsche.com/tshow?%s&info=%s"

#define TANX_STR_VERSION   (pPara->m_ctRequest.m_sPara1)
#define TANX_STR_ADZAPI   (pPara->m_ctRequest.m_sPara2)

int AdProtocolTanX::HttpToInternal(AdAdapterPara * pPara)
{
	int iChange = 0;

	TANXRES_ADZ* pCAdz = NULL;

	TANXREQ_MOB_DEV CReqmd;
	TANXREQ_VIDEO     CReqv;
	TANXREQ_MOB CReqm;

	TANXREQ CReq;
	char* pBody = pPara->m_pBody;
	int iBody = pPara->m_iBody;
	int iAdz = 0;
	int iCount = 0;
	char sVal[AD_BYTE512];

	int iFlow = AD_PROTOCOL_FLOW_PC;

	TANX_STR_ADZAPI= "0";
	TANX_STR_VERSION= "0";

	/* tanx */
	if (true == CReq.ParseFromArray(pBody, iBody)) 
	{
		// required int32 version = 1;
		if (true == CReq.has_version())
		{
			snprintf(sVal, AD_BYTE512, "%d", CReq.version());
			TANX_STR_VERSION = sVal;
		}
		else
		{
		}
	
		// required string bid = 2;
		if (true == CReq.has_bid()) 
		{
			pPara->m_ctRequest.m_sBidID = CReq.bid();
		}
		else 
		{
		}
	
		// optional uint32 is_ping = 12 [default = 0];
		if (true == CReq.has_is_ping()) 
		{
			if (0 != CReq.is_ping())
			{
				return AD_FAILURE;
			}
		}
		else 
		{
		}
	
		// optional string tid = 3;
		if (true == CReq.has_tid()) 
		{
			pPara->m_ctRequest.m_sUserID = CReq.tid();
		}
		else
		{
		}
	
		// optional string ip = 4;
		if (true == CReq.has_ip()) 
		{
			pPara->m_ctRequest.m_sUserIP = CReq.ip();
		}
		else 
		{
		}
	
		// optional string user_agent = 5;
		if (true == CReq.has_user_agent())
		{
			pPara->m_ctRequest.m_sUserAgent = CReq.user_agent();
			AdUaParser(pPara);
		}
		else
		{
		}
	
		// repeated string excluded_click_through_url = 6;
		if (CReq.excluded_click_through_url_size() > 0) 
		{
			for(iCount = 0; iCount < CReq.excluded_click_through_url_size(); iCount ++) 
			{
				//GT_AT_STRING(GT_RD_EURL, CReq.excluded_click_through_url(iCount));
			}
		}
		else 
		{
		}
	
		// optional string url = 7;
		if (true == CReq.has_url()) 
		{
			pPara->m_ctRequest.m_sUserUrl = CReq.url();
		}
		else 
		{
		}
	
		// optional uint32 category = 8;
		if (true == CReq.has_category()) 
		{
			snprintf(sVal, AD_BYTE512, "%d", CReq.category());
			pPara->m_ctRequest.m_ltSiteCategory.push_back(sVal);
		}
		else 
		{
		}
	
		// optional uint32 adx_type = 9 [default = 0];
		if (true == CReq.has_adx_type()) 
		{
		}
		else 
		{
		}
	
		// repeated AdzInfo adzinfo = 10;
		for (iAdz = 0; iAdz < CReq.adzinfo_size(); iAdz ++) 
		{
			pCAdz = const_cast<TANXRES_ADZ * >(&CReq.adzinfo(iAdz));
	
			// required uint32 id = 1;
			if (true == pCAdz->has_id()) 
			{	
			}
			else
			{
			}
	
			// required string pid = 2;
			if (true == pCAdz->has_pid()) 
			{
				pPara->m_ctRequest.m_sPId = pCAdz->pid();
			}
			else
			{
			}
	
			// optional string size = 3;
			if (true == pCAdz->has_size())
			{
				pPara->m_ctRequest.m_sSize = pCAdz->size();
			}
			else 
			{
			}
	
			// optional uint32 ad_bid_count = 4 [default = 2];
			if (true == pCAdz->has_ad_bid_count())
			{
				//no use
			}
			else 
			{
			}
	
			// repeated uint32 view_type = 5;
			if (pCAdz->view_type_size() > 0) 
			{
				for (iCount = 0; iCount < pCAdz->view_type_size(); iCount ++)
				{
					switch(pCAdz->view_type(iCount))
					{
						case 1:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_FIXED;
							break;
						case 5:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_POPUP;
							break;
						case 7:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_POP;
							break;
						case 12:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_VIDEOPOUSE;
							break;
						case 11:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_VIDEOPATCH;
							break;
						case 101:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_WIRELESS_OPENSCREEN;
							break;
						case 102:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_WIRELESS_POPUP;
							break;
						case 103:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_WIRELESS_BANNER;
							break;
						case 106:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_WIRELESS_VIDEO;
							break;
						case 107:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_WIRELESS_VIDEO;
							break;
						case 111:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_WIRELESS_NATIVE;
							break;
						default:					
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_OTHRE;							
							break;
					}
				}
			}
			else
			{
			}
	
			// repeated uint32 excluded_filter = 6;
			if (pCAdz->excluded_filter_size() > 0) 
			{
				for (iCount = 0; iCount < pCAdz->excluded_filter_size(); iCount ++)
				{
					snprintf(sVal, AD_BYTE512, "%d", pCAdz->excluded_filter(iCount) );
					pPara->m_ctRequest.m_ltCreate.push_back(sVal);
				}
			}
			else 
			{
			}
	
			// optional uint32 min_cpm_price = 7;
			if (true == pCAdz->has_min_cpm_price()) 
			{
				snprintf(sVal, AD_BYTE512, "%d", pCAdz->min_cpm_price());
				pPara->m_ctRequest.m_sMinPrice = sVal;
			}
			else 
			{
			}
	
			// optional ViewScreen view_screen = 9 [default = SCREEN_NA];
			if (true == pCAdz->has_view_screen()) 
			{
				snprintf(sVal, AD_BYTE512, "%d", pCAdz->view_screen());
				pPara->m_ctRequest.m_sAdViewScreen= sVal;
			}
			else
			{
			}
	
			// 推广位在整个页面的渲染顺序，从0开始递增
			//TODO:optional uint32 page_session_ad_idx = 10;
			if (pCAdz->api_size() > 0)
			{
				for(iCount = 0; iCount < pCAdz->api_size(); iCount++)
				{
					if (3 == pCAdz->api(iCount) || 5 == pCAdz->api(iCount))
					{
						TANX_STR_ADZAPI= "1";
					}
					else
					{
						TANX_STR_ADZAPI= "0";
					}
				}
			}
			else
			{
			}
		}
	

		// optional uint32 is_test = 11 [default = 0];
		if (true == CReq.has_is_test())
		{
		}
		else 
		{
		}
	
		// optional int32 timezone_offset = 13;
		if (true == CReq.has_timezone_offset()) 
		{
			//no use
		}
		else 
		{
		}
	
		// repeated int32 user_vertical = 14;
		if (CReq.user_vertical_size() > 0) 
		{
			for(iCount = 0; iCount < CReq.user_vertical_size(); iCount ++) 
			{
				//no use
				//GT_AT_UINT32(GT_RD_TUVT, CReq.user_vertical(iCount));
			}
		}
		else 
		{
		}
	
		// optional string anonymous_id = 15;
		if (true == CReq.has_anonymous_id())
		{
			//???
			//GT_AT_STRING(GT_RD_TAID, CReq.anonymous_id());
		}
		else 
		{
		}
	
		// optional string detected_language = 16;
		if (true == CReq.has_detected_language()) 
		{
			//GT_AT_STRING(GT_RD_TDLG, CReq.detected_language());
		}
		else
		{
		}

		if (CReq.crowd_category_size() > 0) 
		{
			for( iCount = 0; iCount < CReq.crowd_category_size(); iCount++)
			{
				TANXRES_CROWD * pCrowdCat;
                   		pCrowdCat = const_cast<TANXRES_CROWD*>(&CReq.crowd_category(iCount));
                   		if( true == pCrowdCat->has_id())
				{
					snprintf(sVal, AD_BYTE512, "%d", pCrowdCat->id());
					pPara->m_ctRequest.m_ltCrowdCategory.push_back(sVal);
				}
			}
		}
	
		// repeated int32 excluded_sensitive_category = 17;
		if (CReq.excluded_sensitive_category_size() > 0) 
		{
			for(iCount = 0; iCount < CReq.excluded_sensitive_category_size(); iCount ++) 
			{
				snprintf(sVal, AD_BYTE512, "%d", CReq.excluded_sensitive_category(iCount));
				pPara->m_ctRequest.m_ltSensitive.push_back(sVal);
			}
		}
		else
		{
		}
	
		// optional int32 category_version = 18;
		if (true == CReq.has_category_version())
		{
			//GT_AT_UINT32(GT_RD_TCTV, CReq.category_version());
		}
		else 
		{
		}
	
		// repeated int32 excluded_ad_category = 20;
		if (CReq.excluded_ad_category_size() > 0) 
		{
			for(iCount = 0; iCount < CReq.excluded_ad_category_size(); iCount ++) 
			{
				snprintf(sVal, AD_BYTE512, "%d", CReq.excluded_ad_category(iCount));
				pPara->m_ctRequest.m_ltCategory.push_back(sVal);
			}
		}
		else 
		{
		}
		
		//lcadd
		if (CReq.has_mobile()) 
		{
			pPara->m_ctRequest.m_bMobile = true;
			iFlow = AD_PROTOCOL_FLOW_MOBILE;
			CReqm = static_cast<TANXREQ_MOB>(CReq.mobile());
			if (CReqm.has_device())
			{
				CReqmd=static_cast<TANXREQ_MOB_DEV>(CReqm.device());
				if(CReqmd.has_operator_())
				{
					switch(CReqmd.operator_())
					{
						case 0:
							break;
						case 1:
							pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_YIDONG);
							break;
						case 2:
							pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_LIANTONG);
							break;
						case 3:
							pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_DIANXIN);
							break;
						default:
							break;
					}
				}
				
				if (CReqmd.has_platform()) 
				{
					if (AD_SUCCESS == strncasecmp(CReqmd.platform().c_str(), "iphone", 6)) 
					{
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
					}
					
					if (AD_SUCCESS == strncasecmp(CReqmd.platform().c_str(), "ipad", 4)) 
					{
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
					}
	
					if (AD_SUCCESS == strncasecmp(CReqmd.platform().c_str(), "android", 7)) 
					{
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
					}
				}
				else 
				{
				}

				if(CReqmd.has_os())
				{
					if(AD_SUCCESS == strncasecmp(CReqmd.os().c_str(),"android",CReqmd.os().length()))
					{
						//pPara->m_ctRequest.m_ltMobileOperator.push_back("1");
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
					}
	
					if(AD_SUCCESS == strncasecmp(CReqmd.os().c_str(),"ios",CReqmd.os().length()))
					{
						//pPara->m_ctRequest.m_ltMobileOperator.push_back("2");
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
					}
				}
				else
				{
				}
	
				if(CReqmd.has_device_id())
				{
					pPara->m_sIDFA = CReqmd.device_id();
				}
	
				if (CReqmd.has_network()) 
				{			
					if (CReqmd.network() == 1)
					{
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_WIFI);
					}
						
					if (CReqmd.network() == 2)
					{
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_2G);
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_NOWIFI);
					}
						
					if (CReqmd.network() == 3)
					{
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_3G);
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_NOWIFI);
					}
						
					if (CReqmd.network() == 4)
					{
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_4G);
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_NOWIFI);
					}
	
				}
				else 
				{
				}
	
				if(CReqmd.has_operator_()) 
				{
					iChange = CReqmd.operator_();
					if (iChange == 1)
					{
						pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_YIDONG);
					}
						
					if (iChange == 2)
					{
						pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_LIANTONG);
					}
					
					if (iChange == 3)
					{
						pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_DIANXIN);
					}
				}
				else
				{
					
				}
			}
			else 
			{
                   
			} 
		}
		else
		{
		}
			   
		if (CReq.has_video())
		{
			iFlow = AD_PROTOCOL_FLOW_VIDEO;
			CReqv=(TANXREQ_VIDEO)(CReq.video());
	  
			if (CReqv.has_min_ad_duration())
			{
				snprintf(sVal, AD_BYTE512, "%d", CReqv.min_ad_duration());
				pPara->m_ctRequest.m_sVideoMinTime = sVal;
			}
			else 
			{
			}
	
			if (CReqv.has_max_ad_duration())
			{
                		snprintf(sVal, AD_BYTE512, "%d", CReqv.max_ad_duration());
				pPara->m_ctRequest.m_sVideoMaxTime = sVal;
			}
			else 
			{
			}
		}
		else 
		{
		}
		snprintf(sVal, AD_BYTE512, "%d", iFlow);
		pPara->m_ctRequest.m_sFlow = sVal;
	}
	else 
	{
		return AD_FAILURE;
	}
	CheckUserId(pPara);
	return AD_SUCCESS;
}

int AdProtocolTanX::InternalToHttp(AdAdapterPara * pPara)
{
	std::string STanx;

	char sHtml[AD_KBYTES4] = {0};
	char sTemp[AD_KBYTES4];
	char sShowInfo[AD_KBYTES4] = { 0 };
	TANXRES  CRes;
	TANXRES_ADS * pCAds = NULL;

	int ret;
	
	pCAds = CRes.add_ads();
	if (NULL != pCAds) 
	{
		pCAds->set_adzinfo_id(0);
		pCAds->set_ad_bid_count_idx(0);
		CRes.set_bid(pPara->m_ctResponse.m_sBidID);
		CRes.set_version(atoi(TANX_STR_VERSION.c_str()));
		pCAds->set_max_cpm_price(atoi(pPara->m_ctResponse.m_sMaxPrice.c_str()));
		pCAds->add_click_through_url(pPara->m_ctResponse.m_sClickUrl);
		pCAds->add_category(atoi(pPara->m_ctResponse.m_sCategory.c_str()));
		pCAds->add_creative_type(atoi(pPara->m_ctResponse.m_sCreative.c_str()));
		pCAds->add_destination_url(pPara->m_ctResponse.m_sDestUrl);
		pCAds->add_advertiser_ids(atoi(pPara->m_ctResponse.m_sTanxID.c_str()));
		pCAds->set_creative_id(pPara->m_ctResponse.m_sMaterialID);
		
		string sImgUrl = AD_TANX_IMGURL;
		string sExtStr;
		if (false == pPara->m_ctRequest.m_bMobile)
		{
			sExtStr = AD_TANX_PRICE_CLICK;
			if("1" == TANX_STR_ADZAPI)
			{
				pPara->m_ctResponse.m_sHtml = pPara->m_ctResponse.m_sHtmlMraid;
				ret = GetShowHtml(pPara, sExtStr, sImgUrl, sHtml, AD_KBYTES4);
			}
			else if("0" == TANX_STR_ADZAPI)
			{
				ret = GetShowHtml(pPara, sExtStr, sImgUrl, sHtml, AD_KBYTES4);
			}
			else
			{
				return AD_FAILURE;
			}
			if(ret != AD_SUCCESS)
				return AD_FAILURE;
		}
		else
		{
			sExtStr = AD_TANX_CLICK;
			if ("0" == TANX_STR_ADZAPI)
			{
				ret = GetShowHtml(pPara, sExtStr, sImgUrl, sTemp, AD_KBYTES4);
				if(ret != AD_SUCCESS)
					return AD_FAILURE;
				snprintf(sHtml, AD_KBYTES4, AD_TANX_HTML,  sTemp);
			}
			else
			{
				if ("1" == TANX_STR_ADZAPI)
				{
					//sExtStr = AD_TANX_PRICE;
					pPara->m_ctResponse.m_sHtml = pPara->m_ctResponse.m_sHtmlMraid;
					ret = GetShowHtml(pPara, sExtStr, sImgUrl, sHtml, AD_KBYTES4);
					if(ret != AD_SUCCESS)
						return AD_FAILURE;
				}
				else
				{
					return AD_FAILURE;
				}
			}

			GetShowInfo(pPara,  sShowInfo, AD_KBYTES4);
			snprintf(sTemp, AD_KBYTES4, AD_TANX_FEED, AD_TANX_PRICE, sShowInfo);
			pCAds->set_feedback_address(sTemp);
		}

		pCAds->set_html_snippet(sHtml);
		CRes.SerializeToString(&STanx);

		/* head */
		snprintf(sTemp, AD_KBYTES4, AD_HTTP200, CRes.ByteSize());

		int ret;
		ret = pPara->m_ctCacheOut.Write(sTemp ,strlen(sTemp));
		if(ret != AD_SUCCESS)
			return AD_FAILURE;

		ret = pPara->m_ctCacheOut.Append(STanx.c_str(), CRes.ByteSize());
		if(ret != AD_SUCCESS)
			return AD_FAILURE;
	}
	else 
	{
		return AD_FAILURE;
	}

	return AD_SUCCESS;
}

int AdProtocolTanX::EmptyToHttp(AdAdapterPara * pPara)
{
	TANXRES CRes;
	std::string STanx;
	char sHead[AD_KBYTES2] = { 0 };

	/* tanx */
	CRes.set_version(atoi(TANX_STR_VERSION.c_str()));
	CRes.set_bid(pPara->m_ctRequest.m_sBidID);
	CRes.SerializeToString(&STanx);

	/* head */
	snprintf(sHead, AD_KBYTES2, AD_HTTP200, CRes.ByteSize());

	int ret;
	ret = pPara->m_ctCacheOut.Write(sHead ,strlen(sHead));
	if(ret != AD_SUCCESS)
		return AD_FAILURE;

	ret = pPara->m_ctCacheOut.Append(STanx.c_str(), CRes.ByteSize());
	if(ret != AD_SUCCESS)
		return AD_FAILURE;
	
	return AD_SUCCESS;
}

int  SharedLibInit(AdProtocolBase *  & ctpPro)
{
	AdProtocolTanX *  pPro = new AdProtocolTanX;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}

