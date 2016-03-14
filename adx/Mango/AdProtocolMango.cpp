/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolMango.h"
#include "json/json.h"
#include <string.h>
#include <stdlib.h>

#define AD_MANGO_PRICE 	"p=#WIN_PRICE#"
#define AD_MANGO_FEED  "http://amlog.istreamsche.com/amshow?%s&tp=2&info=%s"
#define AD_MANGO_CLICK "http://amlog.istreamsche.com/amclick?tp=2&info=%s&url=%s"
#define AD_MANGO_RETURN	"%s<a href=\"#CLICK_URL#\" target=\"_blank\"><img src=\"%s\" width=\"%s\" height=\"%s\"/></a>#IMP_TRACK#"


#define IM_STR_IMPID   (pPara->m_ctRequest.m_sPara1)
#define DV_STR_SW   (pPara->m_ctRequest.m_sPara2)
#define DV_STR_SH   (pPara->m_ctRequest.m_sPara3)
#define DV_STR_INSTL (pPara->m_ctRequest.m_sPara4)


int AdProtocolMango::HttpToInternal(AdAdapterPara * pPara)
{
	int iCount = 0;
	int iFlowtype = AD_PROTOCOL_FLOW_MOBILE;
	Json::Reader reader;
	Json::Value value;
	Json::Value subvalue;
	Json::Value grandvalue;
	Json::Value ggrandvalue;
	int iReturn = AD_SUCCESS;
	char szVal[AD_KBYTES1] = { 0 };
	string sValue;
	char sVal[AD_BYTE512] = {0};

	int isPad = 0;
	
	if (true == reader.parse(pPara->m_pBody, value)) 
	{
		if (true == value.isMember("id")) 
		{
			pPara->m_ctRequest.m_sBidID = value["id"].asString();
		}
		// imp
		if( value.isMember("imp"))
		{
			if( value["imp"].size() > 0)
			{
				for(iCount = 0; (UINT)iCount < value["imp"].size(); iCount++)
				{
					subvalue = value["imp"][iCount];
					if( subvalue.isMember("impid"))
					{
						IM_STR_IMPID = subvalue["impid"].asString();
					}
					if( subvalue.isMember("bidfloor"))
					{
						snprintf(sVal, AD_BYTE512, "%d", subvalue["bidfloor"].asInt()/100);
						pPara->m_ctRequest.m_sMinPrice = sVal;
					}
					if(subvalue.isMember("w") && subvalue.isMember("h")) 
					{
						snprintf(szVal, AD_BYTE512, "%dx%d", subvalue["w"].asInt(), subvalue["h"].asInt());
						pPara->m_ctRequest.m_sSize = szVal;              
					}		
					if(subvalue.isMember("pos"))
					{}
					if(subvalue.isMember("btype"))
					{}
					if(subvalue.isMember("battr"))
					{}	
					if(subvalue.isMember("instl"))
					{
						snprintf(szVal, AD_BYTE512, "%d", subvalue["instl"].asInt());
						DV_STR_INSTL = szVal;
					}
					break;
				}
			}
			
		}
		// app
		if(value.isMember("app"))
		{
			subvalue = value["app"];
			if( subvalue.isMember("aid"))
			{
				pPara->m_ctRequest.m_sPId = subvalue["aid"].asString();
			}
			if( subvalue.isMember("cat"))
			{
				for(iCount = 0; (UINT)iCount < subvalue["cat"].size(); iCount++)
				{
					pPara->m_ctRequest.m_ltSiteCategory.push_back(subvalue["cat"][iCount].asString());
				}
			}
			if( subvalue.isMember("pid"))
			{}

		}

		// devie
		if(value.isMember("device"))
		{
			subvalue = value["device"];
			if( subvalue.isMember("did"))
			{
				pPara->m_ctRequest.m_sIMEI = subvalue["did"].asString();
			}
			if( subvalue.isMember("dpid"))
			{
				pPara->m_ctRequest.m_sIDFA= subvalue["dpid"].asString();
				pPara->m_ctRequest.m_sAndroidID= subvalue["dpid"].asString();
			}
			if( subvalue.isMember("ua"))
			{
				//pPara->m_ctRequest.m_sUserAgent = subvalue["ua"].asString();
				//AdUaParser(pPara);
			}
			if(true == subvalue.isMember("ip")) 
			{
				pPara->m_ctRequest.m_sUserIP = subvalue["ip"].asString();
			}

			if( subvalue.isMember("carrier"))
			{
				pPara->m_ctRequest.m_ltMobileOperator.push_back(subvalue["carrier"].asString());
			}
			if(true == subvalue.isMember("model")) 
			{
				if(strcasestr(subvalue["model"].asString().c_str(), "ipad")  != NULL)
				{
					isPad = 1;
				}
			}			
			if( subvalue.isMember("os"))
			{
				if(strcasestr(subvalue["os"].asString().c_str(), "iphone")  != NULL)
				{
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
				}
				if(strcasestr(subvalue["os"].asString().c_str(), "android")  != NULL)
				{
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
				}
				
			}
			if( subvalue.isMember("connectiontype"))
			{
				snprintf( sVal, AD_BYTE512, "%d", subvalue["connectiontype"].asInt());
				sValue = sVal;
				pPara->m_ctRequest.m_ltMobileConnect.push_back(sValue);
			}
			if( subvalue.isMember("devicetype"))
			{
				snprintf( sVal, AD_BYTE512, "%d", subvalue["devicetype"].asInt());
				sValue = sVal;
				pPara->m_ctRequest.m_ltMobileDevice.push_back(sValue);
			}
			if( subvalue.isMember("loc"))
			{
				std::vector<string> vStr; 
				AdString::StrSplit(subvalue["loc"].asString(), vStr, ",");
				if(vStr.size() == 2){
					pPara->m_ctRequest.m_sLat = vStr[0].c_str();
					pPara->m_ctRequest.m_sLon = vStr[1].c_str();
				}					
			}
			if( subvalue.isMember("sw"))
			{
				snprintf( sVal, AD_BYTE512, "%d", subvalue["sw"].asInt());
				DV_STR_SW = sVal;
			}
			if( subvalue.isMember("sh"))
			{
				snprintf( sVal, AD_BYTE512, "%d", subvalue["sh"].asInt());
				DV_STR_SH = sVal;
			}
			if( subvalue.isMember("orientation"))
			{
				if( DV_STR_INSTL == "1")
				{
					if( subvalue["orientation"].asInt() == 2)// heng ping
					{
						if(isPad == 1){
							if(atoi(DV_STR_SH.c_str()) >= 1024)
							{
								pPara->m_ctRequest.m_sSize = "1024x768";
							}
							else{
								pPara->m_ctRequest.m_sSize = "750x560";
							}
						}
						else{
							if(atoi(DV_STR_SH.c_str()) >= 1136)
							{
								pPara->m_ctRequest.m_sSize = "1136x640";
							}
							else{
								pPara->m_ctRequest.m_sSize = "700x480";
							}
						}
						//snprintf(szVal, AD_BYTE512, "%sx%s", DV_STR_SH.c_str(), DV_STR_SW.c_str());
						//pPara->m_ctRequest.m_sSize = szVal;
					}
					else{
						if(isPad == 1){
							if(atoi(DV_STR_SH.c_str()) >= 1024)
							{
								pPara->m_ctRequest.m_sSize = "768x1024";
							}
							else{
								pPara->m_ctRequest.m_sSize = "560x750";
							}
						}						
						else{
							if(atoi(DV_STR_SH.c_str()) >= 1136)
							{
								pPara->m_ctRequest.m_sSize = "640x1136";
							}
							else{
								pPara->m_ctRequest.m_sSize = "480x700";
							}
						}						
						//snprintf(szVal, AD_BYTE512, "%sx%s", DV_STR_SW.c_str(), DV_STR_SH.c_str());
						//pPara->m_ctRequest.m_sSize = szVal; 
					}
				}
				
			}


		}
		// bcat
		if(value.isMember("bcat"))
		{
			for( iCount = 0; (UINT)iCount < value["bcat"].size(); iCount++)
			{
				pPara->m_ctRequest.m_ltCategory.push_back(value["bcat"][iCount].asString());
			}

		}

		sprintf(szVal, "%d",  iFlowtype);
		sValue= szVal;
		pPara->m_ctRequest.m_sFlow = sValue;
	}
	else 
	{
		iReturn = AD_FAILURE;
	}
	CheckUserId(pPara);
	return iReturn;
}

int AdProtocolMango::InternalToHttp(AdAdapterPara * pPara)
{
	int iLength = 0;
	
	Json::Value root;
	Json::Value seatbid;
	Json::Value bid;
	Json::Value cturl;
	string reBuffer;
	string sMonitorUrl;
	Json::FastWriter writer;
	char sTemp[AD_KBYTES2] = {0};
	char sShowInfo[AD_KBYTES2] = {0};
  	char szHead[AD_KBYTES2] = { 0 };       
	

	root["id"] = pPara->m_ctRequest.m_sBidID;
	root["bidid"] = "";
	
	bid["impid"] = IM_STR_IMPID;
	bid["price"] = atoi(pPara->m_ctResponse.m_sMaxPrice.c_str())*100;
	bid["adid"] = pPara->m_ctResponse.m_sMaterialID;
	bid["crid"] = pPara->m_ctResponse.m_sMaterialID;
	bid["cid"] = pPara->m_ctResponse.m_sExecID;
	// imp url win notice
	GetShowInfo(pPara,	sShowInfo, AD_KBYTES2);
	snprintf(sTemp, AD_KBYTES2, AD_MANGO_FEED, AD_MANGO_PRICE, sShowInfo);
	bid["nurl"] = sTemp;
	// click url
	snprintf(sTemp, AD_KBYTES2, AD_MANGO_CLICK, sShowInfo, pPara->m_ctResponse.m_sClickUrl.c_str());
	cturl = sTemp;
	bid["cturl"].append(cturl);
	bid["curl"] = pPara->m_ctResponse.m_sDestUrl;
	bid["ctype"] = 3;

	// adomain
	if (pPara->m_ctResponse.m_sDestUrl.length() > 0)
	{
		vector<string> vStr;
		vector<string>::iterator vIter;
		AdPageUrlParse(pPara->m_ctResponse.m_sDestUrl, vStr);
		for(vIter = vStr.begin(); vIter != vStr.end(); vIter ++)
		{
			bid["adomain"] = (*vIter);
		}
	}	
	//bid["attr"].append(pPara->m_ctResponse.m_sCategory);

	// size 
	std::vector<string> vStr; 
	AdString::StrSplit(pPara->m_ctResponse.m_sSize, vStr, "x");
	if(vStr.size() != 2){
		return AD_FAILURE;
	}
	//pPara->m_ctResponse.m_sWidth += vStr[0].c_str();
	//pPara->m_ctResponse.m_sHeight+= vStr[1].c_str();
	bid["adw"] = atoi(vStr[0].c_str());
	bid["adh"] = atoi(vStr[1].c_str());
	// adm
	if( !pPara->m_ctResponse.m_ltMonitorUrl.empty())
	{
		sMonitorUrl = *(pPara->m_ctResponse.m_ltMonitorUrl.begin());
	}	
	//if( atoi(DV_STR_INSTL.c_str()) == 1)
	//{
		string strtemp = "100%";
		snprintf(sTemp, AD_KBYTES2, AD_MANGO_RETURN, sMonitorUrl.c_str(),\
			pPara->m_ctResponse.m_sHtml.c_str(), strtemp.c_str(), strtemp.c_str());
	//}
	//else{
	//	string strtemp = "100%";
	//	snprintf(sTemp, AD_KBYTES2, AD_MANGO_RETURN, sMonitorUrl.c_str(),\
	//		pPara->m_ctResponse.m_sHtml.c_str(), strtemp.c_str(), strtemp.c_str());		
	//}
	bid["adm"] = sTemp;
	seatbid["seat"] = "1";
	seatbid["bid"].append(bid);
	root["seatbid"].append(seatbid);

	reBuffer = writer.write(root);
        //std::cout << reBuffer << std::endl;
	/* head */
	iLength = reBuffer.length();
	snprintf(szHead,  AD_KBYTES2, AD_HTTP200_JSON, iLength);

    int ret;
    ret = pPara->m_ctCacheOut.Write(szHead ,strlen(szHead));
    if(ret != AD_SUCCESS)
        return AD_FAILURE;

    ret = pPara->m_ctCacheOut.Append(const_cast<char *>(reBuffer.c_str()), iLength);
    if(ret != AD_SUCCESS)
        return AD_FAILURE;

	return AD_SUCCESS;
}

int AdProtocolMango::EmptyToHttp(AdAdapterPara * pPara)
{

	std::string sRes;
	char sHead[AD_KBYTES2] = { 0 };

	Json::FastWriter writer;
	Json::Value root;

	root["id"] = pPara->m_ctRequest.m_sBidID;
	root["nbr"] = 0;

	sRes = writer.write(root);

	int iLength = sRes.length();
	/* head */
	snprintf(sHead, AD_KBYTES2, AD_HTTP200_JSON, iLength);

	int ret;
	ret = pPara->m_ctCacheOut.Write(sHead ,strlen(sHead));
	if(ret != AD_SUCCESS)
		return AD_FAILURE;

	ret = pPara->m_ctCacheOut.Append(sRes.c_str(), iLength);
	if(ret != AD_SUCCESS)
		return AD_FAILURE;
	
	return AD_SUCCESS;

}

int  SharedLibInit(AdProtocolBase *  & ctpPro)
{
	AdProtocolMango *  pPro = new AdProtocolMango;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}

