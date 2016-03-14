/***************************************
*	Description: ssh madule, transfer protocol  send for adx
*	Author: wilson
*	Date: 2016/03/03
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <errno.h>
#include "AdSsp.h"
#include "AdParaList.h"
#include "json/json.h"

AdSsp::AdSsp()
{
}

AdSsp::~AdSsp()
{
}

int AdSsp::Init(AdRtbConfig & ctConfig)
{
	m_cPidData.Init(ctConfig);
	return m_cPidData.Update();
}

int AdSsp::Update(void)
{
	return m_cPidData.Update();
}

int AdSsp::EmptyToHttp(AdRtbPara & ctPara)
{

	string & sStatus = ctPara.m_cHttp.GetStatus();
	if(!sStatus.empty())
	{
		ctPara.m_ctCacheOut.Write((char *)sStatus.c_str(), sStatus.size());
	}
	else
	{
		if(ctPara.m_ctRequest.m_vImp.size() > 0)
		{
			ctPara.m_cHttp.GetResponse(HTTP_NO_CONTENT,NULL,ctPara.m_ctCacheOut);
		}
		else
		{
			ctPara.m_cHttp.GetResponse(HTTP_BAD_REQ,NULL,ctPara.m_ctCacheOut);
		}
	}

	return AD_SUCCESS;
}

int AdSsp::ToInternal(AdRtbPara & ctPara, string & sIp)
{
	int ret;

	map<string, string>::iterator iter; 

	string sPid;
	//http Para
	ret = ctPara.m_cHttp.GetPara("pid", sPid);
	if((ret != AD_SUCCESS) && ( sPid.empty()))
	{
		return AD_FAILURE;
	}

	//http to internal
	ret = ctPara.m_cHttp.GetPara("cb", ctPara.m_sJsonCb);
	if((ret != AD_SUCCESS) && (ctPara.m_sJsonCb.empty()))
	{
		return AD_FAILURE;
	}

	string sCo;
	//http to internal
	ret = ctPara.m_cHttp.GetHeader("Cookie", sCo);
	if(ret != AD_SUCCESS)
	{
		//return ret;
	}

	string sUA;
	ctPara.m_cHttp.GetHeader("User-Agent", sUA);

	AdPid cPidInfo;
	ret = m_cPidData.GetPid(sPid, cPidInfo);
	if(ret != AD_SUCCESS)
		return ret;

	AdRtbImp cImp;
	CHAR8 buff[AD_BYTE128];
	//init impresstion data
	ULONG  impid = ctPara.m_ctRequest.getTime();
	snprintf(buff, AD_BYTE128, "%ld", impid);
	cImp.m_sId = buff;
	cImp.m_sTagId = sPid;
	cImp.m_fBidFloor = cPidInfo.m_fBidFloor;

	if((cPidInfo.m_iWidth <= 0) && ( cPidInfo.m_iHeight <= 0))
	{
		return AD_FAILURE;
	}

	//init banner data
	cImp.m_cBanner.m_iWidth = cPidInfo.m_iWidth;
	cImp.m_cBanner.m_iHeight= cPidInfo.m_iHeight;
	cImp.m_cBanner.m_iPos = cPidInfo.m_iPos;

	ctPara.m_ctRequest.m_vImp.push_back(cImp);

	//init site data
	AdRtbSite & cSite = ctPara.m_ctRequest.m_cSite;

	cSite.m_sPage = cPidInfo.m_sPage;
	cSite.m_vCat = cPidInfo.m_vCat;
	cSite.m_cContent.m_sKeywords = cPidInfo.m_sKeywords;

	//init User data
	AdRtbUser & cUser = ctPara.m_ctRequest.m_cUser;

	cUser.m_sId = sCo;

	//init Device Data
	AdRtbDevice & cDevice = ctPara.m_ctRequest.m_cDevice;

	cDevice.m_sIp = sIp;
	cDevice.m_sUserAgent = sUA;

	ctPara.m_iMid = cPidInfo.m_iMid;
	
	return AD_SUCCESS;
}

int AdSsp::ToHttp(AdRtbPara & ctPara)
{
//	CHAR8  sBody[AD_KBYTES8];
	std::string sStr;
	Json::FastWriter writer;
	Json::Value cJson;

	AdRtbImp & cImp = ctPara.m_ctRequest.m_vImp[0];
	AdRtbBid & cBid = ctPara.m_ctResponse.m_vSeatBid[0].m_vBid[0];

	cJson["w"] = cImp.m_cBanner.m_iWidth;
	cJson["h"] = cImp.m_cBanner.m_iHeight;
	cJson["htmlData"] = cBid.m_sAdm.c_str();
	sStr = ctPara.m_sJsonCb + "(" + writer.write(cJson) + ")";

	
//	int iLen = snprintf(sBody, AD_KBYTES8, "%s({\"w\": \"%d\",\"h\": \"%d\",\"htmlData\": \"%s\"})",
//		ctPara.m_sJsonCb.c_str(),cImp.m_cBanner.m_iWidth, cImp.m_cBanner.m_iHeight, );

	ctPara.m_cHttp.GetResponse(HTTP_OK, "application/x-javascript",sStr.c_str(),sStr.size(),ctPara.m_ctCacheOut);

	return ctPara.m_ctCacheOut.Size();
}
