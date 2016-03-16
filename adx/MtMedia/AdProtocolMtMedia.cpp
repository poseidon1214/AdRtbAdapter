/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolMtMedia.h"
#include "json/json.h"
#include <string.h>
#include <stdlib.h>

#define AD_URL        const_cast<char*>("http://lg.istreamsche.com/")

int AdProtocolMtMedia::HttpToInternal(AdAdapterPara * pPara)
{
	int iCount = 0;
	int iFloorPrice = 0;
	int iFlowtype = AD_PROTOCOL_FLOW_PC;
	Json::Reader reader;
	Json::Value value;
	Json::Value subvalue;
	Json::Value grandvalue;
	Json::Value ggrandvalue;
	int iReturn = AD_SUCCESS;
	char szVal[AD_KBYTES1] = { 0 };
	string sValue;

	AdPrivateInfo *pPInfo = new AdPrivateInfo;
	if(pPInfo == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}

	pPara->m_pPrivate = static_cast<void *>(pPInfo);
	
	if (true == reader.parse(pPara->m_pBody, value)) 
	{
		if (true == value.isMember("rid")) 
		{
			pPara->m_ctRequest.m_sBidID = value["rid"].asString();
		}
		if( value.isMember("ad_w") && value.isMember("ad_h")) 
		{
			sprintf(szVal, "%sx%s", value["ad_w"].asString().c_str(), value["ad_h"].asString().c_str());
			sValue= szVal;
			pPara->m_ctRequest.m_sSize = sValue;              
		}
		
		if (true == value.isMember("flowtype")) 
		{
			iFlowtype = atoi(value["flowtype"].asString().c_str());
		}
		if (true == value.isMember("pid")) 
		{
		    pPara->m_ctRequest.m_sPId = value["pid"].asString();
		}
		if(true == value.isMember("ip")) 
		{
			pPara->m_ctRequest.m_sUserIP = value["ip"].asString();
		}
		# if 0	
		if (true == value.isMember("device")) 
		{
			subvalue = value["device"];
			// required string device ua = 1;
			if(true == subvalue.isMember("ua"))
			{
				pPara->m_ctRequest.m_sUserAgent = subvalue["ua"].asString();
			}
			// required string device ip = 2;
			if(true == subvalue.isMember("ip")) 
			{
				pPara->m_ctRequest.m_sUserIP = subvalue["ip"].asString();
			}
			// required string device language = 3;		
			if(true == subvalue.isMember("language")) 
			{
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

			// required string device ext = 14;
			if(true == subvalue.isMember("ext")) 
			{
				grandvalue = subvalue["ext"];
				// required string device ext idfa = 1;
				if (true == grandvalue.isMember("idfa")) 
				{
					pPara->m_sIDFA = grandvalue["idfa"].asString();
				}
				// required string device ext idfa = 1;
				if (true == grandvalue.isMember("imei")) 
				{
					pPInfo->m_sIMEI = grandvalue["imei"].asString();
				}				
			}
				// required object device geo = 15;
			if(true == subvalue.isMember("geo")) 
			{
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
				pPara->m_sIDFA = subvalue["id"].asString();
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
					pPInfo->m_sImpId = subvalue["id"].asString();
				}

				// required string imp tagid = 2;
				if(true == subvalue.isMember("tagid")) 
				{
					pPara->m_ctRequest.m_sPId = subvalue["tagid"].asString();
				}
				
				// required float imp bidfloor = 3;
				if (true == subvalue.isMember("bidfloor")) 
				{
					iFloorPrice = subvalue["bidfloor"].asInt();
				}

				// required object imp banner = 4;
				if(true == subvalue.isMember("banner")) 
				{
					grandvalue = subvalue["banner"];
					// required int imp banner w = 1,h = 2;
					if(true == grandvalue.isMember("w")) 
					{
						sprintf(szVal, "%dx%d", grandvalue["w"].asInt(), grandvalue["h"].asInt());
						sValue= szVal;
						pPara->m_ctRequest.m_sSize = sValue;              
					}
				}
				// feed
				if(true == subvalue.isMember("feed")){
					grandvalue = subvalue["feed"];
					if(true == grandvalue.isMember("type")){
						
					}

				} 
				break;//first impression
			}   
		}
        #endif

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
	
	return iReturn;
}

int AdProtocolMtMedia::InternalToHttp(AdAdapterPara * pPara)
{
	int iLength = 0;
	string type;
	Json::Value root;
	Json::Value value;
	Json::Value iarray;
	Json::Value varray;
	Json::Value item;
	Json::Value ext;
	std::string reBuffer;
	Json::FastWriter writer;
	string sClickPara;
  	char szHead[AD_KBYTES2] = { 0 };       
	char sShowPara[AD_KBYTES2];

	AdPrivateInfo  * pInfo = static_cast<AdPrivateInfo*>(pPara->m_pPrivate);

	if(pInfo == NULL)
	{
		return AD_FAILURE;
	}
	
	root["rid"] = pPara->m_ctResponse.m_sBidID;
	root["executeid"] = pPara->m_ctResponse.m_sExecID;
	root["advid"] = pPara->m_ctResponse.m_sAdvertiserId;
	root["creativeid"] = pPara->m_ctResponse.m_sCreativeID;
	root["areaid"] = pPara->m_ctResponse.m_sArea;
	root["material_url"] = pPara->m_ctResponse.m_sHtml;
	root["pid"] = pPara->m_ctResponse.m_sPId;
    root["click_url"] = pPara->m_ctResponse.m_sDestUrl;
   	
	int iFormat = atoi(pPara->m_ctResponse.m_sMaterailFormat.c_str());
		switch(iFormat){//1:png, 2:jpg, 3:gif, 4:flv, 5:flash, 6:swf 7:hmtl 8:MP4 
			case AD_PROTOCOL_MAT_FORMAT_PNG:
				type = "img";
				break;
			case AD_PROTOCOL_MAT_FORMAT_JPG:
				type = "img";
				break;
			case AD_PROTOCOL_MAT_FORMAT_GIF:
				type = "img";
				break;
			case AD_PROTOCOL_MAT_FORMAT_FLV:
				type = "mv";
				break;
			case AD_PROTOCOL_MAT_FORMAT_SWF:
				type = "flash";
				break;
			default:
				type = "img";
				break;
			 }
	root["m_type"] = type;

	reBuffer = writer.write(root);

	/* head */
	iLength = reBuffer.length();
	snprintf(szHead,  AD_KBYTES2, AD_HTTP200_JSON, iLength);

    int ret;
    ret = pPara->m_ctCacheOut.Write(szHead ,strlen(szHead));
    if(ret != AD_SUCCESS)
        return AD_FAILURE;

    ret = pPara->m_ctCacheOut.Append(reBuffer.c_str(), iLength);
    if(ret != AD_SUCCESS)
        return AD_FAILURE;

	delete pInfo;
	pPara->m_pPrivate = NULL;
	return AD_SUCCESS;
}

int AdProtocolMtMedia::EmptyToHttp(AdAdapterPara * pPara)
{
	AdPrivateInfo * pInfo = static_cast<AdPrivateInfo *>(pPara->m_pPrivate);

    snprintf(pPara->m_ctCacheOut.Get(), pPara->m_ctCacheOut.Max(),AD_HTTP204, 0);
           pPara->m_ctCacheOut.Set(strlen(pPara->m_ctCacheOut.Get()));

	if(pInfo != NULL)
	{
		delete pInfo;
		pPara->m_pPrivate = NULL;
	}
	return AD_SUCCESS;
}

AdProtocolBase * AdProtocolInit()
{
	AdProtocolMtMedia *  pPro = new AdProtocolMtMedia;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
	}
	return static_cast<AdProtocolBase *>(pPro);
}
