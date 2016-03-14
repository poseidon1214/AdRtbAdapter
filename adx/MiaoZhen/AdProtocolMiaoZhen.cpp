/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolMiaoZhen.h"
#include "json/json.h"
#include <string.h>
#include <stdlib.h>

#define AD_URL        const_cast<char*>("http://mzlog.istreamsche.com/")

#define	AD_MIAOZHEN_PRICE_CLICK		"p=%%WINNING_PRICE%%&clickurl=%%CLICK_URL_ESC%%"
#define AD_MIAOZHEN_IMGURL 		const_cast<char*>("http://cc.xtgreat.com/cm.gif?dspid=11212")

#define MZ_STR_MD5     (pPara->m_ctRequest.m_sPara1)
#define MZ_STR_IMPID     (pPara->m_ctRequest.m_sPara2)

int AdProtocolMiaoZhen::HttpToInternal(AdAdapterPara * pPara)
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
	
	if (true == reader.parse(pPara->m_pBody, value)) 
	{
		if (true == value.isMember("id")) 
		{
			pPara->m_ctRequest.m_sBidID = value["id"].asString();
		}

		if (true == value.isMember("site")) 
		{
			subvalue = value["site"];
			if(true == subvalue.isMember("name"))
			{
			}

			// page
			if(true == subvalue.isMember("page")) 
			{
				pPara->m_ctRequest.m_sUserUrl =  subvalue["page"].asString().c_str();
			}
			//  ref
			if(true == subvalue.isMember("ref")) 
			{
			}
					
			// cat
			if( true == subvalue.isMember("cat"))
			{
					//
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
			// required string device language = 3;		
			if(true == subvalue.isMember("language")) 
			{
			}
			// required int device js = 4;
			if (true == subvalue.isMember("js")) 
			{
			}
			// required int device devicetype = 5;
			if (true == subvalue.isMember("devicetype")) 
			{
				if( subvalue["devicetype"].asInt() == 3)
				{
					pPara->m_ctRequest.m_ltMobileDevice.push_back(AD_PROTOCOL_MOBILE_DEV_TV);
				}
			}
			// didmd5
			if (true == subvalue.isMember("didmd5")) 
			{
				MZ_STR_MD5 = subvalue["didmd5"].asString();
			}
			// dpidmd5
			if (true == subvalue.isMember("dpidmd5")) 
			{
			}
			// required string device make = 8;
			if (true == subvalue.isMember("make")) 
			{
			}
			// required string device model = 9;
			if (true == subvalue.isMember("model"))
			{
				if(strcasestr(subvalue["model"].asString().c_str(), "iphone")  != NULL)
				{
					sValue = AD_PROTOCOL_MOBILE_DEV_IPHONE;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(sValue);
                    		}
				if(strcasestr(subvalue["model"].asString().c_str(), "ipad")  != NULL)
				{
					sValue = AD_PROTOCOL_MOBILE_DEV_IPAD;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(sValue);
                    		}
				if(strcasestr(subvalue["model"].asString().c_str(), "android")  != NULL)
				{
					sValue = AD_PROTOCOL_MOBILE_DEV_ANDROID;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(sValue);
                   		 }
			}

			// required string device dpidmd5 = 10;
			if (true == subvalue.isMember("os")) 
			{
				if(strcasestr(subvalue["os"].asString().c_str(), "ios")  != NULL)
				{
					iFlowtype = AD_PROTOCOL_FLOW_MOBILE;
					sValue = AD_PROTOCOL_MOBILE_DEV_IPHONE;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(sValue);
					sValue = AD_PROTOCOL_MOBILE_DEV_IPAD;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(sValue);
				}
				if(strcasestr(subvalue["os"].asString().c_str(), "android")  != NULL)
				{
					iFlowtype = AD_PROTOCOL_FLOW_MOBILE;
					sValue = AD_PROTOCOL_MOBILE_DEV_ANDROID;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(sValue);
					sValue = AD_PROTOCOL_MOBILE_DEV_APAD;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(sValue);
				}
				if(strcasestr(subvalue["os"].asString().c_str(), "wp")  != NULL)
				{
					iFlowtype = AD_PROTOCOL_FLOW_MOBILE;
					sValue = AD_PROTOCOL_MOBILE_DEV_WPHONE;
					pPara->m_ctRequest.m_ltMobileDevice.push_back(sValue);
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
					pPara->m_ctRequest.m_sIDFA = grandvalue["idfa"].asString();
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
				//pPara->m_sIDFA = subvalue["id"].asString();
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
					MZ_STR_IMPID= subvalue["id"].asString();
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
					if( true == grandvalue.isMember("pos")) 
					{
						if (grandvalue["pos"].asInt() == 1)
						{
							sValue = "1";
							pPara->m_ctRequest.m_sAdViewScreen = sValue;
						}
					}
				}
				// pmp
				if(true == subvalue.isMember("pmp")) 
				{
					grandvalue = subvalue["pmp"];
					// deals
					if( true == grandvalue.isMember("deals"))
					{
						for(iCount = 0; (UINT)iCount < ggrandvalue.size(); iCount++)
						{
							ggrandvalue = grandvalue["deals"][iCount];								
								// id
							if( true == ggrandvalue.isMember("id") )
							{
							}							
						}
					}
				}	
				
				// required object imp video = 5;
				if(true == subvalue.isMember("video")) 
				{
					grandvalue = subvalue["video"];

					//set flow type
					iFlowtype = AD_PROTOCOL_FLOW_VIDEO;
					// required int imp video w = 1,h = 2;
					if(true == grandvalue.isMember("w")) 
					{
						sprintf(szVal, "%dx%d", grandvalue["w"].asInt(), grandvalue["h"].asInt());
						pPara->m_ctRequest.m_sSize = szVal;
					}
					else 
					{
					}
					// required array int imp video mimes = 3;
					if(true == grandvalue.isMember("mimes")) 
					{

					}
					// required int imp video linearity = 4;
					if (true == grandvalue.isMember("linearity")) 
					{
						sprintf(szVal, "%d",  grandvalue["linearity"].asInt());
						sValue= szVal;
						pPara->m_ctRequest.m_sLinearity = sValue;
					}
					else 
					{
					}
					// required int imp video minduration = 5;
					if (true == grandvalue.isMember("minduration")) 
					{
						sprintf(szVal, "%d",  grandvalue["minduration"].asInt());
						sValue= szVal;
						pPara->m_ctRequest.m_sVideoMinTime = sValue;
					}
					
					// required int imp video maxduration = 6;
					if (true == grandvalue.isMember("maxduration")) 
					{
						sprintf(szVal, "%d",  grandvalue["maxduration"].asInt());
						sValue= szVal;
						pPara->m_ctRequest.m_sVideoMaxTime = sValue;
					}

				}
				if(true == subvalue.isMember("ext")) 
				{
					grandvalue = subvalue["ext"];
					if( true == grandvalue.isMember("showtype"))
					{
						if( grandvalue["showtype"].asInt() == 5 ||\
								grandvalue["showtype"].asInt() == 8)
						{
							pPara->m_ctRequest.m_sAdViewType = AD_PROTOCOL_VIEW_TYPE_FIXED;
						}
						if( grandvalue["showtype"].asInt() == 4)
						{
							pPara->m_ctRequest.m_sAdViewType = AD_PROTOCOL_VIEW_TYPE_VIDEOPOUSE;
						}
					}
				}					
				break;//first impression
			}   

		}

		// required object app = 5;
		if(true == value.isMember("app")) 
		{
			grandvalue = value["app"];

			//set flow type
			iFlowtype = AD_PROTOCOL_FLOW_MOBILE;
			// required string app name = 1;
			if(true == grandvalue.isMember("name")) 
			{                
			}

			// required string app ext = 2;
			if(true == grandvalue.isMember("ext"))
			{
				grandvalue = grandvalue["ext"];
				// required string app ext sdk = 1;
				if(true == grandvalue.isMember("sdk")) 
				{
				}
				// required int app ext market = 2;
				if (true == grandvalue.isMember("market")) 
				{
				}
				// required string app ext appid = 3;
				if(true == grandvalue.isMember("appid")) 
				{             
				}

				// required string app ext cat = 4;
				if(true == grandvalue.isMember("cat")) 
				{                   
				}

				// required string app ext tag = 5;
				if(true == grandvalue.isMember("tag"))
				{                  
				}
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

int AdProtocolMiaoZhen::InternalToHttp(AdAdapterPara * pPara)
{
	int iLength = 0;
	int ret;
	
	Json::Value root;
	Json::Value value;
	Json::Value iarray;
	Json::Value varray;
	Json::Value item;
	Json::Value ext;
	std::string SMiaoZhen;
	Json::FastWriter writer;
	string sClickPara;
  	char szHead[AD_KBYTES2];       
	char sShowPara[AD_KBYTES2];
	char sTemp[AD_KBYTES4] = {0};

	string sImgUrl = AD_MIAOZHEN_IMGURL;
	string sExtStr = AD_MIAOZHEN_PRICE_CLICK;
	// width height snip gc eid bid info
	switch(atoi(pPara->m_ctResponse.m_sFlow.c_str()))
	{
		case AD_PROTOCOL_FLOW_PC:
			ret = GetShowHtml(pPara, sExtStr, sImgUrl, sTemp, AD_KBYTES4);
			if(ret != AD_SUCCESS)
				return AD_FAILURE;
			ext["type"] = "c";
			item["crid"] = pPara->m_ctResponse.m_sCreativeID;
			item["adm"] = sTemp;
			break;
		case AD_PROTOCOL_FLOW_MOBILE:
			GetClickPara(&(pPara->m_ctResponse), sClickPara);
			GetShowPara(pPara, sShowPara, AD_KBYTES2 );
			
			snprintf( sTemp,AD_KBYTES4 , "%smzshow?p=${AUCTION_PRICE}&%s",AD_URL, sShowPara);
			ext["pm"].append(sTemp);
			if( !pPara->m_ctResponse.m_ltMonitorUrl.empty())
			{
				ext["pm"].append(*(pPara->m_ctResponse.m_ltMonitorUrl.begin()));
			}

			snprintf(sTemp, AD_KBYTES4, "%smzclick?%s&gc=%s",  AD_URL, sShowPara, sClickPara.c_str());
			ext["cm"].append(sTemp);

			item["adm"] = pPara->m_ctResponse.m_sHtml;

			ext["ldp"] = pPara->m_ctResponse.m_sDestUrl;
			break;
		case AD_PROTOCOL_FLOW_VIDEO:
			GetClickPara(&(pPara->m_ctResponse), sClickPara);
			GetShowPara(pPara, sShowPara, AD_KBYTES2 );
			ext["type"] = "flv"; 
			snprintf( sTemp,AD_KBYTES4 , "%smzshow?p=${AUCTION_PRICE}&%s",AD_URL, sShowPara);
			ext["pm"].append(sTemp);
			if( !pPara->m_ctResponse.m_ltMonitorUrl.empty())
			{
				ext["pm"].append(*(pPara->m_ctResponse.m_ltMonitorUrl.begin()));
			}

			snprintf(sTemp, AD_KBYTES4, "%smzclick?%s&gc=%s",  AD_URL, sShowPara, sClickPara.c_str());
			ext["cm"].append(sTemp);

			item["adm"] = pPara->m_ctResponse.m_sHtml;

			ext["ldp"] = pPara->m_ctResponse.m_sDestUrl;
			break;
		default:
			break;
	}
	// id
	root["id"] = pPara->m_ctResponse.m_sBidID;
	// bidid 
	root["bidid"] = "1";
	item["impid"] = MZ_STR_IMPID;
	item["id"] = "1";
	item["ext"] = ext;	
	item["nurl"] = "";
	item["price"] = atof(pPara->m_ctResponse.m_sMaxPrice.c_str());
	iarray.append(item);

	value["bid"] = iarray;
	varray.append(value);	
	root["seatbid"] = varray;
	SMiaoZhen = writer.write(root);

	iLength = SMiaoZhen.length();
	snprintf(szHead,  AD_KBYTES2, AD_HTTP200_JSON, iLength);

	ret = pPara->m_ctCacheOut.Write(szHead ,strlen(szHead));
	if(ret != AD_SUCCESS)
		return AD_FAILURE;

	ret = pPara->m_ctCacheOut.Append(const_cast<char *>(SMiaoZhen.c_str()), iLength);
	if(ret != AD_SUCCESS)
		return AD_FAILURE;

	return AD_SUCCESS;
}

int AdProtocolMiaoZhen::EmptyToHttp(AdAdapterPara * pPara)
{
	snprintf(pPara->m_ctCacheOut.Get(), pPara->m_ctCacheOut.Max(),AD_HTTP204, 0);
       pPara->m_ctCacheOut.Set(strlen(pPara->m_ctCacheOut.Get()));
	return AD_SUCCESS;
}

int  SharedLibInit(AdProtocolBase *  & ctpPro)
{
	AdProtocolMiaoZhen *  pPro = new AdProtocolMiaoZhen;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}

