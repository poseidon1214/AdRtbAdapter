/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolWeiBo.h"
#include "json/json.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "AdMd5.h"


#define AD_URL        const_cast<char*>("http://lg.istreamsche.com/")

#define WB_STR_DEALID   (pPara->m_ctRequest.m_sPara1)
#define WB_STR_IMPID       (pPara->m_ctRequest.m_sPara2)

int AdProtocolWeiBo::HttpToInternal(AdAdapterPara * pPara)
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
	char szVal[AD_BYTE512] = { 0 };
	string sValue;
	
	if (true == reader.parse(pPara->m_pBody, value)) 
	{
		if (true == value.isMember("id")) 
		{
			pPara->m_ctRequest.m_sBidID = value["id"].asString();
		}
		
		if (true == value.isMember("dealid")) 
		{
			WB_STR_DEALID = value["dealid"].asString();
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
			//if(true == subvalue.isMember("language")) 
			//{
			//}
			// geo
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
					AdMd5 md5;
					unsigned char key[AD_MD5 * 2];
					string devid = "weibogeotmtdataexchange" + grandvalue["idfa"].asString();
					md5.AdMd5Run((char *)devid.c_str(), devid.size(),  key);

					char tmp[16] = {0};
				    for(int j = 0; j < 16; j++ )
				    {
				       sprintf( tmp,"%02x", key[j]);
				       pPara->m_ctRequest.m_sIDFA += tmp;
				    }
				}
				// required string device ext idfa = 1;
				if (true == grandvalue.isMember("imei")) 
				{
					AdMd5 md5;
					unsigned char key[AD_MD5 * 2];
					string devid = "weibogeotmtdataexchange" + grandvalue["idfa"].asString();
					md5.AdMd5Run((char *)devid.c_str(), devid.size(),  key);

					char tmp[16] = {0};
				    for(int j = 0; j < 16; j++ )
				    {
				       sprintf( tmp,"%02x", key[j]);
				       pPara->m_ctRequest.m_sIMEI += tmp;
				    }					
				}				
			}
				// required object device geo = 15;
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
			// gender
			if(true == subvalue.isMember("gender"))
			{
				if (subvalue["gender"].asString() == "M")
				{
					pPara->m_ctRequest.m_ltCrowdCategory.push_back("GENDER_M");					
				}
				if (subvalue["gender"].asString() == "F")
				{
					pPara->m_ctRequest.m_ltCrowdCategory.push_back("GENDER_F");					
				}				
			}			
			// yob
			if(true == subvalue.isMember("yob"))
			{
				string age;
				UserAgeGroup(subvalue["yob"].asInt(), age);
				if(age.size() > 0){
					pPara->m_ctRequest.m_ltCrowdCategory.push_back(age);
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
					WB_STR_IMPID= subvalue["id"].asString();
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
					pPara->m_ctRequest.m_sSize = "1x1";              
					grandvalue = subvalue["feed"];
					if(true == grandvalue.isMember("type")){
						
					}

				} 
				#if 0
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
				#endif	
				break;//first impression
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

int AdProtocolWeiBo::InternalToHttp(AdAdapterPara * pPara)
{
	int iLength = 0;
	
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
	
	GetClickPara(&(pPara->m_ctResponse), sClickPara);
	GetShowPara(pPara, sShowPara, AD_KBYTES2 );
	// width height snip gc eid bid info

	// win notice
	char pv[AD_KBYTES1] = {0};
	snprintf( pv, AD_KBYTES1, "%swbshow?p=${AUCTION_PRICE}&bid=${AUCTION_BID_ID}&%s", AD_URL, sShowPara);
	item["nurl"] = pv ;
	// dsp click monitor
	char cm[AD_KBYTES1] = {0};
	snprintf( cm, AD_KBYTES1, "%swbclick?%s&gc=%s", AD_URL, sShowPara, sClickPara.c_str());
	ext["cm"].append(cm);
	// pv monitor
	if( !pPara->m_ctResponse.m_ltMonitorUrl.empty())
	{
		ext["pm"].append(*(pPara->m_ctResponse.m_ltMonitorUrl.begin()));
	}
	// landing page
	ext["ldp"] = pPara->m_ctResponse.m_sDestUrl;	
	root["id"] = pPara->m_ctResponse.m_sBidID;
	item["impid"] = WB_STR_IMPID;
	item["crid"] = pPara->m_ctResponse.m_sMaterialID;
	item["id"] = "1";	
	item["adm"] = pPara->m_ctResponse.m_sHtml;
	item["price"] = atof(pPara->m_ctResponse.m_sMaxPrice.c_str());
	item["ext"] = ext;	
	// bidid 
	root["bidid"] = "1";
	iarray.append(item);
	value["bid"] = iarray;
	varray.append(value);
	root["seatbid"] = varray;
	item["nurl"] = "";
	item["price"] = atof(pPara->m_ctResponse.m_sMaxPrice.c_str());
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

	return AD_SUCCESS;
}

int AdProtocolWeiBo::EmptyToHttp(AdAdapterPara * pPara)
{
    snprintf(pPara->m_ctCacheOut.Get(), pPara->m_ctCacheOut.Max(),AD_HTTP204, 0);
           pPara->m_ctCacheOut.Set(strlen(pPara->m_ctCacheOut.Get()));
	return AD_SUCCESS;
}

int AdProtocolWeiBo::UserAgeGroup(int born_year, string &sAge)
{
    	Time ctTime;
	int now_year = ctTime.getYear();
	int ages = now_year - born_year;
	
	if ( ages < 10 )
	{
		sAge = "AGE_1";
	}
	else if( ages < 20)
	{
		sAge = "AGE_2";
	}
	else if( ages < 25)
	{
		sAge = "AGE_3";
	}
	else if( ages < 30)
	{
		sAge = "AGE_4";
	}
	else if( ages < 35)
	{
		sAge = "AGE_5";
	}	
	else if( ages < 40)
	{
		sAge = "AGE_6";
	}	
	else if( ages < 50)
	{
		sAge = "AGE_7";
	}	
	else
	{
		sAge = "AGE_8";
	}
	return AD_SUCCESS;
}

int  SharedLibInit(AdProtocolBase *  & ctpPro)
{
	AdProtocolWeiBo *  pPro = new AdProtocolWeiBo;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
		return AD_FAILURE;
	}
	ctpPro = static_cast<AdProtocolBase *>(pPro);
	return AD_SUCCESS;
}

