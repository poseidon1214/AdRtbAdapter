/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolBaidu.h"
#include "baidu_realtime_bidding.pb.h"
#include <string.h>
#include <stdlib.h>


typedef BesBidRequest		    BESREQ;
typedef BesBidRequest_BaiduId  BESREQ_USER;
typedef BesBidRequest_AdSlot	BESREQ_SLOT;
typedef BesBidResponse		    BESRES;
typedef BesBidResponse_Ad		BESRES_AD;

#define AD_BD_CLICK_MACRO	"%%CLICK_URL_0%%"
#define AD_BD_PRICE_MACRO  "%%PRICE%%"
#define AD_COOKIEMAPPING_BAIDU		"http://cm.pos.baidu.com/pixel?dspid=8086152&ext_data=geo"


static int sg_iIbiddingCf[] = {1, 1, 1, 1, 7, 2, 1, 1, 1};


int AdProtocolBaidu::HttpToInternal(AdAdapterPara * pPara)
{
	BESREQ CReq;
	BESREQ_SLOT* pAdSlot = NULL;
	BESREQ_USER* pBesUser = NULL;
	
	char* pBody = pPara->m_pBody;
	int iBody = pPara->m_iBody;
	int iAdz = 0;
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
			if (0 != CReq.is_ping())
			{
				return AD_FAILURE;
			}
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
		// userid
		for(iCount = 0;iCount < CReq.baidu_id_list_size();iCount++){
			pBesUser = const_cast<BESREQ_USER*>(&CReq.baidu_id_list(iCount));
			if ( pBesUser->has_baidu_user_id()){
				pPara->m_ctRequest.m_sUserID = pBesUser->baidu_user_id();
			}

		}
		// user tags
		if (CReq.user_category_size() > 0) {
			for( iCount = 0; iCount < CReq.user_category_size(); iCount++){
				snprintf(sVal, AD_BYTE512, "%ld", long(CReq.user_category(iCount)));
				pPara->m_ctRequest.m_ltCrowdCategory.push_back(sVal);
			}

		}
		if (true == CReq.has_detected_language()){
			// pass
		}
		// url
		if (true == CReq.has_url()){
			pPara->m_ctRequest.m_sUserUrl = CReq.url();
		}
		// site_category
		if (CReq.has_site_category()){
			snprintf(sVal, AD_BYTE512, "%d", CReq.site_category());
			pPara->m_ctRequest.m_ltSiteCategory.push_back(sVal);
		}
		// exclude
		if (CReq.excluded_product_category_size() > 0) {
			for(iCount = 0; iCount < CReq.excluded_product_category_size(); iCount ++) {
				snprintf(sVal, AD_BYTE512, "%d", CReq.excluded_product_category(iCount));
				pPara->m_ctRequest.m_ltCategory.push_back(sVal);
			}
		}
		for (iAdz = 0; iAdz < CReq.adslot_size(); iAdz ++){
			pAdSlot = const_cast<BESREQ_SLOT*>(&CReq.adslot(iAdz));
			if (pAdSlot->has_ad_block_key()){
				snprintf(sVal, AD_BYTE512, "%ld", pAdSlot->ad_block_key());
				pPara->m_ctRequest.m_sPId = sVal;
			}
			if (pAdSlot->has_adslot_type()){
				switch (pAdSlot->adslot_type())
				{
					case 0:
						pPara->m_ctRequest.m_sAdViewType = AD_PROTOCOL_VIEW_TYPE_FIXED;
						break;
					case 1:
						pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_POPUP;
						break;
					case 11:
						pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_WIRELESS_POPUP;
						break;
					case 12:
						pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_WIRELESS_OPENSCREEN;
						break;
					case 21:
						pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_VIDEOPATCH;
						break;
					case 22:
						pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_VIDEOPATCH;
						break;
					case 23:
						pPara->m_ctRequest.m_sAdViewType= AD_PROTOCOL_VIEW_TYPE_VIDEOPATCH;
						break;
					default:
						pPara->m_ctRequest.m_sAdViewType = AD_PROTOCOL_VIEW_TYPE_FIXED;
						break;

				}
				// size 
				if (pAdSlot->has_width() && pAdSlot->has_height()) {
					snprintf(sVal, AD_BYTE512, "%dx%d", pAdSlot->width(), pAdSlot->height());
					pPara->m_ctRequest.m_sSize = sVal;
				}

				// slot_visibility
				if (pAdSlot->has_slot_visibility()) {
					if(pAdSlot->slot_visibility() == 1){
						pPara->m_ctRequest.m_sAdViewScreen= "1";
					}
					else{
						pPara->m_ctRequest.m_sAdViewScreen= "2";
					}
				}
				// creative 1 pic   2 flash  7 video
				if (pAdSlot->creative_type_size() > 0){
					// pPara->m_ctRequest.m_ltCreate.push_back(sVal);
					int create_tup[10] = {0};
					for(iCount = 0; iCount < pAdSlot->creative_type_size(); iCount++)
					{
						if( pAdSlot->creative_type(iCount) < 10){
							create_tup[pAdSlot->creative_type(iCount)] = 1;
						}
					}
					if( 0 == create_tup[1] && 0 == create_tup[2] && 0 == create_tup[7]){
						// no exclude create
					}
					else if( 1 == create_tup[1] && 1 == create_tup[2] && 1 == create_tup[7]){
						// no exclude create
					}
					else if (1 == create_tup[1] && 1 == create_tup[2] && 0 == create_tup[7]){
						pPara->m_ctRequest.m_ltCreate.push_back("4");
					}
					else if( 1 == create_tup[1] && 0 == create_tup[2] && 1 == create_tup[7] ){
						pPara->m_ctRequest.m_ltCreate.push_back("3");
					}
					else if( 0 == create_tup[1] && 1 == create_tup[2] && 1 == create_tup[7]){
						pPara->m_ctRequest.m_ltCreate.push_back("2");
					}
					else if( 1 == create_tup[1] && 0 == create_tup[2] && 0 == create_tup[7]){
						pPara->m_ctRequest.m_ltCreate.push_back("3");
						pPara->m_ctRequest.m_ltCreate.push_back("4");
					}
					else if( 0 == create_tup[1] && 1 == create_tup[2] && 0 == create_tup[7]){
						pPara->m_ctRequest.m_ltCreate.push_back("4");
						pPara->m_ctRequest.m_ltCreate.push_back("2");
					}
					else if( 0 == create_tup[1] && 0 == create_tup[2] && 1 == create_tup[7]){
						pPara->m_ctRequest.m_ltCreate.push_back("2");
						pPara->m_ctRequest.m_ltCreate.push_back("3");
					}

					
				}
				// ban landing page
				if (pAdSlot->excluded_landing_page_url_size() > 0){
					
				}
				// 
				if (pAdSlot->has_minimum_cpm()){
					snprintf(sVal, AD_BYTE512, "%d", pAdSlot->minimum_cpm());
					pPara->m_ctRequest.m_sMinPrice = sVal;					
				}
				
			}
			break;
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

int AdProtocolBaidu::InternalToHttp(AdAdapterPara * pPara)
{
	std::string SBuffer;

	char sTemp[AD_KBYTES4];
	BESRES CRes;
	BESRES_AD* pCAds = NULL;
	
	pCAds = CRes.add_ad();
	CRes.set_id(pPara->m_ctRequest.m_sBidID);
	if (NULL != pCAds) 
	{
		pCAds->set_max_cpm(atoi(pPara->m_ctResponse.m_sMaxPrice.c_str()));
		pCAds->set_category(atoi(pPara->m_ctResponse.m_sCategory.c_str()));
		pCAds->set_landing_page(pPara->m_ctResponse.m_sClickUrl);
		pCAds->set_advertiser_id(atoi(pPara->m_ctResponse.m_sAdvertiserId.c_str()));
		if((UINT)atoi(pPara->m_ctResponse.m_sMaterailFormat.c_str()) < sizeof(sg_iIbiddingCf)) {
				pCAds->set_type(sg_iIbiddingCf[atoi(pPara->m_ctResponse.m_sMaterailFormat.c_str())]);
		}

		pCAds->set_creative_id(atol(pPara->m_ctResponse.m_sMaterialID.c_str()));
		pCAds->set_is_cookie_matching(true);
		std::vector<string> vStr; 
		AdString::StrSplit(pPara->m_ctResponse.m_sSize, vStr, "x");
		if(vStr.size() != 2){
			return AD_FAILURE;
		}
		pPara->m_ctResponse.m_sWidth += vStr[0].c_str();
		pPara->m_ctResponse.m_sHeight+= vStr[1].c_str();		
		pCAds->set_width(atoi(pPara->m_ctResponse.m_sWidth.c_str()));
		pCAds->set_height(atoi(pPara->m_ctResponse.m_sHeight.c_str()));		
		pCAds->set_sequence_id(1);

		string cmurl = AD_COOKIEMAPPING_BAIDU;
		string price_macro = AD_BD_PRICE_MACRO;
		string click_macro = AD_BD_CLICK_MACRO;	
		char sSnippet[AD_KBYTES4] = {0};
		if( NULL != strcasestr(pPara->m_ctResponse.m_sHtml.c_str(), "html") || \
                NULL != strcasestr(pPara->m_ctResponse.m_sHtml.c_str(), "ats.istreamsche.com")){
			string par = "p=";
			string price =  par + AD_BD_PRICE_MACRO;
			GetScriptHtml(pPara, price, click_macro, sSnippet, AD_KBYTES4);
			pCAds->add_target_url(pPara->m_ctResponse.m_sClickUrl);
		}
		else{
			string sClickPara;
			char sShowPara[AD_KBYTES2];
			char sAdClickUrl[AD_KBYTES2] = {0};	
			GetShowPara(pPara, sShowPara, AD_KBYTES2);
			GetClickPara(&(pPara->m_ctResponse), sClickPara);
			snprintf(sAdClickUrl, AD_KBYTES2,"http://lg.istreamsche.com/besclick?gc=%s&%s&url=%s", sClickPara.c_str(), sShowPara, pPara->m_ctResponse.m_sClickUrl.c_str());	
			GenerateSinppet(pPara, sSnippet, cmurl, price_macro, click_macro, AD_KBYTES4);	
			pCAds->add_target_url(sAdClickUrl);
		}
		pCAds->set_html_snippet(sSnippet);
		CRes.SerializeToString(&SBuffer);

		/* head */
		snprintf(sTemp, AD_KBYTES4, AD_HTTP200_OCT, CRes.ByteSize());

		int ret;
		ret = pPara->m_ctCacheOut.Write(sTemp ,strlen(sTemp));
		if(ret != AD_SUCCESS)
			return AD_FAILURE;

		ret = pPara->m_ctCacheOut.Append(SBuffer.c_str(), CRes.ByteSize());
		if(ret != AD_SUCCESS)
			return AD_FAILURE;
	}
	else 
	{
		return AD_FAILURE;
	}

	return AD_SUCCESS;
}

int AdProtocolBaidu::EmptyToHttp(AdAdapterPara * pPara)
{
	BESRES CRes;
	std::string STanx;
	char sHead[AD_KBYTES2] = { 0 };

	CRes.set_id(pPara->m_ctRequest.m_sBidID);
	CRes.SerializeToString(&STanx);

	/* head */
	snprintf(sHead, AD_KBYTES2, AD_HTTP200_OCT, CRes.ByteSize());

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
	AdProtocolBaidu *  pPro = new AdProtocolBaidu;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}
