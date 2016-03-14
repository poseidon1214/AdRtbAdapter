/********************************************************/
/*	Geo Polymerization Technology Co, Ltd		*/
/*	Project:	GtAdapter-1.0.0			*/
/*	Author:		zp			*/
/*	Date:		2016/1/26			*/
/*	File:		AdProtocolSohu.cpp			*/
/********************************************************/

#include "AdProtocolSohu.h"
#include "sohuRTB.pb.h"
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef sohuadx::Request		           ADATREQS_C;
typedef sohuadx::Request_Site              ADATREQSITE_C;
typedef sohuadx::Request_Device            ADATREQDEVICE_C;
typedef sohuadx::Request_User              ADATREQUSER_C;
typedef sohuadx::Request_Impression        ADATREQIMP_C;
typedef sohuadx::Request_Impression_Banner ADATREQIMPBANNER_C;
typedef sohuadx::Request_Impression_Video  ADATREQIMPVIDEO_C;
typedef sohuadx::Response                  ADATRES_C;
typedef sohuadx::Response_SeatBid          ADATRESSEATBID_C;
typedef sohuadx::Response_Bid              ADATRESBID_C;


#define GDT_STR_IMPID(pPara)     (pPara->m_ctRequest.m_sPara1)

void GtAdpGdtInfo(AdAdapterPara * pPara, char* pstBuf);

AdProtocolSohu::AdProtocolSohu()
{
}

int AdProtocolSohu::HttpToInternal(AdAdapterPara * pPara)
{
    ADATREQS_C      CReq;
    ADATREQSITE_C   *CSite;
    ADATREQDEVICE_C *CDevice;
    ADATREQUSER_C   *CUser;
    ADATREQIMP_C    *CImp;
    ADATREQIMPBANNER_C  *CImpBanner;
    ADATREQIMPVIDEO_C   *CImpVid;
    
	char* pBody = pPara->m_pBody;
	int iBody = pPara->m_iBody;
	int iCount = 0;
	char sVal[AD_BYTE512];
    string sStr;

	int iFlow = AD_PROTOCOL_FLOW_PC;

    if(true == CReq.ParseFromArray(pBody, iBody)){
		// required int32 version = 1;
		if (true == CReq.has_version()){
		}
        
        if(true == CReq.has_bidid()){
            pPara->m_ctRequest.m_sBidID = CReq.bidid();
        }
        
        if(true == CReq.has_istest()){
        }
        
        if(true == CReq.has_site()){
            CSite = const_cast<ADATREQSITE_C *>(&CReq.site());
            if(CSite->has_page()){
                pPara->m_ctRequest.m_sUserUrl = CSite->page();
            }
            if(CSite->has_category()){
                memset(sVal, 0, AD_BYTE512);
                snprintf(sVal, AD_BYTE512, "%ld", CSite->category());
                sStr = sVal;
                pPara->m_ctRequest.m_ltSiteCategory.push_back(sStr);
            }
            if(CSite->has_ref()){
            }
        }
        
        if(true == CReq.has_device()){
            CDevice = const_cast<ADATREQDEVICE_C *>(&CReq.device());
            if(CDevice->has_type()){
            }
            if(CDevice->has_ip()){
                pPara->m_ctRequest.m_sUserIP = CDevice->ip();
            }
            if(CDevice->has_ua()){
                pPara->m_ctRequest.m_sUserAgent = CDevice->ua();
            }
            if(CDevice->has_carrier()){
            }
            if(CDevice->has_nettype()){
                if(CDevice->nettype() == "2G"){
                    pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_2G);
                }else if(CDevice->nettype() == "3G"){
                    pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_3G);
                }else if(CDevice->nettype() == "WIFI"){
                    pPara->m_ctRequest.m_ltMobileConnect.push_back(AD_PROTOCOL_MOBILE_CON_WIFI);
                }
            }
            if (CDevice->has_type() && CDevice->type() == "Mobile" && CDevice->has_mobiletype()) {
                iFlow = AD_PROTOCOL_FLOW_MOBILE;
                if(CDevice->mobiletype() == "iPhone") {
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
                }else if(CDevice->mobiletype() == "iPad"){
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
                }else if(CDevice->mobiletype() == "Android Phone"){
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
                }else if(CDevice->mobiletype() == "Android Pad"){
                    pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
                }
            }
            if(CDevice->has_imei()){
                pPara->m_ctRequest.m_sIMEI = CDevice->imei();
            }

            if(CDevice->has_idfa()){
                pPara->m_ctRequest.m_sIDFA = CDevice->idfa();
            }

            if(CDevice->has_androidid()){
                pPara->m_ctRequest.m_sAndroidID = CDevice->androidid();
            }
        }

        if (CReq.has_user()) {
            CUser = const_cast<ADATREQUSER_C *>(&CReq.user());
            if (CUser->has_suid()) {
                pPara->m_ctRequest.m_sUserID = CUser->suid();
            }
        }
        if (CReq.impression_size() > 0) {
            for (iCount = 0; iCount < CReq.impression_size(); ++iCount) {
                CImp = const_cast<ADATREQIMP_C *>(&CReq.impression(iCount));
                if(CImp->has_idx()){
                }
                if(CImp->has_pid()){
                    pPara->m_ctRequest.m_sPId = CImp->pid();
                }

                if (CImp->has_bidfloor()) {
					snprintf(sVal, AD_BYTE512, "%d", CImp->bidfloor());
					pPara->m_ctRequest.m_sMinPrice = sVal;
                }
                
                if (CImp->has_banner()) {
                    CImpBanner = const_cast<ADATREQIMPBANNER_C *>(&CImp->banner());
                    if (CImpBanner->has_width() && CImpBanner->has_height()) {
                        memset(sVal, 0, AD_BYTE512);
                        sprintf(sVal, "%dx%d", CImpBanner->width(), CImpBanner->height());
                        pPara->m_ctRequest.m_sSize = sVal;
                    }
                }
                if (CImp->has_video()) {
                    CImpVid = const_cast<ADATREQIMPVIDEO_C *>(&CImp->video());
                    if(CImpVid->has_durationlimit()){
                        memset(sVal, 0, AD_BYTE512);
                        sprintf(sVal, "%d", CImpVid->durationlimit());
                        pPara->m_ctRequest.m_sVideoMaxTime = sVal;
                    }
                    if (CImpVid->has_width() && CImpVid->has_height()) {
                        memset(sVal, 0, AD_BYTE512);
                        sprintf(sVal, "%dx%d", CImpVid->width(), CImpVid->height());
                        pPara->m_ctRequest.m_sSize = sVal;
                    }
		            if(CImpVid->category_size() > 0){
						for( iCount = 0; iCount < CImpVid->category_size(); iCount++){
		                	pPara->m_ctRequest.m_ltSiteCategory.push_back(CImpVid->category(iCount));
						}
		            }					
                }
                if (CImp->has_screenlocation()) {
                    memset(sVal, 0, AD_BYTE512);
                    sprintf(sVal, "%d", CImp->screenlocation());
                    pPara->m_ctRequest.m_sAdViewScreen = sVal;
                }
                if (CImp->has_ispreferreddeals()){
                    memset(sVal, 0, AD_BYTE512);
                    sprintf(sVal, "%d", CImp->ispreferreddeals());
                    pPara->m_ctRequest.m_sTransaction = sVal;
                }
            }
        }
		snprintf(sVal, AD_BYTE512, "%d", iFlow);
		pPara->m_ctRequest.m_sFlow = sVal;
    }
    CheckUserId(pPara);
	return AD_SUCCESS;
}

int AdProtocolSohu::InternalToHttp(AdAdapterPara * pPara)
{
    std::string sSohu;
    ADATRES_C        CRes;
    ADATRESSEATBID_C *pCSeat = NULL;
    ADATRESBID_C     *pCBid = NULL;
    char szVal[AD_KBYTES2] = { 0 };
    char sTemp[AD_KBYTES4] = {0};
    
    pCSeat = CRes.add_seatbid();
    pCBid = pCSeat->add_bid();

    if(pCSeat && pCBid){
        CRes.set_version(atoi(pPara->m_sVer.c_str()));
        CRes.set_bidid(pPara->m_ctResponse.m_sBidID);
        pCSeat->set_idx(0);
        pCBid->set_price(atoi(pPara->m_ctResponse.m_sMaxPrice.c_str()));
        pCBid->set_adurl(pPara->m_ctResponse.m_sHtml.c_str());
        GetShowInfo(pPara, szVal, AD_KBYTES2);
        pCBid->set_ext(szVal);
        //    pCBid.set_displaypara(const char * value);
        pCBid->set_clickpara(pPara->m_ctResponse.m_sClickUrl.c_str());

        CRes.SerializeToString(&sSohu);
		/* head */
		snprintf(sTemp, AD_KBYTES4, AD_HTTP200, CRes.ByteSize());

		int ret;
		ret = pPara->m_ctCacheOut.Write(sTemp ,strlen(sTemp));
		if(ret != AD_SUCCESS)
			return AD_FAILURE;

		ret = pPara->m_ctCacheOut.Append(sSohu.c_str(), CRes.ByteSize());
		if(ret != AD_SUCCESS)
			return AD_FAILURE;	
    }
    
    return AD_SUCCESS;
}

int AdProtocolSohu::EmptyToHttp(AdAdapterPara * pPara)
{
    std::string sSohu;
    ADATRES_C        CRes;
    char sTemp[AD_KBYTES4] = {0};
    
    CRes.set_bidid( pPara->m_ctRequest.m_sBidID);
    CRes.set_version(atoi(pPara->m_sVer.c_str()));

    CRes.SerializeToString(&sSohu);
	/* head */
	snprintf(sTemp, AD_KBYTES4, AD_HTTP200, CRes.ByteSize());

	int ret;
	ret = pPara->m_ctCacheOut.Write(sTemp ,strlen(sTemp));
	if(ret != AD_SUCCESS)
		return AD_FAILURE;

	ret = pPara->m_ctCacheOut.Append(sSohu.c_str(), CRes.ByteSize());
	if(ret != AD_SUCCESS)
		return AD_FAILURE;	
	return AD_SUCCESS;
}

int  SharedLibInit(AdProtocolBase *  & ctpPro)
{
	AdProtocolSohu *  pPro = new AdProtocolSohu;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}

