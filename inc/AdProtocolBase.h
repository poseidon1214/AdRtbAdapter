/***************************************
*	Description: adx protocol transfer interface.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef __AD_PROTOCOL_BASE_H__
#define __AD_PROTOCOL_BASE_H__

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <zlib.h>
#include "AdGlobal.h"
#include "AdRtbBase.h"
#include "AdUtil.h"
#include "AdUaParser.h"

using namespace std;
// js url
#define AD_JS_URL const_cast<char*>("http://material.istreamsche.com/aittp.js")

//http response head
#define AD_HTTP200_JSON   const_cast<char*>("HTTP/1.0 200 OK\r\nContent-Length: %d\r\nContent-Type: application/json; charset=UTF-8\r\nServer: TornadoServer/2.2\r\n\r\n")
#define	AD_HTTP200			const_cast<char*>("HTTP/1.0 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html; charset=UTF-8\r\nServer: TornadoServer/2.2\r\n\r\n")
#define	AD_HTTP200_OCT			const_cast<char*>("HTTP/1.0 200 OK\r\nContent-Length: %d\r\nContent-Type: application/octet-stream; charset=UTF-8\r\nServer: AitouServer/2.2\r\n\r\n")
#define   AD_HTTP204           		const_cast<char*>("HTTP/1.0 204 No Content\r\nContent-Length: %d\r\nContent-Type: application/json; charset=UTF-8\r\nServer: TornadoServer/2.2\r\n\r\n")

//http response body,  html infomation
#define	AD_SHOW_INFO			"time:%s\tarea:%s\ttag_cr:%s\ttag_kw:%s\thot_key:%s\tsize:%s\tadverID:%s\texecuteID:%s\tcreativeID:%s\tcgroupID:%s\tmaterialID:%s\ttID:%s\trequestID:%s\tunionID:%d\tbidding_price:%s\tres_equip_id:%s\tpID:%s\ttarget_price:%s\tbilling_mode:%s\tos:%s\tidfa:%s"
#define	AD_HTML_SNIP			"<iframe width=\"%s\" height=\"%s\" frameborder=\"0\" scrolling=\"no\" src=\"%s?%s&gc=%s&pid=%s&%s\"></iframe><img src=\"%s&\"/>"																																 
#define	AD_HTML_SNIP_NO_EXTEND			"<iframe width=\"%s\" height=\"%s\" frameborder=\"0\" scrolling=\"no\" src=\"%s?%s&gc=%s&pid=%s\"></iframe><img src=\"%s&\"/>"	

#define AD_SCRIPT_SNIP "<style type=\"text/css\">*{margin:0px; padding:0px;}</style><script type=\"text/javascript\"> document.write(\'<iframe width=\"%s\" height=\"%s\" frameborder=\"0\" scrolling=\"no\" src=\"%s?%s&gc=%s&pid=%s&%s&bdclick=\'+encodeURIComponent(\"%s\") +\'\"></iframe>\');</script>"


#define AD_SINP  const_cast<char*>("<!DOCTYPE html><html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><style type=\"text/css\">* { margin: 0; padding: 0; border: 0; }</style><script type=\"text/javascript\" src=\"%s\"></script></head><body><script type=\"text/javascript\">var _d = {\"tpType\": \"%s\",\"v\":\"1.0.0\",\"mUrl\": \"%s\",\"cu\": \"%s\",\"su\": \"%s\",\"cmu\": \"%s\",\"s3u\": \"%s\",\"w\": \"%s\",\"h\": \"%s\",\"lImg\": \"http://material.istreamsche.com/logo/a_2.png\",\"lhImg\": \"http://material.istreamsche.com/logo/b_2.png\",\"lpst\": \"rb\",\"lUrl\": \"http://aitou.geotmt.com/un/reg/form\"};aittp.adHandler(_d);</script></body></html>")


#define AD_MSG_TYPE_REQUEST          				1
#define AD_MSG_TYPE_RESPONSE        				2
#define AD_MSG_TYPE_REQUEST_BIDDING                 3

class AdLogMsg
{
public:
	AdLogMsg()
	{
		clear();
	//	m_iMaxLen = AD_KBYTES4;
	};
	~AdLogMsg()
	{
	};
	void clear()
	{
		m_iLength = AD_FAILURE;
		m_ucType = 0;
	};
	int m_iLength;
	//int m_iMaxLen;
	UCHAR m_ucType;
	char m_uMsg[AD_KBYTES8];
};

class AdAdapterPara
{
public:
	AdAdapterPara()
	{
		clear();
	};
	~AdAdapterPara(){};
	void clear()
	{
		m_pCacheIn= NULL;
		m_pBody = NULL;
		m_iBody = 0;
		m_ctCacheReq.clear();
		m_ctCacheRes.clear();
		m_ctRequest.clear();
		m_ctResponse.clear();
		m_sVer.clear();
		m_sImpression.clear();
		m_pPrivate = NULL;
		m_pSrcSession = NULL;
		m_pDstSession = NULL;
		m_pAdxMethod = NULL;
		m_sIDFA.clear();
		m_bDebugUser = false;
		m_bTimeout = false;
		m_ctCacheOut.Clear();
	}
	AdCache * m_pCacheIn;		               // Buffer
	AdCache   m_ctCacheOut;
	char * m_pBody;
	int m_iBody;
	AdLogMsg  m_ctCacheReq;			// ReqLog
	AdLogMsg  m_ctCacheRes;		      // ResLog
	
	AdRtbRequest m_ctRequest;
	AdRtbResponse m_ctResponse;
	
	string m_sVer;
	string m_sImpression;
	string m_sIDFA;
	void * m_pPrivate;
	void * m_pSrcSession;
	void * m_pDstSession;
	void * m_pAdxMethod;
	bool    m_bDebugUser;
	bool    m_bTimeout;
};

class AdProtocolBase
{
public:
	AdProtocolBase(){};
	virtual ~AdProtocolBase(){};

	virtual int HttpToInternal(AdAdapterPara * pPara)
	{
		return AD_SUCCESS;
	};
	virtual int InternalToHttp(AdAdapterPara * pPara)
	{
		return AD_SUCCESS;
	};

	virtual int EmptyToHttp(AdAdapterPara * pPara)
	{
		return AD_SUCCESS;
	};

private:
	AdUserAgentParser m_AdUaParser;

protected:
	inline int AdUaParser(AdAdapterPara * pPara)
	{
		
		UA_MAP_RESULT mResult;
		UA_MAP_RESULT::iterator iter;
		char szVal[AD_BYTE16] = {0};
			
		m_AdUaParser.parse(pPara->m_ctRequest.m_sUserAgent, mResult);
		if ( !mResult.empty() ){
			for(iter = mResult.begin(); iter != mResult.end(); iter ++)
			{
				sprintf(szVal, "%d", (int&)(iter->first));
				pPara->m_ctRequest.m_ltUalist.push_back(szVal);
			}
			return AD_SUCCESS;
		}
		else{
			return AD_FAILURE;
		}
	};
		
	inline int AdCompress(UCHAR* pSrc, int iSrc, UCHAR* puszDst, int* piDst)
	{
		ULONG ullDst = AD_KBYTES4;
		int iRet = 	compress(puszDst, &ullDst, (const Bytef*)pSrc, (ULONG)iSrc);
   	 	*piDst = ullDst;
		return iRet;
	};

	inline int AdUnCompress(UCHAR* pSrc, int iSrc, UCHAR* puszDst, int* piDst)
	{
		ULONG ullDst = AD_KBYTES4;
		int iRet = uncompress(puszDst, &ullDst, (const Bytef*)pSrc, (ULONG)iSrc);
   		 *piDst = ullDst;
		return iRet;
	};

	unsigned char ToHex(unsigned char x) 
	{ 
		return  x > 9 ? x + 55 : x + 48; 
	};

	unsigned char FromHex(unsigned char x) 
	{ 
		unsigned char y;
		if (x >= 'A' && x <= 'Z') 
			y = x - 'A' + 10;
		else if (x >= 'a' && x <= 'z') 
			y = x - 'a' + 10;
		else if (x >= '0' && x <= '9') 
			y = x - '0';
   		 return y;
	};

	void UrlEncode(const std::string& str, std::string & sDst)
	{
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (isalnum((unsigned char)str[i]) || (str[i] == '-') || (str[i] == '_') || 
				(str[i] == '.') || (str[i] == '~'))
			{
				sDst += str[i];
			}
			else if (str[i] == ' ')
				sDst += "+";
			else
			{
 				sDst	 += '%';
				sDst += ToHex((unsigned char)str[i] >> 4);
				sDst += ToHex((unsigned char)str[i] % 16);
			}
		}
	};

	void UrlDecode(const std::string& str,  std::string & sDst)
	{
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (str[i] == '+')
				sDst += ' ';
			else if (str[i] == '%')
			{
				if(i + 2 < length)
				{
					unsigned char high = FromHex((unsigned char)str[++i]);
					unsigned char low = FromHex((unsigned char)str[++i]);
					sDst += high*16 + low;
				}
			}
			else 
				sDst += str[i];
		}
	};

	int BaseEncode(UCHAR* pSrc, int iSrc, UCHAR* pDst, int iDst)
	{
		int iLen = AD_FAILURE;
		UCHAR *pIn = NULL;
		UCHAR *pEnd = NULL;
		UCHAR *pPos = NULL;
		UCHAR sBase[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		if(((iSrc + 3 - iSrc % 3) * 4 / 3 + 1 ) > iDst)
		{
			return AD_FAILURE;
		}
		
		if (NULL !=  pDst ) 
		{
			pIn = pSrc;
			pEnd = pSrc + iSrc;
			pPos = pDst;

			while (pEnd - pIn >= 3) {
				*pPos ++ = sBase[pIn[0] >> 2];
				*pPos ++ = sBase[((pIn[0] & 0x03) << 4) | (pIn[1] >> 4)];
				*pPos ++ = sBase[((pIn[1] & 0x0f) << 2) | (pIn[2] >> 6)];
				*pPos ++ = sBase[pIn[2] & 0x3f];
				pIn += 3;
			}

			if (pEnd - pIn) {
				*pPos ++ = sBase[pIn[0] >> 2];
				if (pEnd - pIn == 1) {
					*pPos ++ = sBase[(pIn[0] & 0x03) << 4];
					*pPos ++ = '=';
				}
				else {
					*pPos ++ = sBase[((pIn[0] & 0x03) << 4) | (pIn[1] >> 4)];
					*pPos ++ = sBase[(pIn[1] & 0x0f) << 2];
				}
				*pPos ++ = '=';
			}

			iLen = pPos - pDst;
			*pPos = '\0';
		}

		return iLen;
	};

	int BaseDecode(UCHAR* pSrc, int iSrc, UCHAR* pDst, int iDst)
	{
		int iCnt = 0;
		int iLen = AD_FAILURE;
		int iCount = 0;
		UCHAR ucTmp = '\0';
		UCHAR *pPos = NULL;
		UCHAR sIn[4] = { 0 };
		UCHAR sBlock[4] = { 0 };
		UCHAR sTable[256] = { 0x80 };
		UCHAR sBase[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		for (iCnt = 0; iCnt < (int)sizeof(sBase) - 1; iCnt ++) {
			sTable[sBase[iCnt]] = (UCHAR)iCnt;
		}
		sTable['='] = 0;

		for (iCnt = 0; iCnt < iSrc; iCnt ++) {
			if (0x80 != sTable[pSrc[iCnt]]) {
				iCount ++;
			}
		}

		if ((0 == iCount % 4)  && ((iCount / 4 * 3) <=  iDst))
		{
			if (NULL != pDst) 
			{
				pPos = pDst;
				iCount = 0;
				for (iCnt = 0; iCnt < iSrc; iCnt ++) {
					if (0x80 == (ucTmp = sTable[pSrc[iCnt]])) {
						continue;
					}
			
					sIn[iCount] = pSrc[iCnt];
					sBlock[iCount] = ucTmp;
					iCount ++;
					if (4 ==  iCount) {
						*pPos ++ = (sBlock[0] << 2) | (sBlock[1] >> 4);
						*pPos ++ = (sBlock[1] << 4) | (sBlock[2] >> 2);
						*pPos ++ = (sBlock[2] << 6) | sBlock[3];
						iCount = 0;
					}
				}

				if (pPos > pDst) {
					if (sIn[2] == '=') {
						pPos -= 2;
					}
					else if (sIn[3] == '=') {
						pPos --;
					}
				}
				iLen = pPos - pDst;
			}
		}

		return iLen;
	};

	int CheckUserId(AdAdapterPara * pPara)
	{
		// mobile
		if (atoi(pPara->m_ctRequest.m_sFlow.c_str()) == AD_PROTOCOL_FLOW_MOBILE)
		{
			if(!pPara->m_ctRequest.m_sIDFA.empty())
			{
				pPara->m_ctRequest.m_sUserID = pPara->m_ctRequest.m_sIDFA;
				return AD_SUCCESS;
			}
			else if(!pPara->m_ctRequest.m_sIMEI.empty())
			{
				pPara->m_ctRequest.m_sUserID = pPara->m_ctRequest.m_sIMEI;
				return AD_SUCCESS;
			}	
			else if(!pPara->m_ctRequest.m_sAndroidID.empty())
			{
				pPara->m_ctRequest.m_sUserID = pPara->m_ctRequest.m_sAndroidID;
				return AD_SUCCESS;
			}			
			else if(!pPara->m_ctRequest.m_sUserIP.empty())
			{
				pPara->m_ctRequest.m_sUserID = pPara->m_ctRequest.m_sUserIP;
				return AD_SUCCESS;
			}			
		}
		
		if(pPara->m_ctRequest.m_sUserID.empty())
		{
			if(!pPara->m_ctRequest.m_sUserIP.empty())
			{
				pPara->m_ctRequest.m_sUserID = pPara->m_ctRequest.m_sUserIP;
				return AD_SUCCESS;
			}
			else{
				return AD_FAILURE;
			}
		}
		return AD_SUCCESS;
	}

	void GetClickPara(AdRtbResponse * pRes, string & sPara)
	{
		char cPara[AD_KBYTES1];
    		snprintf(cPara,  AD_KBYTES1, "%s,%s,%s,%d,%s,%s,%s,%s", \
			pRes->m_sBidID.c_str(), pRes->m_sAdvertiserId.c_str(), \
			pRes->m_sExecID.c_str(), pRes->m_uiAdx, pRes->m_sPId.c_str(), \
			pRes->m_sArea.c_str(), pRes->m_sCreativeID.c_str(), pRes->m_sHotKey.c_str());
      		UrlEncode(cPara, sPara);
	}

	void GetShowInfo(AdAdapterPara * pPara, char* pBuf, int iBuf)
	{
		string cat;
		int iVal = 0;
		char sVal[AD_KBYTES2];
		char tmpTime[AD_BYTE32] ;
		sprintf(tmpTime, "%ld", time(NULL));

		AdRtbResponse * pRes = &(pPara->m_ctResponse);
		
		list<string>::iterator iter;
		for(iter = pPara->m_ctRequest.m_ltSiteCategory.begin(); iter != pPara->m_ctRequest.m_ltSiteCategory.end(); iter ++)
		{
			cat = (*iter).c_str();
			break;
		}
		
		snprintf(sVal, AD_KBYTES2, AD_SHOW_INFO, tmpTime, pRes->m_sArea.c_str(), pRes->m_sHotTag.c_str(), \
				pRes->m_sKeyWord.c_str(), pRes->m_sHotKey.c_str(), pRes->m_sSize.c_str(), \
				pRes->m_sAdvertiserId.c_str(), pRes->m_sExecID.c_str(), \
				pRes->m_sCreativeID.c_str(), pRes->m_sCreateGID.c_str(), pRes->m_sMaterialID.c_str(),\
				pRes->m_sUserID.c_str(), pRes->m_sBidID.c_str(), pRes->m_uiAdx, pRes->m_sMaxPrice.c_str(), \
				pRes->m_sRegion.c_str(), pRes->m_sPId.c_str(), pRes->m_sTargetPrice.c_str(), pRes->m_sTargetPriceType.c_str(), \
				pRes->m_sDevType.c_str(), pPara->m_ctRequest.m_sIDFA.c_str());

		iVal= BaseEncode((UCHAR*)sVal, strlen(sVal), (UCHAR *)pBuf, iBuf);
		if (iVal > 0)
		{
			while (0x3d == pBuf[-- iVal]) 
			{
				if(iVal == 0)
					break;
				pBuf[ iVal] = '\0';
			}
		}
	}

	void GetShowPara(AdAdapterPara * pPara, char* pBuf, int iBuf)
	{
		char sVal[AD_KBYTES2];
		
		char tmpTime[AD_BYTE32] ;
		sprintf(tmpTime, "%ld", time(NULL));

		AdRtbResponse * pRes = &(pPara->m_ctResponse);
		
		GetShowInfo(pPara, sVal, AD_KBYTES2);
		
		snprintf(pBuf,  iBuf, "tp=%d&adx=%d&aid=%s&eid=%s&cid=%s&bid=%s&info=%s&tm=%s",\
		 1, pRes->m_uiAdx, pRes->m_sAdvertiserId.c_str(), \
			pRes->m_sExecID.c_str(), pRes->m_sCreativeID.c_str(), pRes->m_sBidID.c_str(), sVal, tmpTime);
	}																															 

	int GetShowHtml(AdAdapterPara * pPara, string & sExtStr,  string & sImage,  char* pBuf, int iBuf)
	{
		char sShowPara[AD_KBYTES2];
		string sClickPara;
		vector<string> vStr; 
		
		GetShowPara(pPara, sShowPara, AD_KBYTES2);
		GetClickPara(&(pPara->m_ctResponse), sClickPara);
		
		AdString::StrSplit(pPara->m_ctResponse.m_sSize, vStr, "x");
		if(vStr.size() != 2)
			return AD_FAILURE;
		
		if(sExtStr.empty())
		{
			snprintf(pBuf,  iBuf, AD_HTML_SNIP_NO_EXTEND,  vStr[0].c_str(), vStr[1].c_str(),
					pPara->m_ctResponse.m_sHtml.c_str(), sShowPara, 
					sClickPara.c_str(), sClickPara.c_str(), sImage.c_str());
		}
		else
		{
			snprintf(pBuf,  iBuf, AD_HTML_SNIP,  vStr[0].c_str(), vStr[1].c_str(),
					pPara->m_ctResponse.m_sHtml.c_str(), sShowPara, 
					sClickPara.c_str(), sClickPara.c_str(), 
					sExtStr.c_str(), sImage.c_str());
		}

		return AD_SUCCESS;
	}
	int GetScriptHtml(AdAdapterPara * pPara, string & sExtStr,  string & clickmacro, char* pBuf, int iBuf)
	{
		char sShowPara[AD_KBYTES2];
		string sClickPara;
		vector<string> vStr; 
		
		GetShowPara(pPara, sShowPara, AD_KBYTES2);
		GetClickPara(&(pPara->m_ctResponse), sClickPara);
		
		AdString::StrSplit(pPara->m_ctResponse.m_sSize, vStr, "x");
		if(vStr.size() != 2)
			return AD_FAILURE;
		
		if(sExtStr.empty())
		{
			// pass
		}
		else
		{
			snprintf(pBuf,  iBuf, AD_SCRIPT_SNIP,  vStr[0].c_str(), vStr[1].c_str(),
					pPara->m_ctResponse.m_sHtml.c_str(), sShowPara, 
					sClickPara.c_str(), sClickPara.c_str(), 
					sExtStr.c_str(), clickmacro.c_str());
		}

		return AD_SUCCESS;
	}	
	void AdPageUrlParse(string &sSrc, vector<string> &vStr)
	{
		vector<string> vStrTemp;
		vector<string>::iterator iter;
		string sTemp;
		AdString::StrSplit(sSrc, vStrTemp, "//");
		if(vStrTemp.size() >= 2)
		{
			sTemp = vStrTemp[1];
			vStrTemp.clear();
		}
		else
			return;
		AdString::StrSplit(sTemp, vStrTemp, "/");
		if(vStrTemp.size() >= 1)
		{
			sTemp = vStrTemp[0];
			vStrTemp.clear();
		}
		else
			return;
	
		AdString::StrSplit(sTemp, vStrTemp, "?");
		if(vStrTemp.size() >= 1)
		{
			sTemp = vStrTemp[0];
			vStrTemp.clear();
		}
		else
			return;
	
		AdString::StrSplit(sTemp, vStrTemp, ".");
		if(vStrTemp.empty())
			return;
		while(vStrTemp.size() > 1)
		{
			sTemp.clear();
			for(iter = vStrTemp.begin(); iter != vStrTemp.end(); iter ++)
			{
				if(sTemp.empty())
				{
					sTemp = *iter;
				}
				else
				{
					sTemp += "." + *iter;
				}
			}
			vStrTemp.erase(vStrTemp.begin());
			vStr.push_back(sTemp);
		}
	}
		
	int GenerateSinppet(AdAdapterPara * pPara, char * sBuf, string &cmurl, string &price_macro, string &click_macro,int iBuf)
	{
		string type;
		string sClickPara;
		string sClickEncode;
		string sClickUrlRes;
		char sShowPara[AD_KBYTES2];
		char sShowUrl[AD_KBYTES2] = {0};
		char sAdClickUrl[AD_KBYTES2] = {0};	
		
		GetShowPara(pPara, sShowPara, AD_KBYTES2);
		GetClickPara(&(pPara->m_ctResponse), sClickPara);
		
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
			// 7,8	not surpport
		}
		switch( pPara->m_ctRequest.m_uiAdx ){
			case AD_PROTOCOL_KEY_ADX_GOOGLE:
				snprintf(sShowUrl, AD_KBYTES2,"http://lg.istreamsche.com/ggshow?p=%s&%s", price_macro.c_str(), sShowPara);
				snprintf(sAdClickUrl, AD_KBYTES2,"http://lg.istreamsche.com/ggclick?gc=%s&%s", sClickPara.c_str(), sShowPara);
				break;
			case AD_PROTOCOL_KEY_ADX_INMOBI:
				snprintf(sShowUrl, AD_KBYTES2,"http://inmolog.istreamsche.com/inmoshow?p=%s&%s", price_macro.c_str(), sShowPara);
				snprintf(sAdClickUrl, AD_KBYTES2,"http://inmolog.istreamsche.com/inmoclick?gc=%s&%s&url=%s", sClickPara.c_str(), sShowPara, pPara->m_ctResponse.m_sClickUrl.c_str());	
				break;
			case AD_PROTOCOL_KEY_ADX_BAIDU:
				snprintf(sShowUrl, AD_KBYTES2,"http://lg.istreamsche.com/besshow?p=%s&%s", price_macro.c_str(), sShowPara);
				//snprintf(sAdClickUrl, AD_KBYTES2,"http://lg.istreamsche.com/besclick?gc=%s&%s&url=%s", sClickPara.c_str(), sShowPara, pPara->m_ctResponse.m_sClickUrl.c_str());	
				break;
			// add another adx
			default:
				break;
		}
		if (click_macro.size() > 0){
			UrlEncode(sAdClickUrl, sClickEncode);
		    sClickUrlRes =  click_macro + sClickEncode;
		}
		else{
		    sClickUrlRes =  sAdClickUrl;
		}
		/*
			type | mUrl | clickurl |  showurl | cookiemapping url  | third impurl | width | height 
		*/
		snprintf(sBuf, iBuf, AD_SINP, AD_JS_URL, type.c_str(), \
			pPara->m_ctResponse.m_sHtml.c_str(), sClickUrlRes.c_str(), sShowUrl, cmurl.c_str(), \
			(*pPara->m_ctResponse.m_ltMonitorUrl.begin()).c_str(),\
			pPara->m_ctResponse.m_sWidth.c_str(), pPara->m_ctResponse.m_sHeight.c_str() \
		);
		return AD_SUCCESS;
	}

	int GenerateVastVedio( char* pBuf, int iBuf, AdAdapterPara * pPara,  string &price_macro, string &click_macro )
	{

		char sVal[AD_KBYTES2];
		
		char tmpTime[AD_BYTE32] ;
		sprintf(tmpTime, "%ld", time(NULL));
		string sMateUrlEncode;
		string sMoniUrlEncode;
		AdRtbResponse * pRes = &(pPara->m_ctResponse);
		
		GetShowInfo(pPara, sVal, AD_KBYTES2);
		
		UrlEncode(pPara->m_ctResponse.m_sHtml, sMateUrlEncode);

		if( pPara->m_ctResponse.m_ltMonitorUrl.size() > 0){
			UrlEncode(*pPara->m_ctResponse.m_ltMonitorUrl.begin(),sMoniUrlEncode);
		}
		// for adserver
		snprintf(pBuf,  iBuf, "http://lg.istreamsche.com/vedio?p=%s&adx=%d&aid=%s&eid=%s&dura=%s&format=%s&w=%s&h=%s&tm=%s&clickmacro=%s&material=%s&desurl=%s&monitorurl=%s&info=%s",\
		 	price_macro.c_str(), pRes->m_uiAdx, pRes->m_sAdvertiserId.c_str(), \
			pRes->m_sExecID.c_str(),pRes->m_sDuration.c_str(),pRes->m_sMaterailFormat.c_str(),\
			pRes->m_sWidth.c_str(), pRes->m_sHeight.c_str(), \
			tmpTime, click_macro.c_str(), sMateUrlEncode.c_str(),\
			pRes->m_sClickUrl.c_str(), sMoniUrlEncode.c_str(), sVal \
			);
		return AD_SUCCESS;
    }	
	
};

#endif

