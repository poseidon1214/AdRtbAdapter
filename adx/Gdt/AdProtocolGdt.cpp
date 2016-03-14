/********************************************************/
/*	Geo Polymerization Technology Co, Ltd		*/
/*	Project:	GtAdapter-1.0.0			*/
/*	Author:		zp			*/
/*	Date:		2015_12_10			*/
/*	File:		GtAdpGdt.cpp			*/
/********************************************************/

#include "AdProtocolGdt.h"
#include "gdt_rtb.pb.h"
#include <vector>
#include <algorithm>
#include <stdio.h>

typedef gdt::adx::GdtBidRequest                 ADATREQ_C;
typedef gdt::adx::GdtBidRequest_Impression      ADATIMP_C;
typedef gdt::adx::GdtBidRequest_Device          ADATDEV_C;
typedef gdt::adx::GdtBidRequest_Geo             ADATGEO_C;
typedef gdt::adx::GdtBidRequest_App             ADATAPP_C;
typedef gdt::adx::GdtBidRequest_User            ADATUSR_C;

typedef gdt::adx::GdtBidResponse                ADATRSP_C;
typedef gdt::adx::GdtBidResponse_SeatBid        ADATSBI_C;
typedef gdt::adx::GdtBidResponse_Bid            ADATBID_C;

typedef gdt::adx::GdtBidRequest_User_Demographic AdATUSERDEMO_C;
typedef gdt::adx::GdtBidRequest_User_Vertical   ADATUSERVERT_C;

#define	GT_AT_GDT_INFO		"ai=%s&ei=%s&ci=%s&ri=%s&adx=%d&bp=%s&pi=%s&os=%s&ia=%s"

#define GDT_STR_IMPID(pPara)     (pPara->m_ctRequest.m_sPara1)

void GtAdpGdtInfo(AdAdapterPara * pPara, char* pstBuf);

AdProtocolGdt::AdProtocolGdt()
{
    mtSize[133] = "582x166";
    mtPltf[133] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[134] = "114x114";
    mtPltf[134] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[79] = "640x960";
    mtPltf[79] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[80] = "320x480";
    mtPltf[80] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[119] = "480x720";
    mtPltf[119] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[120] = "640x1136";
    mtPltf[120] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[124] = "720x1280";
    mtPltf[124] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[125] = "1080x1920";
    mtPltf[125] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[142] = "750x1334";
    mtPltf[142] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[96] = "140x425";
    mtPltf[96] = AD_PROTOCOL_FLOW_PC;
    mtSize[99] = "200x162";
    mtPltf[99] = AD_PROTOCOL_FLOW_PC;
    mtSize[23] = "198x100";
    mtPltf[23] = AD_PROTOCOL_FLOW_PC;
    mtSize[12] = "198x40";
    mtPltf[12] = AD_PROTOCOL_FLOW_PC;
    mtSize[2] = "160x210";
    mtPltf[2] = AD_PROTOCOL_FLOW_PC;
    mtSize[207] = "640x1136";
    mtPltf[207] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[208] = "640x960";
    mtPltf[208] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[212] = "320x480";
    mtPltf[212] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[147] = "1200x800";
    mtPltf[147] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[148] = "1280x720";
    mtPltf[148] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[149] = "1200x627";
    mtPltf[149] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[150] = "800x1200";
    mtPltf[150] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[58] = "300x250";
    mtPltf[58] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[59] = "600x500";
    mtPltf[59] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[70] = "72x72";
    mtPltf[70] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[113] = "640x960";
    mtPltf[113] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[114] = "320x480";
    mtPltf[114] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[10] = "640x100";
    mtPltf[10] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[28] = "240x38";
    mtPltf[28] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[31] = "480x75";
    mtPltf[31] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[35] = "320x50";
    mtPltf[35] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[69] = "72x72";
    mtPltf[69] = AD_PROTOCOL_FLOW_MOBILE;
    mtSize[65] = "1000x560";
    mtPltf[65] = AD_PROTOCOL_FLOW_MOBILE;
}

int AdProtocolGdt::HttpToInternal(AdAdapterPara * pPara)
{
    ADATREQ_C   CReq;
//    ADATRSP_C   *CRSP;
    ADATIMP_C   *CImp;
//    ADATGEO_C   *CGeo;
//    ADATAPP_C   *CApp;
    ADATDEV_C   *CDev;
    ADATUSR_C   *CUsr;
    ADATGEO_C   *CGeo;
    ADATAPP_C   *CApp;
    AdATUSERDEMO_C *CUserDemo;
    ADATUSERVERT_C *CUserVert;

	char* pBody = pPara->m_pBody;
	int iBody = pPara->m_iBody;
	int iCount = 0;
	char sVal[AD_BYTE512];

	int iFlow = AD_PROTOCOL_FLOW_PC;

    if(true == CReq.ParseFromArray(pBody, iBody)){
		// required int32 version = 1;
		if (true == CReq.has_id())
		{
            pPara->m_ctRequest.m_sBidID = CReq.id();
		}
		else
		{
		}
	
		// required string bid = 2;
		if (true == CReq.has_is_ping()) {}
		if (true == CReq.has_is_test()) {}
		if (true == CReq.has_ip()) {
            pPara->m_ctRequest.m_sUserIP = CReq.ip();
        }
        
		if (true == CReq.has_area_code()) {
            snprintf(sVal, AD_BYTE512, "%d", CReq.area_code());
            pPara->m_ctRequest.m_sArea = sVal;
        }
        
        for(iCount = 0; iCount < CReq.impressions_size(); iCount++){
            CImp = const_cast<ADATIMP_C *>(&CReq.impressions(iCount));
		    if (true == CImp->has_id()) {
                GDT_STR_IMPID(pPara) = CImp->id();
            }
            
		    if (true == CImp->has_placement_id()) {
                snprintf(sVal, AD_BYTE512, "%ld", CImp->placement_id());
                pPara->m_ctRequest.m_sPId = sVal;
            }
            
            int iImcs = rand()%CImp->creative_specs_size();
            if(mtSize.find(CImp->creative_specs(iImcs)) != mtSize.end()){
                pPara->m_ctRequest.m_sSize = mtSize[CImp->creative_specs(iImcs)];
                iFlow = mtPltf[CImp->creative_specs(iImcs)];
            }else{
                iImcs = rand()%CImp->creative_specs_size();
                if(mtSize.find(CImp->creative_specs(iImcs)) != mtSize.end()){
                    pPara->m_ctRequest.m_sSize = mtSize[CImp->creative_specs(iImcs)];
                    iFlow = mtPltf[CImp->creative_specs(iImcs)];
                }
            }
            
            if(true == CImp->has_bid_floor()){
                snprintf(sVal, AD_BYTE512, "%d", CImp->bid_floor());
                pPara->m_ctRequest.m_sMinPrice = sVal;
            }
            for(int iBcount = 0;iBcount < CImp->blocking_industry_id_size(); iBcount++){}
        }
        if (true == CReq.has_device()) {
            CDev = const_cast<ADATDEV_C*>(&CReq.device());
            if(true == CDev->has_id()) {
                pPara->m_ctRequest.m_sIDFA = CDev->id();
            }
            if(true == CDev->has_device_type()) {
                if(CDev->device_type() == 1){
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_WPHONE);
                }
                if(CDev->device_type() == 2){
                    iFlow = AD_PROTOCOL_FLOW_MOBILE;
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
                }else if(CDev->device_type() == 3){
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
                }
            }
            if(true == CDev->has_os()) {
                if(CDev->os() == 1){ //IOS
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
                }else if(CDev->os() == 2){
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
                }else if(CDev->os() == 3){
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_WPHONE);
                }else if(CDev->os() == 4){

                }else if(CDev->os() == 5){
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
                }
            }
            if(true == CDev->has_os_version()) {}
            if(true == CDev->has_user_agent()) {
                pPara->m_ctRequest.m_sUserAgent = CDev->user_agent();
				AdUaParser(pPara);
            }
            if((true == CDev->has_screen_width()) && (true == CDev->has_screen_height())) {}
            if(true == CDev->has_dpi()) {}
            if(true == CDev->has_carrier()) {
                if(CDev->carrier() == 1){
    			    pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_YIDONG);
                }else if(CDev->carrier() == 2){
    			    pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_LIANTONG);
                }else if(CDev->carrier() == 3){
    			    pPara->m_ctRequest.m_ltMobileOperator.push_back(AD_PROTOCOL_MOBILE_OPERATE_DIANXIN);
                }
            }
            
            if(true == CDev->has_connection_type()) {
                if(CDev->connection_type() == 1){
					pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_WIFI);
                }else if(CDev->connection_type() == 2){
					pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_2G);
                }else if(CDev->connection_type() == 3){
					pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_3G);
                }else if(CDev->connection_type() == 4){
					pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_4G);
                }
            }
            
            if(true == CDev->has_brand_and_model()) {}
            if(true == CDev->has_language()) {}
        }
        if (true == CReq.has_user()) {
            CUsr = const_cast<ADATUSR_C*>(&CReq.user());
            if(true == CUsr->has_id()){
                pPara->m_ctRequest.m_sUserID = CUsr->id();
            }
            if(true == CUsr->has_user_demographic()) {
                CUserDemo = const_cast<AdATUSERDEMO_C*>(&CUsr->user_demographic());
                if(true == CUserDemo->has_gender()){}
                if(true == CUserDemo->has_education()){}
                if(true == CUserDemo->has_age_low()){}
                if(true == CUserDemo->has_age_high()){}
            }
            if(0 != CUsr->verticals_size()){
                for(iCount = 0; iCount < CUsr->verticals_size(); iCount++){
                    CUserVert = const_cast<ADATUSERVERT_C *>(&CUsr->verticals(iCount));
                    if(CUserVert->has_id()){}
                    if(CUserVert->has_weight()){}
                }
            }
        }
        if(true == CReq.has_geo()){
            CGeo = const_cast<ADATGEO_C *>(&CReq.geo());
            if(true == CGeo->has_latitude()){}
            if(true == CGeo->has_longitude()){}
            if(true == CGeo->has_accuracy()){}
        }
        if(true == CReq.has_app()){
            CApp = const_cast<ADATAPP_C *>(&CReq.app());
            if(CApp->has_industry_id()){
                char stTmp[AD_BYTE64] = {0};
                snprintf(stTmp, AD_BYTE64, "%ld", CApp->industry_id());
                string stCat = stTmp;
                pPara->m_ctRequest.m_ltSiteCategory.push_back(stCat); 
            }
            if(CApp->has_app_bundle_id()){}
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

int AdProtocolGdt::InternalToHttp(AdAdapterPara * pPara)
{
	std::string sGdt;

	char sTemp[AD_KBYTES4];
	char sShowInfo[AD_KBYTES4] = { 0 };
    ADATRSP_C   CRsp;
    ADATSBI_C   *CSbi;
    ADATBID_C   *CBid;
    
    AdRtbResponse* pRes = &pPara->m_ctResponse;

	int ret;
    if(NULL == (CSbi = CRsp.add_seat_bids()) || (NULL == (CBid = CSbi->add_bids()))){
        return AD_FAILURE;
    }
    GtAdpGdtInfo(pPara, sShowInfo);

    CBid->set_creative_id(pRes->m_sMaterialID);
    CBid->set_bid_price((atoi(pPara->m_ctResponse.m_sMaxPrice.c_str())));
    CBid->set_impression_param(sShowInfo);
    CBid->set_click_param(sShowInfo);
    CSbi->set_impression_id(GDT_STR_IMPID(pPara));
    CRsp.set_request_id(pRes->m_sBidID);

    CRsp.SerializeToString(&sGdt);
    snprintf(sTemp, AD_KBYTES4, AD_HTTP200, CRsp.ByteSize());
    ret = pPara->m_ctCacheOut.Write(sTemp ,strlen(sTemp));
    if(ret != AD_SUCCESS)
        return AD_FAILURE;

    ret = pPara->m_ctCacheOut.Append(sGdt.c_str(), CRsp.ByteSize());
    if(ret != AD_SUCCESS)
        return AD_FAILURE;
    return AD_SUCCESS;
}

int AdProtocolGdt::EmptyToHttp(AdAdapterPara * pPara)
{
    ADATRSP_C   CRsp;
    ADATSBI_C   *CSbi;
    ADATBID_C   *CBid;
	std::string SGdt;
	char sHead[AD_KBYTES2] = { 0 };
    if(NULL == (CSbi = CRsp.add_seat_bids()) || (NULL == (CBid = CSbi->add_bids()))){
        return AD_FAILURE;
    }

    CSbi->set_impression_id(GDT_STR_IMPID(pPara));
    CRsp.set_request_id(pPara->m_ctRequest.m_sBidID);
	CRsp.SerializeToString(&SGdt);

	/* head */
	snprintf(sHead, AD_KBYTES2, AD_HTTP200, CRsp.ByteSize());

	int ret;
	ret = pPara->m_ctCacheOut.Write(sHead ,strlen(sHead));
	if(ret != AD_SUCCESS)
		return AD_FAILURE;

	ret = pPara->m_ctCacheOut.Append(SGdt.c_str(), CRsp.ByteSize());
	if(ret != AD_SUCCESS)
		return AD_FAILURE;
	
	return AD_SUCCESS;
}

void GtAdpGdtInfo(AdAdapterPara * pPara, char* pstBuf)
{
    AdRtbRequest* pReq  = &pPara->m_ctRequest;
    AdRtbResponse* pRes = &pPara->m_ctResponse;
	sprintf(pstBuf, GT_AT_GDT_INFO, pRes->m_sAdvertiserId.c_str(), pRes->m_sExecID.c_str(), \
		pRes->m_sCreativeID.c_str(),pRes->m_sBidID.c_str(), \
		pRes->m_uiAdx, pRes->m_sMaxPrice.c_str(), pRes->m_sPId.c_str(),\
        pRes->m_sDevType.c_str() ,pReq->m_sIDFA.c_str());

    return;
}

int  SharedLibInit(AdProtocolBase *  & ctpPro)
{
	AdProtocolGdt *  pPro = new AdProtocolGdt;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}

