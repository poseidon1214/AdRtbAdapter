/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolTenc.h"
#include "tenc-bidding.pb.h"
#include <string.h>
#include <stdlib.h>

using namespace TENC;

typedef TENC::Request REQ;
typedef TENC::Request_Impression REQ_IMP;
typedef TENC::Request_Impression_Banner REQ_BANNER;
typedef TENC::Request_Impression_Video REQ_VIDEO;
typedef TENC::Request_Site REQ_SITE;
typedef TENC::Request_Device REQ_DEV;
typedef TENC::Request_User REQ_USER;

typedef TENC::Response RES;
typedef TENC::Response_Bid RES_BID;
typedef TENC::Response_SeatBid RES_SEAT;


#define AD_BD_CLICK_MACRO	"%%CLICK_URL_0%%"
#define AD_BD_PRICE_MACRO  "%%PRICE%%"


int AdProtocolTenc::HttpToInternal(AdAdapterPara * pPara)
{
	REQ CReq;
	REQ_IMP* pAdImp = NULL;
	REQ_BANNER* pAdBanner = NULL;
	REQ_VIDEO* pAdVideo = NULL;
	REQ_SITE* pAdSite = NULL;
	REQ_DEV* pAdDev = NULL;
	REQ_USER* pAdUser = NULL;
	
	char* pBody = pPara->m_pBody;
	int iBody = pPara->m_iBody;
	char sVal[AD_BYTE512];

	int iFlow = AD_PROTOCOL_FLOW_PC;
		
	/* tenc */
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

		// site
		if(CReq.has_site())
		{
			pAdSite = const_cast<REQ_SITE*>(&CReq.site());
			if(pAdSite->has_page())
			{
				pPara->m_ctRequest.m_sUserUrl = pAdSite->page();
			}
		}

		// device
		if(CReq.has_device())
		{
			pAdDev = const_cast<REQ_DEV*>(&CReq.device());
			if(pAdDev->has_ua())
			{
				pPara->m_ctRequest.m_sUserAgent = pAdDev->ua();
				AdUaParser(pPara);
			}
			if(pAdDev->has_ip())
			{
				pPara->m_ctRequest.m_sUserIP = pAdDev->ip();
			}			
		}		

		// user
		if(CReq.has_user())
		{
			pAdUser = const_cast<REQ_USER*>(&CReq.user());
			if(pAdUser->has_id())
			{
				pPara->m_ctRequest.m_sUserID = pAdUser->id();
			}
		}
		
		// imp
		if( 0 < CReq.impression_size())
		{
			pAdImp = const_cast<REQ_IMP*>(&CReq.impression(0));
			if(pAdImp->has_id())
			{
				pPara->m_sImpression = pAdImp->id();
			}			
			if(pAdImp->has_tagid())
			{
				pPara->m_ctRequest.m_sPId = pAdImp->tagid();
			}
			if(pAdImp->has_bidfloor())
			{
				snprintf(sVal, AD_BYTE512, "%d", int(pAdImp->bidfloor()));
				pPara->m_ctRequest.m_sMinPrice = sVal;

			}	
			if(pAdImp->has_banner())
			{
				pAdBanner = const_cast<REQ_BANNER*>(&pAdImp->banner());
				if (pAdBanner->has_width() && pAdBanner->has_height()) {
					snprintf(sVal, AD_BYTE512, "%dx%d", pAdBanner->width(), pAdBanner->height());
					pPara->m_ctRequest.m_sSize = sVal;
				}	
				if(pAdBanner->has_visibility())
				{
					if(pAdBanner->visibility() == 1){
						pPara->m_ctRequest.m_sAdViewScreen= "1";
					}
					else{
						pPara->m_ctRequest.m_sAdViewScreen= "2";
					}					
				}
			}
			if(pAdImp->has_video())
			{
				iFlow = AD_PROTOCOL_FLOW_VIDEO;
				pAdVideo= const_cast<REQ_VIDEO*>(&pAdImp->video());
				if (pAdVideo->has_width() && pAdVideo->has_height()) {
					snprintf(sVal, AD_BYTE512, "%dx%d", pAdVideo->width(), pAdVideo->height());
					pPara->m_ctRequest.m_sSize = sVal;
				}	
				// 
				if (true == pAdVideo->has_minduration()) 
				{
					sprintf(sVal, "%d",  pAdVideo->minduration());
					pPara->m_ctRequest.m_sVideoMinTime = sVal;
				}
				if (true == pAdVideo->has_maxduration()) 
				{
					sprintf(sVal, "%d",  pAdVideo->maxduration());
					pPara->m_ctRequest.m_sVideoMaxTime = sVal;
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

int AdProtocolTenc::InternalToHttp(AdAdapterPara * pPara)
{
	std::string SBuffer;

	char sTemp[AD_KBYTES4];
	RES CRes;
	RES_SEAT* pCSeat = NULL;
	RES_BID*  pCBid = NULL;
	
	pCSeat = CRes.add_seatbid();
	//pCBid = CRes.add_bid();
	CRes.set_id(pPara->m_ctRequest.m_sBidID);
	//CRes.set_bidid(pPara->m_ctRequest.m_sBidID);
	if (NULL != pCSeat && (NULL != (pCBid = pCSeat->add_bid())) ) 
	{
		pCBid->set_impid(pPara->m_sImpression);
		pCBid->set_adid(pPara->m_ctResponse.m_sMaterialID);
		pCBid->set_price(atof(pPara->m_ctResponse.m_sMaxPrice.c_str()));
		//pCBid->set_adm(pPara->m_ctResponse.m_sHtml);
		
		char sVal[AD_KBYTES2];
		GetShowInfo(pPara, sVal, AD_KBYTES2);
		pCBid->set_ext(sVal);
		
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

int AdProtocolTenc::EmptyToHttp(AdAdapterPara * pPara)
{
	RES CRes;
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
	AdProtocolTenc *  pPro = new AdProtocolTenc;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}
