/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/10/23
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolJuXiao.h"
#include "max-bidding_v3.0.pb.h"
#include <string.h>
#include <stdlib.h>

typedef MAX::MaxBidRequest_AdSlot                         JUXIAOREQ_ADSLOT;
typedef MAX::MaxBidRequest_Mobile_Device 		JUXIAOREQ_MOB_DEV;
typedef MAX::MaxBidRequest_Mobile 			JUXIAOREQ_MOB;
typedef MAX::MaxBidRequest					JUXIAOREQ;
typedef MAX::MaxBidResponse					JUXIAORES;
typedef MAX::MaxBidResponse_Ads				JUXIAORES_ADS;

#define	AD_JUXIAO_PRICE_CLICK	"p=%%WIN_PRICE%%&clickurl=%%CLICK_URL_ESC%%"
#define  AD_JUXIAO_IMGURL                  "http://cm.mediav.com/?mvdid=111"

AdProtocolJuXiao::AdProtocolJuXiao()
{
	m_sExtStr = AD_JUXIAO_PRICE_CLICK;
	m_sImgUrl = AD_JUXIAO_IMGURL;
}

#define JX_STR_SLOTID (pPara->m_ctRequest.m_sPara1)

int AdProtocolJuXiao::HttpToInternal(AdAdapterPara * pPara)
{
	int iAst = 0;

	JUXIAOREQ_MOB_DEV CReqmd;
	JUXIAOREQ_MOB CReqm;
	JUXIAOREQ CReq;
	JUXIAOREQ_ADSLOT * pReqast;
	
	char* pBody = pPara->m_pBody;
	int iBody = pPara->m_iBody;
	
	int iCount = 0;
	char sVal[AD_BYTE512];

	int iFlow = AD_PROTOCOL_FLOW_PC;

	JX_STR_SLOTID = "0";
		
	/* tanx */
	if (true == CReq.ParseFromArray(pBody, iBody)) 
	{
		if (true == CReq.has_bid()) 
		{
			pPara->m_ctRequest.m_sBidID = CReq.bid();
		}
		
		if (true == CReq.has_mv_user_id()) 
		{
			pPara->m_ctRequest.m_sUserID = CReq.mv_user_id();
		}
		
		if (true == CReq.has_ip()) 
		{
			pPara->m_ctRequest.m_sUserIP = CReq.ip();
		}
		
		// optional string user_agent = 5;
		if (true == CReq.has_user_agent()) 
		{
			pPara->m_ctRequest.m_sUserAgent= CReq.user_agent();
			AdUaParser(pPara);
		}
		
		// optional string url = 7;
		if (true == CReq.has_url()) 
		{
			pPara->m_ctRequest.m_sUserUrl= CReq.url();
		}

		// optional uint32 adx_type = 9 [default = 0];
		if (true == CReq.has_adx_type()) 
		{
		}
		
		for (iAst = 0; iAst < CReq.adslot_size(); iAst++)
		{
			pReqast= const_cast<JUXIAOREQ_ADSLOT * >(&CReq.adslot(iAst));
		
			if (true == pReqast->has_id())
			{
				snprintf(sVal, AD_BYTE512, "%d", pReqast->id());
				JX_STR_SLOTID = sVal;
			}
			
		
			if (true == pReqast->has_pid())
			{
				pPara->m_ctRequest.m_sPId = pReqast->pid();
			}
			
			if (true == pReqast->has_width() && true == pReqast->has_height())
			{
				snprintf(sVal,  AD_BYTE512, "%dx%d", pReqast->width(), pReqast->height());
				pPara->m_ctRequest.m_sSize = sVal;
			}

			if (pReqast->view_type_size() > 0)
			{
				for (iCount = 0; iCount < pReqast->view_type_size();iCount++)
				{
					switch(pReqast->view_type(iCount))
					{
						case 0:
							pPara->m_ctRequest.m_sAdViewType = AD_PROTOCOL_VIEW_TYPE_NO;
							break;
						case 1:
							pPara->m_ctRequest.m_sAdViewType = AD_PROTOCOL_VIEW_TYPE_FIXED;
							break;
						case 2:
							pPara->m_ctRequest.m_sAdViewType = AD_PROTOCOL_VIEW_TYPE_POPUP;
							break;
						case 5:
							pPara->m_ctRequest.m_sAdViewType = AD_PROTOCOL_VIEW_TYPE_VIDEOPOUSE;
							break;
						case 10:
							pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_WIRELESS_BANNER;
							break;
						default:
							pPara->m_ctRequest.m_sAdViewType = AD_PROTOCOL_VIEW_TYPE_OTHRE;
							break;
					}
				}
			}
			
			if (pReqast->has_min_cpm_price())
			{
				snprintf(sVal, AD_BYTE512, "%d", pReqast->min_cpm_price()/10000);
				pPara->m_ctRequest.m_sMinPrice = sVal;
			}
			
			if (pReqast->has_slot_visibility())
			{
				snprintf(sVal, AD_BYTE512, "%d", pReqast->slot_visibility());
				pPara->m_ctRequest.m_sMinPrice = sVal;
			}
		}	

		// optional uint32 is_test = 11 [default = 0];
		if (true == CReq.has_is_test())
		{
		}
		else 
		{
		}


		// repeated int32 user_vertical = 14;
		if (CReq.user_vertical_size() > 0) 
		{
			for(iCount = 0; iCount < CReq.user_vertical_size(); iCount ++)
			{
				//cout<<CReq.user_vertical(iCount))<<endl;
			}
		}

		// optional string detected_language = 16;
		if (true == CReq.has_detected_language())
		{
			std::cout << CReq.detected_language() << std::endl;
		}

		// optional int32 category_version = 18;
		if (true == CReq.has_publisher_category_version()) 
		{
			//std::cout << CReq.publisher_category_version() << std::endl;
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
		
		if (CReq.has_mobile()) 
		{
			pPara->m_ctRequest.m_bMobile = true;
			iFlow = AD_PROTOCOL_FLOW_MOBILE;
			CReqm = (JUXIAOREQ_MOB)(CReq.mobile());
			if (CReqm.has_is_app())
			{
			}
			

			if (CReqm.has_is_fullscreen())
			{
			}

			if (CReqm.has_app_id())
			{
				//pPara->m_ctRequest.m_sPId = CReqm.app_id();
			}

			if (CReqm.has_device())
			{
				CReqmd=(JUXIAOREQ_MOB_DEV)(CReqm.device());
	
				//todo os_version
				if (CReqmd.has_platform()) 
				{
					if (AD_SUCCESS == strncasecmp((const char *)(UCHAR *)CReqmd.platform().c_str(), "iphone", 6)) 
					{
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
					}

					if (AD_SUCCESS == strncasecmp((const char *)(UCHAR *)CReqmd.platform().c_str(), "ipad", 4))
					{
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
					}

					if (AD_SUCCESS == strncasecmp((const char *)(UCHAR *)CReqmd.platform().c_str(), "android", 7)) 
					{
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
					}
				}
					
				if(CReqmd.has_os()) 
				{
					if(AD_SUCCESS == strncasecmp((const char *)(UCHAR *)CReqmd.os().c_str(),"android",CReqmd.os().length()))
					{		
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
					}

					if(AD_SUCCESS == strncasecmp((const char*)(UCHAR *)CReqmd.os().c_str(),"ios",CReqmd.os().length()))
					{
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
					}
				}
					

				if (CReqmd.has_network()) 
				{
					switch(CReqmd.network())
					{
						case 1:		
							pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_WIFI);
							break;
						case 2:
							pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_2G);
							break;
						case 3:
							pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_3G);
							break;
						case 4:
							pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_4G);
							break;
						default:
							pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_OTHER);
							break;
					}
				}
					
				if (true == CReqmd.has_screen_width() && true == CReqmd.has_screen_height())
				{
					//sprintf(sVal,  AD_BYTE512, "%dx%d", CReqmd.screen_width(), CReqmd.screen_height());
				}
				
				if (true == CReqmd.has_screen_orientation())
				{
					snprintf(sVal,  AD_BYTE512, "%d", CReqmd.screen_orientation());
					pPara->m_ctRequest.m_sAdViewScreen = sVal;
				}
				//todo device_id
				if(true == CReqmd.has_idfa())
				{
					pPara->m_ctRequest.m_sIDFA = CReqmd.idfa();
				}
				if(true == CReqmd.has_hashed_idfa())
				{
					if (pPara->m_ctRequest.m_sIDFA.empty() ){
						pPara->m_ctRequest.m_sIDFA = CReqmd.hashed_idfa();
					}
				}
				
				if(true == CReqmd.has_imei())
				{
					pPara->m_ctRequest.m_sIMEI= CReqmd.imei();
				}
				if(true == CReqmd.has_hashed_imei())
				{
					if (pPara->m_ctRequest.m_sIMEI.empty() ){
						pPara->m_ctRequest.m_sIMEI= CReqmd.hashed_imei();
					}
				}	

				if(true == CReqmd.has_android_id())
				{
					pPara->m_ctRequest.m_sAndroidID = CReqmd.android_id();
				}
				if(true == CReqmd.has_hashed_android_id())
				{
					if (pPara->m_ctRequest.m_sAndroidID.empty() ){
						pPara->m_ctRequest.m_sAndroidID= CReqmd.hashed_android_id();
					}
				}

				
			}
			
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


int AdProtocolJuXiao::InternalToHttp(AdAdapterPara * pPara)
{
	std::string sJuXiao;
	int ret;
	
	char sTemp[AD_KBYTES4] = {0};
	char sHtml[AD_KBYTES4] = { 0 };
	JUXIAORES  CRes;
	JUXIAORES_ADS * pCAds = NULL;

	vector<string> vStr; 
	
	pCAds = CRes.add_ads();
	if (NULL != pCAds) 
	{
		CRes.set_bid(pPara->m_ctResponse.m_sBidID);
		pCAds->set_adslot_id(atoi(JX_STR_SLOTID.c_str()));
		pCAds->set_max_cpm_price(atoi(pPara->m_ctResponse.m_sMaxPrice.c_str()) * 10000);
		pCAds->add_category(atoi(pPara->m_ctResponse.m_sCategory.c_str()));
		// must be 1:html
		pCAds->add_creative_type(1);
		pCAds->add_destination_url(pPara->m_ctResponse.m_sDestUrl);
		pCAds->set_advertiser_id(pPara->m_ctResponse.m_sAdvertiserId);
		pCAds->set_creative_id(pPara->m_ctResponse.m_sMaterialID);

		AdString::StrSplit(pPara->m_ctResponse.m_sSize, vStr, "x");
		if(vStr.size() != 2)
			return AD_FAILURE;

		pCAds->set_height(atoi(vStr[1].c_str()));
		pCAds->set_width(atoi(vStr[0].c_str()));
		

	//	pCAds->add_click_through_url(pPara->m_ctResponse.m_sClickUrl);

		ret = GetShowHtml(pPara,  m_sExtStr, m_sImgUrl, sHtml, AD_KBYTES4);
		if(ret != AD_SUCCESS)
			return AD_FAILURE;

		pCAds->set_html_snippet(sHtml);
		CRes.SerializeToString(&sJuXiao);

		/* head */
		snprintf(sTemp, AD_KBYTES4, AD_HTTP200, CRes.ByteSize());

		int ret;
		ret = pPara->m_ctCacheOut.Write(sTemp ,strlen(sTemp));
		if(ret != AD_SUCCESS)
			return AD_FAILURE;

		ret = pPara->m_ctCacheOut.Append(sJuXiao.c_str(), CRes.ByteSize());
		if(ret != AD_SUCCESS)
			return AD_FAILURE;	
	}
	else 
	{
		return AD_FAILURE;
	}

	return AD_SUCCESS;
}

int AdProtocolJuXiao::EmptyToHttp(AdAdapterPara * pPara)
{
	JUXIAORES CRes;
	std::string sJuXiao;
	char sHead[AD_KBYTES2] = { 0 };

	/* tanx */
	CRes.set_bid(pPara->m_ctRequest.m_sBidID);
	CRes.SerializeToString(&sJuXiao);

	/* head */
	snprintf(sHead, AD_KBYTES2, AD_HTTP200, CRes.ByteSize());

	int ret;
	ret = pPara->m_ctCacheOut.Write(sHead ,strlen(sHead));
	if(ret != AD_SUCCESS)
		return AD_FAILURE;

	ret = pPara->m_ctCacheOut.Append(sJuXiao.c_str(), CRes.ByteSize());
	if(ret != AD_SUCCESS)
		return AD_FAILURE;

	return AD_SUCCESS;
}

int  SharedLibInit(AdProtocolBase *  & ctpPro)
{
	AdProtocolJuXiao *  pPro = new AdProtocolJuXiao;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}

