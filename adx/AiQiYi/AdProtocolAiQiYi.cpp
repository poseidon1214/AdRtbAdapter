/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolAiQiYi.h"
#include "bid_request.pb.h"
#include "bid_response.pb.h"
#include <string.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace ads_serving;

typedef ads_serving::proto::Banner	BANNER;
typedef ads_serving::proto::Video	VIDEO;
typedef ads_serving::proto::Impression IMP;
typedef ads_serving::proto::Site SITE;
typedef ads_serving::proto::Content CONTENT;
typedef ads_serving::proto::Device 	DEVICE;
typedef ads_serving::proto::Geo GEO;
typedef ads_serving::proto::User USER;
typedef ads_serving::proto::BidRequest REQUEST;

typedef ads_serving::proto::Settlement SETTLE;
typedef ads_serving::proto::Bid BID;
typedef ads_serving::proto::Seatbid	SEATBID;
typedef ads_serving::proto::BidResponse RESPONSE;

#define AIQIYI_IMP_ID   (pPara->m_ctRequest.m_sPara1)
#define AD_URL        const_cast<char*>("http://lg.istreamsche.com/")

#define AD_AQY_PRICE_MACRO "${SETTLEMENT}"

AdProtocolAiQiYi::AdProtocolAiQiYi()
{
	m_PC_Size["1000000000041"] = "425x320";
	//m_PC_Size["1000000000457"] = 
	m_PC_Size["1000000000381"] = "1280x720";
	m_PC_Size["1000000000538"] = "480x70";

	m_MB_Size["1000000000410"] = "428x356";
	m_MB_Size["1000000000456"] = "180x150";
	m_MB_Size["1000000000381"] = "1280x720";
}


int AdProtocolAiQiYi::HttpToInternal(AdAdapterPara * pPara)
{
	USER CUser;
	SITE CSite;
	DEVICE CDevice;
	
	
	IMP *pImp;
	GEO *pGeo = NULL;
	BANNER *pBanner = NULL;
	VIDEO *pVideo = NULL;
	CONTENT *pContent = NULL;
	

	REQUEST CReq;
	char* pBody = pPara->m_pBody;
	int iBody = pPara->m_iBody;
	int iCount = 0;
	char sVal[AD_BYTE512];

	int iFlow = AD_PROTOCOL_FLOW_PC;

	/* tanx */
	if (true == CReq.ParseFromArray(pBody, iBody)) 
	{

	
		// required string bid = 2;
		if (true == CReq.has_id()) 
		{
			pPara->m_ctRequest.m_sBidID = CReq.id();
		}
		else 
		{
		}
	
		// optional uint32 is_ping = 12 [default = 0];
		if (true == CReq.has_is_ping()) 
		{
			if (CReq.is_ping())
			{
				return AD_FAILURE;
			}
		}
		else 
		{
		}

		// user
		if( CReq.has_user())
		{
			CUser = static_cast<USER>(CReq.user());
			if( CUser.has_id())
			{
				pPara->m_ctRequest.m_sUserID = CUser.id();
			}
		}

		// site
		if( CReq.has_site())
		{
			CSite = static_cast<SITE>(CReq.site());
			if( CSite.has_id())
			{}
			if( CSite.has_content())
			{
				pContent = const_cast<CONTENT*>(&CSite.content());
				if(pContent->has_url())
				{
					pPara->m_ctRequest.m_sUserUrl = pContent->url();
				}
				if(pContent->has_channel_id())
				{
					sprintf(sVal, "%ld",  pContent->channel_id());
					pPara->m_ctRequest.m_ltSiteCategory.push_back(sVal);	
				}
			}
		}
		
		// device
		if( CReq.has_device())
		{
			CDevice = static_cast<DEVICE>(CReq.device());
			if(CDevice.has_ua())
			{
				pPara->m_ctRequest.m_sUserAgent = CDevice.ua();
				AdUaParser(pPara);				
			}
			if(CDevice.has_ip())
			{
				pPara->m_ctRequest.m_sUserIP = CDevice.ip();				
			}		
			// geo
			if(CDevice.has_geo())
			{
				pGeo = const_cast<GEO*>(&CDevice.geo());
			}
			// device
			if(CDevice.has_connection_type())
			{
				switch(CDevice.connection_type())
				{
					case 2:
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_NOWIFI);
					case 3:
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_2G);
					case 4:
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_3G);						
					case 5:
						pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_4G);				
					default:
						break;
				}
			}
			// os
			if(CDevice.has_platform_id())
			{
				switch(CDevice.platform_id())
				{
					case 11:
						iFlow = AD_PROTOCOL_FLOW_PC;
					case 12:
						iFlow = AD_PROTOCOL_FLOW_PC;
					case 13:
						iFlow = AD_PROTOCOL_FLOW_PC;
					case 21:
						iFlow = AD_PROTOCOL_FLOW_MOBILE;
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
					case 22:
						iFlow = AD_PROTOCOL_FLOW_MOBILE;
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);	
					case 23:
						iFlow = AD_PROTOCOL_FLOW_MOBILE;
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
					case 25:
						iFlow = AD_PROTOCOL_FLOW_MOBILE;
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_WPHONE);
					case 32:
						iFlow = AD_PROTOCOL_FLOW_MOBILE;
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
					case 33:
						iFlow = AD_PROTOCOL_FLOW_MOBILE;
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
					case 35:
						iFlow = AD_PROTOCOL_FLOW_MOBILE;
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_WPHONE);
					case 311:
						iFlow = AD_PROTOCOL_FLOW_MOBILE;
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
					case 312:
						iFlow = AD_PROTOCOL_FLOW_MOBILE;
						pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);	
					default:
						break;					
				}
			}
		}
		if(CReq.imp_size() > 0)
		{
			for(iCount = 0; iCount < CReq.imp_size(); iCount++)
			{
				pImp = const_cast<IMP*>(&CReq.imp(iCount));
				if(pImp->has_id())
				{
					AIQIYI_IMP_ID = pImp->id();
				}
				if(pImp->has_banner())
				{
					pBanner = const_cast<BANNER*>(&pImp->banner());
					if(pBanner->has_ad_zone_id())
					{
						pPara->m_ctRequest.m_sPId = pBanner->ad_zone_id();
					}
				}

				if(pImp->has_video())
				{
					iFlow = AD_PROTOCOL_FLOW_VIDEO;
					pVideo = const_cast<VIDEO*>(&pImp->video());
					if(pVideo->has_ad_zone_id())
					{
						pPara->m_ctRequest.m_sPId = pBanner->ad_zone_id();
					}
					if(pVideo->has_ad_type())
					{

					}
					if(pVideo->has_minduration())
					{
						sprintf(sVal, "%d",  pVideo->minduration());
						pPara->m_ctRequest.m_sVideoMinTime = sVal;	
					}
					if(pVideo->has_maxduration())
					{
						sprintf(sVal, "%d",  pVideo->maxduration());
						pPara->m_ctRequest.m_sVideoMaxTime = sVal;	
					}				
				}	
				if(pImp->has_bidfloor())
				{
					snprintf(sVal, AD_BYTE512, "%d", pImp->bidfloor());
					pPara->m_ctRequest.m_sMinPrice = sVal;
				}
				if(pImp->has_campaign_id())
					{}

				if(pImp->has_is_pmp())
					{}
				if(pImp->blocked_ad_tag_size() > 0)
				{
					for(iCount = 0; iCount < pImp->blocked_ad_tag_size(); iCount++)
					{
						snprintf(sVal, AD_BYTE512, "%d", pImp->blocked_ad_tag(iCount));
						pPara->m_ctRequest.m_ltCategory.push_back(sVal);	
					}
				}	

				break;
			}
			
		}

		if( AD_FAILURE == AdSize( pPara->m_ctRequest.m_sPId, iFlow, pPara->m_ctRequest.m_sSize))
		{
			return AD_FAILURE;
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

int AdProtocolAiQiYi::InternalToHttp(AdAdapterPara * pPara)
{
	RESPONSE CRes;
	std::string Response;
	BID * pBid = NULL;
	SEATBID *pSeat = NULL;
	//SETTLE *pSettle = NULL;
	char sTemp[AD_KBYTES8] = {0};
	
	pSeat = CRes.add_seatbid();
	if (NULL != pSeat) 
	{
		CRes.set_id(pPara->m_ctRequest.m_sBidID); 
		pBid = pSeat->add_bid();
		if(NULL != pBid)
		{
			char sVast[AD_KBYTES4] = {0};
			pBid->set_id(pPara->m_ctRequest.m_sBidID);
			pBid->set_impid(AIQIYI_IMP_ID);
			pBid->set_price(atoi(pPara->m_ctResponse.m_sMaxPrice.c_str()));
			pBid->set_crid(pPara->m_ctResponse.m_sMaterialID);

			string cmurl = "";
			string price_macro = AD_AQY_PRICE_MACRO;
			string click_macro = ""; 	
			if( AD_SUCCESS == GenerateVastVedio(pPara, sVast, cmurl, price_macro, click_macro, AD_KBYTES4))
			{
				pBid->set_adm(sVast);
			}
			else{
				return AD_FAILURE;
			}
			
		}
		
		CRes.SerializeToString(&Response);

		/* head */
		snprintf(sTemp, AD_KBYTES8, AD_HTTP200, CRes.ByteSize());

		int ret;
		ret = pPara->m_ctCacheOut.Write(sTemp ,strlen(sTemp));
		if(ret != AD_SUCCESS)
			return AD_FAILURE;

		ret = pPara->m_ctCacheOut.Append(Response.c_str(), CRes.ByteSize());
		if(ret != AD_SUCCESS)
			return AD_FAILURE;
	}
	else 
	{
		return AD_FAILURE;
	}

	return AD_SUCCESS;
}

int AdProtocolAiQiYi::AdSize(string& ad_zone_id, int flow, string& size)
{
	if( flow == AD_PROTOCOL_FLOW_PC ){
		if(m_PC_Size.find(ad_zone_id) != m_PC_Size.end()){
			size = m_PC_Size[ad_zone_id];
			return AD_SUCCESS;
		}
		else{
			return AD_FAILURE;
		}
	}
	if( flow == AD_PROTOCOL_FLOW_MOBILE )
	{
		if(m_MB_Size.find(ad_zone_id) != m_MB_Size.end()){
			size = m_MB_Size[ad_zone_id];
			return AD_SUCCESS;
		}
		else{
			return AD_FAILURE;
		}
	}
	if( flow == AD_PROTOCOL_FLOW_VIDEO ){
		if(m_PC_Size.find(ad_zone_id) != m_PC_Size.end()){
			size = m_PC_Size[ad_zone_id];
			return AD_SUCCESS;
		}
		else{
			return AD_FAILURE;
		}		
	}
	return AD_FAILURE;
}

int AdProtocolAiQiYi::GenerateVastVedio(AdAdapterPara * pPara, char * sBuf, string &cmurl, string &price_macro, string &click_macro,int iBuf)
{
	int buffersize;
	xmlChar *xmlbuff;
	string sClickPara;
	char title[AD_BYTE32] = {0};
	char sShowPara[AD_KBYTES2];
	char impurl[AD_KBYTES4] = {0};
	char clickurl[AD_KBYTES4] = {0};
	char duration[AD_BYTE16] = {0};
	
	sprintf(title, "Adirects AdvertiserID:%s", pPara->m_ctResponse.m_sAdvertiserId.c_str());
	// width height
	std::vector<string> vStr; 
	AdString::StrSplit(pPara->m_ctResponse.m_sSize, vStr, "x");
	if(vStr.size() != 2){
		return AD_FAILURE;
	}
	pPara->m_ctResponse.m_sWidth += vStr[0].c_str();
	pPara->m_ctResponse.m_sHeight+= vStr[1].c_str();

	// imp click url
	GetClickPara(&(pPara->m_ctResponse), sClickPara);
	GetShowPara(pPara, sShowPara, AD_KBYTES2 );
	snprintf( impurl,AD_KBYTES4 , "%sAQYshow?p=${SETTLEMENT}&%s",AD_URL, sShowPara);
	snprintf( clickurl, AD_KBYTES4, "%sAQYclick?%s&gc=%s&url=%s",  AD_URL, sShowPara, sClickPara.c_str(), pPara->m_ctResponse.m_sClickUrl.c_str());

	// duration
	int iDuration = atoi(pPara->m_ctResponse.m_sDuration.c_str());
	if( iDuration > 0 ){
		int minutes = 0;
		int seconds = 0;
		if( (minutes = iDuration / 60 ) > 1 ){
			seconds = iDuration - minutes*60;
		}
		else{
			seconds = iDuration;
		}
		sprintf(duration, "00:%02d:%02d", minutes, seconds);
	}
	string type;
	int iFormat = atoi(pPara->m_ctResponse.m_sMaterailFormat.c_str());
	switch(iFormat){//1:png, 2:jpg, 3:gif, 4:flv, 5:flash, 6:swf 7:hmtl 8:MP4 
		case AD_PROTOCOL_MAT_FORMAT_PNG:
			type = "image/png";
			break;
		case AD_PROTOCOL_MAT_FORMAT_JPG:
			type = "image/jpeg";
			break;
		case AD_PROTOCOL_MAT_FORMAT_GIF:
			type = "image/gif";
			break;
		case AD_PROTOCOL_MAT_FORMAT_FLV:
			type = "video/xflv";
			break;
		case AD_PROTOCOL_MAT_FORMAT_SWF:
			type = "application/x-shockwave-flash";
			break;
		case AD_PROTOCOL_MAT_FORMAT_MP4:
			type = "video/mp4";
		default:
			type = "image/jpeg";
			break;
		// 7,8	not surpport
	}


	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"VAST");
	xmlNewProp(root_node,BAD_CAST"version",BAD_CAST"3.0");

	// root
	xmlDocSetRootElement(doc, root_node);
	// ad
	xmlNodePtr ad_node = xmlNewNode(NULL,BAD_CAST"Ad");
	//xmlNewProp(ad_node,BAD_CAST"id",BAD_CAST"1");
	//xmlNewProp(ad_node,BAD_CAST"sequence",BAD_CAST"1");
	// inline
	xmlNodePtr inline_node = xmlNewNode(NULL,BAD_CAST"InLine");
	// AdSystem
	xmlNodePtr system_node = xmlNewNode(NULL,BAD_CAST"AdSystem");
	xmlNewProp(system_node,BAD_CAST"version",BAD_CAST"1.0");
	xmlNodePtr system_content = xmlNewText(BAD_CAST"Adirects DSP AD Server");
	xmlAddChild(system_node, system_content);
	// Title
	xmlNewTextChild(inline_node, NULL, BAD_CAST "AdTitle", BAD_CAST title);
	// xmlNewTextChild(inline_node, NULL, BAD_CAST "Pricing", BAD_CAST pstAtInfo->m_szbPr);
	// impression
	xmlNodePtr imp_node = xmlNewNode(NULL,BAD_CAST"Impression");
	xmlNewProp(imp_node,BAD_CAST"id",BAD_CAST"1");
	xmlNodePtr imp_content = xmlNewText(BAD_CAST impurl);
	xmlAddChild(imp_node, imp_content);
	
	// third monitor url
	if( !pPara->m_ctResponse.m_ltMonitorUrl.empty())
	{
		xmlNodePtr imp_monitor_node = xmlNewNode(NULL,BAD_CAST"Impression");
		xmlNewProp(imp_monitor_node,BAD_CAST"id",BAD_CAST"2");
		xmlNodePtr imp_monitor_content = xmlNewText(BAD_CAST (*(pPara->m_ctResponse.m_ltMonitorUrl.begin())).c_str());
		xmlAddChild(imp_monitor_node, imp_monitor_content);
		xmlAddChild(inline_node, imp_monitor_node);
	}

	// Creatives
	xmlNodePtr creatives_node = xmlNewNode(NULL,BAD_CAST"Creatives");
	xmlNodePtr creative_node = xmlNewNode(NULL,BAD_CAST"Creative");
	xmlNodePtr linear_node = xmlNewNode(NULL,BAD_CAST"Linear");
	// Duration
	if( iDuration > 0 ){
		xmlNewTextChild(linear_node, NULL, BAD_CAST "Duration", BAD_CAST duration);
	}
	// TrackingEvents
	/*
	xmlNodePtr trackingevents_node = xmlNewNode(NULL,BAD_CAST"TrackingEvents");
	xmlNodePtr tracking_node = xmlNewNode(NULL,BAD_CAST"Tracking");
	xmlNewProp( tracking_node, BAD_CAST"event",BAD_CAST"creativeView");
	xmlNodePtr tracking_content = xmlNewText(BAD_CAST"http://impurl.com/");
	xmlAddChild(tracking_node, tracking_content);
	xmlAddChild(trackingevents_node, tracking_node);
	xmlAddChild(linear_node, trackingevents_node);
	*/
	// video click
	xmlNodePtr videoclick_node = xmlNewNode(NULL,BAD_CAST"VideoClicks");
	xmlNodePtr clickthrough_node = xmlNewNode(NULL,BAD_CAST"ClickThrough");
	xmlNewProp( clickthrough_node, BAD_CAST"id",BAD_CAST"1");
	xmlNodePtr clickthrough_content = xmlNewText(BAD_CAST clickurl);
	xmlAddChild(clickthrough_node, clickthrough_content);
	xmlAddChild(videoclick_node, clickthrough_node);
	xmlAddChild(linear_node, videoclick_node);
	// media file
	xmlNodePtr media_node = xmlNewNode(NULL,BAD_CAST"MediaFiles");
	xmlNodePtr mediafile_node = xmlNewNode(NULL,BAD_CAST"MediaFile");
	
	xmlNewProp( mediafile_node, BAD_CAST"id",BAD_CAST"1");
	xmlNewProp( mediafile_node, BAD_CAST"delivery",BAD_CAST "progressive");
	xmlNewProp( mediafile_node, BAD_CAST"type",BAD_CAST type.c_str());
	xmlNewProp( mediafile_node, BAD_CAST"width",BAD_CAST pPara->m_ctResponse.m_sWidth.c_str());
	xmlNewProp( mediafile_node, BAD_CAST"height",BAD_CAST pPara->m_ctResponse.m_sHeight.c_str());
	
	xmlNodePtr mediafile_content = xmlNewText(BAD_CAST pPara->m_ctResponse.m_sHtml.c_str());
	xmlAddChild( mediafile_node, mediafile_content);
	xmlAddChild( media_node, mediafile_node);
	xmlAddChild( linear_node, media_node);

	xmlAddChild(creative_node, linear_node);
	xmlAddChild(creatives_node, creative_node);
	xmlAddChild(inline_node, creatives_node);
	// add impression to inline
	xmlAddChild(inline_node, imp_node);
	// add system_node to inline
	xmlAddChild(inline_node, system_node);
	// add line to ad
	xmlAddChild(ad_node,inline_node);
	// add ad to root
	xmlAddChild(root_node,ad_node);

	xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);	

	if(buffersize < iBuf){
		memcpy(sBuf, xmlbuff, buffersize);
	}
	else{
		xmlFree(xmlbuff);
		return AD_FAILURE;
	}
		
	xmlFree(xmlbuff);
	return AD_SUCCESS;
}

int AdProtocolAiQiYi::EmptyToHttp(AdAdapterPara * pPara)
{
    snprintf(pPara->m_ctCacheOut.Get(), pPara->m_ctCacheOut.Max(),AD_HTTP204, 0);
           pPara->m_ctCacheOut.Set(strlen(pPara->m_ctCacheOut.Get()));
	return AD_SUCCESS;   
}

int  SharedLibInit(AdProtocolBase *  & ctpPro)
{
	AdProtocolAiQiYi*  pPro = new AdProtocolAiQiYi;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}





