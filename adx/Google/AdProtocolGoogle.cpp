/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolGoogle.h"
#include "realtime-bidding-proto.pb.h"
#include <string.h>
#include <stdlib.h>
#include <uuid/uuid.h>

#define AD_GG_CLICK_MACRO	"%%CLICK_URL_UNESC%%"
#define AD_GG_PRICE_MACRO  "%%WINNING_PRICE_ESC%%"

#define AD_COOKIEMAPPING_GOOGLE		""

#define	AD_GOOGLE_HTTP200			const_cast<char*>("HTTP/1.0 200 OK\r\nContent-Length: %d\r\nContent-Type: application/octet-stream; charset=UTF-8\r\nServer: TornadoServer/2.2\r\n\r\n")

int AdProtocolGoogle::HttpToInternal(AdAdapterPara * pPara)
{
	int iChange = 0;

	BidRequest_AdSlot* pCAdz = NULL;
    BidRequest_AdSlot_MatchingAdData* pMatching = NULL;


	BidRequest_Device CReqmd;
	BidRequest_Video  CReqv;
	BidRequest_Mobile CReqm;

	BidRequest CReq;
	char* pBody = pPara->m_pBody;
	int iBody = pPara->m_iBody;
	int iAdz = 0;
	int iCount = 0;
	char sVal[AD_BYTE512];

	AdPrivateInfo *pPInfo = new AdPrivateInfo;
	if(pPInfo == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}

	int iFlow = AD_PROTOCOL_FLOW_PC;

	pPara->m_pPrivate = static_cast<void *>(pPInfo);
		
	/* google */
	if (true == CReq.ParseFromArray(pBody, iBody)) 
	{
	
		// required string bid = 2;
		if (true == CReq.has_id()) 
		{
			char tmpstr[128] = {0};
			uuid_t uuid;
			uuid_generate(uuid);
			uuid_unparse(uuid, tmpstr);
			pPara->m_ctRequest.m_sBidID = tmpstr;
			//snprintf(sVal, AD_BYTE512, "%s", CReq.id().c_str());
			//pPara->m_ctRequest.m_sBidID = sVal;
			//pPara->m_ctRequest.m_sBidID = CReq.id();
			//pPara->m_ctRequest.m_sBidID = byteToHexStr(&CReq.id(),16);
		}
		else 
		{
		}
	
		// optional uint32 is_ping = 12 [default = 0];
		if (true == CReq.has_is_ping()) 
		{
			if ( !CReq.is_ping() )
			{
				return AD_FAILURE;
			}
		}
		else 
		{
		}
		// optional uint32 is_test = 11 [default = 0];
		if (true == CReq.has_is_test())
		{
		}
		else 
		{
		}	
		// optional string tid = 3;
		if (true == CReq.has_google_user_id()) 
		{
			pPara->m_ctRequest.m_sUserID = CReq.google_user_id();
		}
		else
		{
		}
	
		// optional string ip = 4;
		if (true == CReq.has_ip()) 
		{
			//pPara->m_ctRequest.m_sUserIP = CReq.ip();
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
	
	
		// optional string url = 7;
		if (true == CReq.has_url()) 
		{
			pPara->m_ctRequest.m_sUserUrl = CReq.url();
		}
		else 
		{
		}
			
		// repeated AdzInfo adzinfo = 10;
		for (iAdz = 0; iAdz < CReq.adslot_size(); iAdz ++) 
		{
			pCAdz = const_cast<BidRequest_AdSlot * >(&CReq.adslot(iAdz));
	
			// required uint32 id = 1;
			if (true == pCAdz->has_id()) 
			{	
				snprintf(sVal, AD_BYTE512, "%d", pCAdz->id());
				pPInfo->m_sSlotID = sVal;
			}
			else
			{
			}
	
			// required string pid = 2;
			if (true == pCAdz->has_ad_block_key()) 
			{
				snprintf(sVal, AD_BYTE512, "%u", (unsigned int)(pCAdz->ad_block_key()));
				pPara->m_ctRequest.m_sPId = sVal;
			}
			else
			{
			}
	
			// optional string size = 3;
			if ( pCAdz->width_size() > 0 && pCAdz->height_size() > 0 )
			{
				snprintf(sVal, AD_BYTE512, "%dx%d", pCAdz->width(0), pCAdz->height(0));
				pPara->m_ctRequest.m_sSize = sVal;
			}
			else 
			{
			}
			// publisher-excludable-creative-attributes.txt
			if ( pCAdz->excluded_attribute_size() > 0 ){
				
			}
			else{}

			// gdn-vendors.txt
			if ( pCAdz->allowed_vendor_type_size() > 0 ){
				
			}
			else{}	

			// ad-sensitive-categories.txt
			if (pCAdz->excluded_sensitive_category_size() > 0) 
			{
				for(iCount = 0; iCount < pCAdz->excluded_sensitive_category_size(); iCount ++) 
				{
					snprintf(sVal, AD_BYTE512, "%d", pCAdz->excluded_sensitive_category(iCount));
					pPara->m_ctRequest.m_ltSensitive.push_back(sVal);
				}
			}
			else
			{
			}

			// repeated int32 excluded_ad_category = 20;
			if (pCAdz->excluded_product_category_size() > 0) 
			{
				for(iCount = 0; iCount < pCAdz->excluded_product_category_size(); iCount ++) 
				{
					snprintf(sVal, AD_BYTE512, "%d", pCAdz->excluded_product_category(iCount));
					pPara->m_ctRequest.m_ltCategory.push_back(sVal);
				}
			}
			else 
			{
			}			
	
			// repeated uint32 excluded_filter = 6;
			// pPara->m_ctRequest.m_ltCreate.push_back(sVal);
            if (pCAdz->matching_ad_data_size() > 0){
                for( iCount = 0; iCount < pCAdz->matching_ad_data_size(); iCount++ ){
                    pMatching = const_cast<BidRequest_AdSlot_MatchingAdData*>( &pCAdz->matching_ad_data(iCount));
                    break;
                }
			    // optional uint32 min_cpm_price = 7;
                if (true == pMatching->has_minimum_cpm_micros()){
            	    snprintf(sVal, AD_BYTE512, "%u", (unsigned int)pMatching->minimum_cpm_micros()/10000);
				    pPara->m_ctRequest.m_sMinPrice = sVal;
                }
                else{}
                // direct_deal
                if( true == pMatching->direct_deal_size() ){
                    // to do

                }
            }
	
			// optional ViewScreen view_screen = 9 [default = SCREEN_NA];
			if (true == pCAdz->has_slot_visibility()) 
			{
				if ( pCAdz->slot_visibility() == pCAdz->NO_DETECTION){
					snprintf(sVal, AD_BYTE512, "%d", 0);
					pPara->m_ctRequest.m_sAdViewScreen= sVal;
				}
				else if ( pCAdz->slot_visibility() == pCAdz->ABOVE_THE_FOLD){
					snprintf(sVal, AD_BYTE512, "%d", 1);
					pPara->m_ctRequest.m_sAdViewScreen= sVal;
				}
				else if ( pCAdz->slot_visibility() == pCAdz->BELOW_THE_FOLD){
					snprintf(sVal, AD_BYTE512, "%d", 2);
					pPara->m_ctRequest.m_sAdViewScreen= sVal;
				}			
			}
			else
			{
			}
	
            break;
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
		if (true == CReq.detected_language_size()) 
		{
			//GT_AT_STRING(GT_RD_TDLG, CReq.detected_language());
		}
		else
		{
		}
		
		//lcadd
		if (CReq.has_mobile()) 
		{
			pPInfo->m_bMobile = true;
			iFlow = AD_PROTOCOL_FLOW_MOBILE;
			CReqm = static_cast<BidRequest_Mobile>(CReq.mobile());

			if (CReqm.has_app_id()){
				
			}
			// is full screen
			if (CReqm.has_is_interstitial_request()){
				
			}
			// app category
			if (CReqm.app_category_ids_size() > 0){

			}
			
			// 
			if (CReqm.has_encrypted_advertising_id()){
				pPInfo->m_sEncrypIdfa = CReqm.encrypted_advertising_id();
			}
			// encrypted_hashed_idfa
			if (CReqm.has_encrypted_hashed_idfa()){
				//pPara->m_ctRequest.m_sHashIdfa = CReqm.encrypted_hashed_idfa();
			}
			// hash idfa  16byte MD5
			if (CReqm.has_hashed_idfa()){
				pPara->m_sIDFA = CReqm.hashed_idfa();
				pPInfo->m_sHashIdfa = CReqm.hashed_idfa();
			}
			// constrained_usage_advertising_id
			// constrained_usage_hashed_idfa
		}
		if (CReq.has_device())
			{
				CReqmd=static_cast<BidRequest_Device>(CReq.device());
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

				/*
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
				}*/
	
				if(CReqmd.has_carrier_id()) 
				{
					iChange = CReqmd.carrier_id();
					if (iChange == 70120)
					{
						pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_YIDONG);
					}
						
					if (iChange == 70123)
					{
						pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_LIANTONG);
					}
					
					if (iChange == 70121)
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
			   
		if (CReq.has_video())
		{
			iFlow = AD_PROTOCOL_FLOW_VIDEO;
			CReqv=(BidRequest_Video)(CReq.video());
	  
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
				if ( CReqv.max_ad_duration() <= 0){// any duration is allowed
					pPara->m_ctRequest.m_sVideoMaxTime += "600";
				}
				else{
                	snprintf(sVal, AD_BYTE512, "%d", CReqv.max_ad_duration()/1000);
					pPara->m_ctRequest.m_sVideoMaxTime = sVal;
				}
			}
			else 
			{
			}
			if (pPara->m_ctRequest.m_sSize.size() == 0){
				pPara->m_ctRequest.m_sSize = "640x480";
			}
			// content_attributes
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

int AdProtocolGoogle::InternalToHttp(AdAdapterPara * pPara)
{
	std::string SGoogle;
	char sTemp[AD_KBYTES4] = {0};
	string sClickPara;	
	BidResponse  CRes;
	BidResponse_Ad * pCAds = NULL;
	BidResponse_Ad_AdSlot * pCAdSlot = NULL;
	AdPrivateInfo  * pPInfo = static_cast<AdPrivateInfo*>(pPara->m_pPrivate);

	if(pPInfo == NULL)
	{
		return AD_FAILURE;
	}

	vector<string> vStr; 
	
	CRes.set_processing_time_ms(20);
	pCAds = CRes.add_ad();
	pCAdSlot = pCAds->add_adslot();
	if (NULL != pCAds && NULL != pCAdSlot) 
	{
		
		pCAds->set_buyer_creative_id(pPara->m_ctResponse.m_sMaterialID);

		pCAds->add_vendor_type(113);//113 Image & Flash (gif, jpg, png, swf)
		// buyer-declarable-creative-attributes.txt
		pCAds->add_attribute(7); // 7 Tagging: IsTagged
		// ad-sensitive-categories.txt
		pCAds->add_category(0); //  0 Ad has no sensitive categories
		// ad-restricted-categories.txt
		//pCAds->add_restricted_category();// 

		// click_through_url
		pCAds->add_click_through_url(pPara->m_ctResponse.m_sDestUrl);
		
		AdString::StrSplit(pPara->m_ctResponse.m_sSize, vStr, "x");
		if(vStr.size() != 2){
			return AD_FAILURE;
        }
		pPara->m_ctResponse.m_sWidth += vStr[0].c_str();
		pPara->m_ctResponse.m_sHeight+= vStr[1].c_str();
		pCAds->set_width(atoi(vStr[0].c_str()));
		pCAds->set_height(atoi(vStr[1].c_str()));

		// Slot
		pCAdSlot->set_id(atoi(pPInfo->m_sSlotID.c_str()));
		// bidprice
		pCAdSlot->set_max_cpm_micros(atoi(pPara->m_ctResponse.m_sMaxPrice.c_str())*10000);
		// billing_id for BidRequest.AdSlot.matching_ad_data.billing_id
		string cmurl = AD_COOKIEMAPPING_GOOGLE;
		string price_macro = AD_GG_PRICE_MACRO;
		string click_macro = AD_GG_CLICK_MACRO;		
		// html_snippet, video_url. only one of should be set
		if(atoi(pPara->m_ctRequest.m_sFlow.c_str()) != AD_PROTOCOL_FLOW_VIDEO){
			char sSnippet[AD_KBYTES4] = {0};
			GenerateSinppet(pPara, sSnippet, cmurl, price_macro, click_macro, AD_KBYTES4);
			pCAds->set_html_snippet(sSnippet);	
		}
		// set video_url 
		else{
            #if 0
			char sVideoUrl[AD_KBYTES4] = {0};		
			GenerateVastVedio(sVideoUrl, AD_KBYTES4, pPara, price_macro, click_macro);
			pCAds->set_video_url(sVideoUrl);
            #endif
		}		
		
		CRes.SerializeToString(&SGoogle);

		/* head */
		snprintf(sTemp, AD_KBYTES4, AD_GOOGLE_HTTP200, CRes.ByteSize());

		int ret;
		ret = pPara->m_ctCacheOut.Write(sTemp ,strlen(sTemp));
		if(ret != AD_SUCCESS)
			return AD_FAILURE;

		ret = pPara->m_ctCacheOut.Append(SGoogle.c_str(), CRes.ByteSize());
		if(ret != AD_SUCCESS)
			return AD_FAILURE;	
	}
	else 
	{
		return AD_FAILURE;
	}

	delete pPInfo;
	pPara->m_pPrivate = NULL;
	return AD_SUCCESS;
}

int AdProtocolGoogle::EmptyToHttp(AdAdapterPara * pPara)
{
	BidResponse CRes;
	std::string SGoogle;
	char sHead[AD_KBYTES2] = { 0 };
	
	AdPrivateInfo  * pPInfo = static_cast<AdPrivateInfo*>(pPara->m_pPrivate);

	/* google */
	CRes.set_processing_time_ms(20);
	CRes.SerializeToString(&SGoogle);

	/* head */
	snprintf(sHead, AD_KBYTES2, AD_GOOGLE_HTTP200, CRes.ByteSize());

	if(pPInfo != NULL)
	{
		delete pPInfo;
		pPara->m_pPrivate = NULL;
	}

	int ret;
	ret = pPara->m_ctCacheOut.Write(sHead ,strlen(sHead));
	if(ret != AD_SUCCESS)
		return AD_FAILURE;

	ret = pPara->m_ctCacheOut.Append(SGoogle.c_str(), CRes.ByteSize());
	if(ret != AD_SUCCESS)
		return AD_FAILURE;	
	
	return AD_SUCCESS;
}

int  SharedLibInit(AdProtocolBase *  & ctpPro)
{
	AdProtocolGoogle *  pPro = new AdProtocolGoogle;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}


