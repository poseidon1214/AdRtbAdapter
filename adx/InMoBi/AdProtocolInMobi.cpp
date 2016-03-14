/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolInMobi.h"
#include "json/json.h"
#include <string.h>
#include <stdlib.h>

#define AD_IM_PRICE 	"p=${AUCTION_PRICE}"
#define AD_IM_PRICE_MACRO "${AUCTION_PRICE}"
#define AD_RESPONSE_SEATID	"fffdba729a194ab2a8cdcb37572df33e"
#define AD_IM_AT_SNIP	"<iframe width=\"%d\" height=\"%d\" frameborder=\"0\" scrolling=\"no\" src=\"%s?p=${AUCTION_PRICE}&pid=%s&executeid=%s&bid=%s&info=%s\"></iframe>"


#define IM_STR_IMPID   (pPara->m_ctRequest.m_sPara1)

int AdProtocolInMobi::HttpToInternal(AdAdapterPara * pPara)
{
	int iCount = 0;
    int usdFlag = 0;
	int cnyFlag = 0;	
	int iFloorPrice = 0;
	int iFlowtype = AD_PROTOCOL_FLOW_MOBILE;
	Json::Reader reader;
	Json::Value value;
	Json::Value subvalue;
	Json::Value grandvalue;
	Json::Value ggrandvalue;
	int iReturn = AD_SUCCESS;
	char szVal[AD_KBYTES1] = { 0 };
	string sValue;
	double usdTcny = 6.37;
	
	if (true == reader.parse(pPara->m_pBody, value)) 
	{
		if (true == value.isMember("id")) 
		{
			pPara->m_ctRequest.m_sBidID = value["id"].asString();
		}
		// currency
		if (true == value.isMember("cur"))
		{
			for(iCount = 0; iCount < (int)value["cur"].size();iCount++){
	        	if(AD_SUCCESS == strncasecmp((const char *)(UCHAR *)value["cur"][(unsigned int)iCount].asString().c_str(), "USD", 3)){
	            	usdFlag = 1; 
	            }
	            if(AD_SUCCESS == strncasecmp((const char *)(UCHAR *)value["cur"][(unsigned int)iCount].asString().c_str(), "CNY", 3)){
	            	cnyFlag = 1;
	            }	
			}
		}

		// ban landing page
		if ( true == value.isMember("badv"))
		{
			for(iCount = 0; iCount < (int)value["badv"].size(); iCount++){
				pPara->m_ctRequest.m_ltCategory.push_back(value["badv"][iCount].asString());
			}
		}
		// app
		if ( true == value.isMember("app"))
		{
			subvalue = value["app"];
			if (subvalue.isMember("cat")){
				for (iCount = 0; iCount < (int)subvalue["cat"].size(); ++iCount){
					unsigned int i;
					char stTmp[32] = {0};
					snprintf(stTmp, 32, "%s",subvalue["cat"][(unsigned int)iCount].asString().c_str());
					for( i = 0; i < strlen(stTmp); i++){
						if( stTmp[i] == '-'){
							stTmp[i] = '\0';
						}
					}
					string stCat = stTmp;
					pPara->m_ctRequest.m_ltSiteCategory.push_back(stCat);
				}
			}
		}


		// required object imp = 6;
		if (true == value.isMember("imp")) 
		{
			for(iCount = 0; iCount < (int)value["imp"].size(); iCount ++)
			{
				subvalue = value["imp"][(unsigned int)iCount];
				// required string imp id = 1;      
				if(true == subvalue.isMember("id")) 
				{
					IM_STR_IMPID = subvalue["id"].asString();
				}
				
				// required object imp banner = 4;
				if(true == subvalue.isMember("banner")) 
				{
					grandvalue = subvalue["banner"];
					// required int imp banner w = 1,h = 2;
					if(grandvalue.isMember("w") && grandvalue.isMember("h")) 
					{
						snprintf(szVal, AD_BYTE512, "%dx%d", grandvalue["w"].asInt(), grandvalue["h"].asInt());
						sValue= szVal;
						pPara->m_ctRequest.m_sSize = sValue;              
					}
					if (grandvalue.isMember("id"))
					{
						pPara->m_ctRequest.m_sPId = grandvalue["id"].asString();
					}
					// ban type   1:html text 2:html banner  3:js  4:iframe
					if (grandvalue.isMember("btype"))
					{
						for(iCount = 0; iCount < (int)grandvalue["btype"].size(); iCount++){
							if( grandvalue["btype"][iCount].asInt() == 2 ){
								// ban html
								return AD_FAILURE;
							}
							//snprintf(szVal, AD_BYTE512, "%d", grandvalue["btype"][iCount].asInt());
							//pPara->m_ctRequest.m_ltCreate.push_back(szVal);	
						}
					}
				}
				
				// required float imp bidfloor = 3;
				if (true == subvalue.isMember("bidfloor")) 
				{
					if(usdFlag == 1){
						iFloorPrice = subvalue["bidfloor"].asDouble() * usdTcny * 100;
					}
					else if(cnyFlag == 1){
						iFloorPrice = subvalue["bidfloor"].asDouble() * 100;
					}
					else{
						iFloorPrice = subvalue["bidfloor"].asDouble() * usdTcny * 100;
					}
				}

				break;//first impression
			}   

		}

	
			
		if (true == value.isMember("device")) 
		{
			subvalue = value["device"];
			// required string device ua = 1;
			if(true == subvalue.isMember("ua"))
			{
				pPara->m_ctRequest.m_sUserAgent = subvalue["ua"].asString();
				AdUaParser(pPara);
			}
			// required string device ip = 2;
			if(true == subvalue.isMember("ip")) 
			{
				pPara->m_ctRequest.m_sUserIP = subvalue["ip"].asString();
			}
			// did imei
			if(true == subvalue.isMember("didsha1")) 
			{
				pPara->m_ctRequest.m_sIMEI = subvalue["didsha1"].asString();
			}
			// did imei
			if(true == subvalue.isMember("dpidsha1")) 
			{
				pPara->m_ctRequest.m_sAndroidID = subvalue["dpidsha1"].asString();
			}			
			// required string device ext = 14;
			if(true == subvalue.isMember("ext")) 
			{
				grandvalue = subvalue["ext"];
				// required string device ext idfa = 1;
				//if (true == grandvalue.isMember("idfasha1")) 
				//{
				//	pPInfo->m_sIdfaSHA= grandvalue["idfa"].asString();
				//}
				// required string device ext idfa = 1;
				if (true == grandvalue.isMember("idfamd5")) 
				{
					pPara->m_ctRequest.m_sIDFA= grandvalue["idfamd5"].asString();
				}				
			}			
			// required string device model = 9;
			if (true == subvalue.isMember("model"))
			{
				if(strcasestr(subvalue["model"].asString().c_str(), "iphone")  != NULL)
				{
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
				}
				if(strcasestr(subvalue["model"].asString().c_str(), "ipad")  != NULL)
				{
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
				}
				if(strcasestr(subvalue["model"].asString().c_str(), "android")  != NULL)
				{
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
				}
			}


			// required string device dpidmd5 = 10;
			if (true == subvalue.isMember("os")) 
			{
				if(strcasestr(subvalue["os"].asString().c_str(), "ios")  != NULL)
				{
					iFlowtype = AD_PROTOCOL_FLOW_MOBILE;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPHONE);
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_IPAD);
				}
				if(strcasestr(subvalue["os"].asString().c_str(), "android")  != NULL)
				{
					iFlowtype = AD_PROTOCOL_FLOW_MOBILE;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_ANDROID);
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_APAD);
				}
				if(strcasestr(subvalue["os"].asString().c_str(), "wp")  != NULL)
				{
					iFlowtype = AD_PROTOCOL_FLOW_MOBILE;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_WPHONE);
				}
			}

			// osv 
			if (true == subvalue.isMember("osv")) 
			{
			}

			// required string device carrier = 12;
			if (true == subvalue.isMember("carrier")) 
			{
				pPara->m_ctRequest.m_ltMobileOperator.push_back(subvalue["carrier"].asString());
			}

			// required int device connectiontype = 13;
			if (true == subvalue.isMember("connectiontype")) 
			{
				switch(subvalue["connectiontype"].asInt())
				{
					case 2:
						sValue =AD_PROTOCOL_MOBILE_CON_WIFI;
						break;
					case 3:
						sValue = AD_PROTOCOL_MOBILE_CON_NOWIFI;
						break;
					case 4:
						sValue = AD_PROTOCOL_MOBILE_CON_2G;
						break;
					case 5:
						sValue = AD_PROTOCOL_MOBILE_CON_3G;
						break;
					case 6:
						sValue = AD_PROTOCOL_MOBILE_CON_4G;
						break;
					default:
						sValue.clear();
						break;
				}
				if(!sValue.empty())
				{
					pPara->m_ctRequest.m_ltMobileConnect.push_back(sValue);
				}				
			}


			// required object device geo = 15;
			if(true == subvalue.isMember("geo")) 
			{
				grandvalue = subvalue["geo"];
				if(grandvalue.isMember("lat"))
				{
					snprintf(szVal, AD_BYTE512, "%.2f", grandvalue["lat"].asDouble());
					pPara->m_ctRequest.m_sLat = szVal;
				}
				if(grandvalue.isMember("lon"))
				{
					snprintf(szVal, AD_BYTE512, "%.2f", grandvalue["lon"].asDouble());
					pPara->m_ctRequest.m_sLon = szVal;
				}				
			}
		}

		// required object user = 5;
		if (true == value.isMember("user")) 
		{
			subvalue = value["user"];
			// required string user id = 1;
			if(true == subvalue.isMember("id"))
			{
				pPara->m_ctRequest.m_sUserID= subvalue["id"].asString();
			}
		}



		sprintf(szVal, "%d",  iFloorPrice);
		sValue= szVal;
		pPara->m_ctRequest.m_sMinPrice = sValue;
		
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

int AdProtocolInMobi::InternalToHttp(AdAdapterPara * pPara)
{
	int iLength = 0;
	
	Json::Value root;
	Json::Value seat;
	Json::Value bid;
	string reBuffer;
	Json::FastWriter writer;
	
  	char szHead[AD_KBYTES2] = { 0 };       
	
	// adomain
	if (pPara->m_ctResponse.m_sDestUrl.length() > 0)
	{
		vector<string> vStr;
		vector<string>::iterator vIter;
		AdPageUrlParse(pPara->m_ctResponse.m_sDestUrl, vStr);
		for(vIter = vStr.begin(); vIter != vStr.end(); vIter ++)
		{
			bid["adomain"].append(*vIter);
		}
	}
	// crid
	bid["crid"] = pPara->m_ctResponse.m_sCreativeID;
	// iurl
	bid["iurl"] = pPara->m_ctResponse.m_sHtml;
	// price
	bid["price"] = atof(pPara->m_ctResponse.m_sMaxPrice.c_str()) / 100;
	// impid
	bid["impid"] = IM_STR_IMPID;
	// adm
	if( NULL != strcasestr(pPara->m_ctResponse.m_sHtml.c_str(), "html") ){
		char sHtml[AD_KBYTES4] = { 0 };
		string m_sExtStr = AD_IM_PRICE;
		string m_sImgUrl = "http://lg.istreamsche.com/1x1.gif?src=inmobi";
		int ret = GetShowHtml(pPara,  m_sExtStr, m_sImgUrl, sHtml, AD_KBYTES4);
		if(ret != AD_SUCCESS)
			return AD_FAILURE;
		bid["adm"] = sHtml;
	}
	else{
		string cmurl = "";
		string click_macro = "";
		string price_macro = AD_IM_PRICE_MACRO;
		char sSnippet[AD_KBYTES4] = {0};
		GenerateSinppet(pPara, sSnippet, cmurl, price_macro, click_macro, AD_KBYTES4);
		bid["adm"] = sSnippet;
	}
	// adid
	bid["adid"] = pPara->m_ctResponse.m_sMaterialID;
	// id
	bid["id"] = "1";
	// dealid

	root["cur"] = "CNY";
	root["id"] = pPara->m_ctRequest.m_sBidID;
	seat["bid"].append(bid);
	seat["seat"] = AD_RESPONSE_SEATID;
	root["seatbid"].append(seat);
	reBuffer = writer.write(root);

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

int AdProtocolInMobi::EmptyToHttp(AdAdapterPara * pPara)
{
    snprintf(pPara->m_ctCacheOut.Get(), pPara->m_ctCacheOut.Max(),AD_HTTP204, 0);
           pPara->m_ctCacheOut.Set(strlen(pPara->m_ctCacheOut.Get()));

	return AD_SUCCESS;
}

int  SharedLibInit(AdProtocolBase *  & ctpPro)
{
	AdProtocolInMobi *  pPro = new AdProtocolInMobi;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}

