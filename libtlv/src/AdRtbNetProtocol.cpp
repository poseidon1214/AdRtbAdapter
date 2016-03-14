/***************************************
*	Description: net protocol,  include rtb request and response
*	Author: wilson
*	Date: 2015/07/22
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#include "AdRtbNetProtocol.h"
#include <zlib.h>
#include <netinet/in.h>

//tlv protocol head
#define	AD_PROTOCOL_EXPPAND_1		1
#define	AD_PROTOCOL_FLAG				0xe8

#define	AD_PROTOCOL_BODY_COMP			0x01
/* dev */
#define	AD_PROTOCOL_KEY_DEVICE								0xff			// device
/* verb */
#define	AD_PROTOCOL_KEY_VERB_REQUEST					1			// request
#define	AD_PROTOCOL_KEY_VERB_RESPONSE					2			// response

#define   AD_PROTOCOL_RESULT_OK                                                          0
#define   AD_PROTOCOL_RESULT_UNKNOW                                            0xff

//tlv key define

/* device id */
#define	AD_PROTOCOL_KEY_DEVICE_IDFA							90			// idfa
#define	AD_PROTOCOL_KEY_DEVICE_IMEI							91			// imei
#define	AD_PROTOCOL_KEY_DEVICE_ANDROID							92			// android id


/* rtb */
#define	AD_PROTOCOL_KEY_RTB_UID							101			// uid
#define	AD_PROTOCOL_KEY_RTB_BIDID							102			// bid = (T:2)
#define	AD_PROTOCOL_KEY_RTB_IP								103			// ip = (T:4) (M:device:3)
#define	AD_PROTOCOL_KEY_RTB_USERAGENT					104			// user_agent = (T:5) (M:device:2)
#define	AD_PROTOCOL_KEY_RTB_CLICK_URL					105			// excluded_click_through_url = (T:6) badv = (M:9)
#define	AD_PROTOCOL_KEY_RTB_USER_URL					106			// url = (T:7) (M:site:4)
#define	AD_PROTOCOL_KEY_RTB_CATEGORY					107			// category = (T:8) (M:site:6)
#define	AD_PROTOCOL_KEY_RTB_ANONYMOUS_ID				110			// anonymous_id = (T:15)

#define	AD_PROTOCOL_KEY_SENSITIVE_CATEGORY			112			// excluded_sensitive_category = (T:17)
#define	AD_PROTOCOL_KEY_AD_CATEGORY						114			// excluded_ad_category = (T:20) #(Q:Impression:7)

#define	AD_PROTOCOL_KEY_DEVICE_TYPE						122			// devicetype = (M:device:6)
#define	AD_PROTOCOL_KEY_TENCENT_DURATION_MIN		123			// tencent ad minduration (Q:Impression:Video:3) # 2 RTB
#define	AD_PROTOCOL_KEY_TENCENT_DURATION_MAX		124			// tencent ad maxduration (Q:Impression:Video:4) # 2 RTB
#define	AD_PROTOCOL_KEY_LINEARITY							127			// linearity

/* Adirect */
#define	AD_PROTOCOL_KEY_ADIRECT_BILLING_MODE			131			// billing_mode
#define	AD_PROTOCOL_KEY_ADIRECT_ORDER_ID				132			// orderID
#define	AD_PROTOCOL_KEY_ADIRECT_EXEC_ID				133			// executeID
#define	AD_PROTOCOL_KEY_ADIRECT_MATERIAL_ID			134			// materialID
#define	AD_PROTOCOL_KEY_ADIRECT_REGION					135			// region
#define	AD_PROTOCOL_KEY_ADIRECT_FLOW_TYPE			136			// flow type

/* adv */
#define	AD_PROTOCOL_KEY_ADV_PID							203			// AdzInfo.pid = (T:2) (M:imp:3) (Q:impression:1) # (Q:impression:2)
#define	AD_PROTOCOL_KEY_ADV_SIZE							204			// AdzInfo.size = (T:3) (M:imp:banner:1£¬2) # w x h
#define	AD_PROTOCOL_KEY_ADV_VIEW_TYPE					206			// AdzInfo.view_type = (T:5) (M:imp:banner:5) (Q:impression:Banner:3 Q:impression:Video:1)
#define	AD_PROTOCOL_KEY_ADV_EXEC_FILTER				207			// AdzInfo.excluded_filter = (T:6) #(Q:impression:Banner:3 Q:impression:Video:1) not found
#define	AD_PROTOCOL_KEY_ADV_MIN_CPM_PRICE				208			// AdzInfo.min_cpm_price = (T:7) (M:imp:4) (Q:impression:3)
#define	AD_PROTOCOL_KEY_ADV_VIEW_SCREEN				210			// AdzInfo.view_screen = (T:9) (M:imp:banner:3) (Q:impression:Banner:5)
#define	AD_PROTOCOL_KEY_ADV_MAX_CPM_PRICE			211			// AdzInfo.max_cpm_price = (T:2)
#define	AD_PROTOCOL_KEY_ADV_CLICK_URL					214			// AdzInfo.click_through_url = (T:5)
#define	AD_PROTOCOL_KEY_ADV_HTML							213			// AdzInfo.html_snippet = (T:4) #(Q:R:Bid:6)
#define	AD_PROTOCOL_KEY_ADV_CREATIVE_TYPE				215			// AdzInfo.creative_type = (T:7)
#define	AD_PROTOCOL_KEY_ADV_DESTINATION_URL			218			// AdzInfo.destination_url = (T:10)
#define	AD_PROTOCOL_KEY_ADV_CREATIVE_ID				219			// AdzInfo.creative_id = (T:11)

#define	AD_PROTOCOL_KEY_MATERIAL_FORMAT				220			// allyesadformat = (M:imp:banner:4)

#define	AD_PROTOCOL_KEY_CREATIVE_ID						229			// seatbid bid crid  (allyes creative id)

#define	AD_PROTOCOL_KEY_MOBILE_CONNECT_TYPE			231			// net connection
#define	AD_PROTOCOL_KEY_MOBILE_OPERATOR				232			// mobile operator
#define	AD_PROTOCOL_KEY_MOBILE_APP						234			// app type

#define	AD_PROTOCOL_KEY_HOT_KEY							236			// hot key
#define	AD_PROTOCOL_KEY_HOT_URL							237			// hot url
#define	AD_PROTOCOL_KEY_HOT_TIME							238			// hot time
#define	AD_PROTOCOL_KEY_MONITOR_URL						239			// monitor url

#define  AD_PROTOCOL_KEY_TRANSACTION_TYPE				240			// transaction type
#define  AD_PROTOCOL_KEY_ADVERTISER_ID					241			// advertiser_id
#define  AD_PROTOCOL_KEY_DURATION							242			// duration

#define  AD_PROTOCOL_KEY_DEVICE_UNIQUE_TYPE			243			// device_unique_type
#define  AD_PROTOCOL_KEY_HOT_TAG							244			// hot tag
#define  AD_PROTOCOL_KEY_CREATE_GROUP_ID				245			// create group id
#define  AD_PROTOCOL_KEY_TARGET_PRICE						246			// target price
#define  AD_PROTOCOL_KEY_ADIRECT_USER_ID					247			// geo uid
#define  AD_PROTOCOL_KEY_ADIRECT_COOKIE_ID				248			// geo cookie id
#define  AD_PROTOCOL_KEY_KEYWORD							249			// keyword tag
#define  AD_PROTOCOL_KEY_MRAID_MATERIAL					251			// Mraid Material
#define  AD_PROTOCOL_KEY_CROWD_CATEGORY					253        //used for capture data

#define  AD_PROTOCOL_KEY_USERAGENT_ID					254			// user agent id

#define AD_PROTOCOL_PARSE_TYPE_STRING         			1
#define AD_PROTOCOL_PARSE_TYPE_LIST_STRING			2

#if 0
#define	AD_PROTOCOL_KV(K, V)		do {						\
	if(!V.empty())													\
	{																	\
		*pCur = K;															\
		pCur += sizeof(UCHAR);												\
		*(int *)pCur =htonl (V.size());											\
		pCur += sizeof(int);													\
		memcpy(pCur,  V.c_str(),   V.size());									\
		pCur += V.size();														\
		usCount ++;															\
	}																		\
} while (0)
#endif

#define	AD_PROTOCOL_KV(K, V, L)		do {						\
		if(!V.empty())														\
		{																	\
			if(L < (int)(sizeof(UCHAR) + sizeof(int) +  V.size()))						\
				return AD_FAILURE;										\
			*pCur = K;															\
			pCur += sizeof(UCHAR);												\
			*(int *)pCur =htonl (V.size()); 										\
			pCur += sizeof(int);													\
			memcpy(pCur,  V.c_str(),   V.size());									\
			pCur += V.size();														\
			usCount ++; 														\
			L -= sizeof(UCHAR) + sizeof(int) + V.size();							\
		}																	\
	} while (0)


AdRtbProtocol::AdRtbProtocol()
{
	m_stHead.ucFlag = AD_PROTOCOL_FLAG;
	m_stHead.ucVersion = 1;
	m_stHead.ucProtocol = 1;
	m_stHead.ucWide = 0;
	m_stHead.ucResponse = 1;
	m_stHead.ucExpand = 0;
		
	m_stHead.ucType = 0;
	m_stHead.ucVerb = AD_PROTOCOL_KEY_VERB_REQUEST;
	m_stHead.ucAttr = 1;
	m_stHead.ucResult = AD_PROTOCOL_RESULT_OK;
	m_stHead.usTotal = 0;
	m_stHead.iLength = 0;
}

void AdRtbProtocol::AdRtbProtocolInitParser(AdRtbRequest & ctReq)
{
	m_mParse.clear();
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_FLOW_TYPE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_FLOW_TYPE].pointer = static_cast<void *>(&ctReq.m_sFlow);

	m_mParse[AD_PROTOCOL_KEY_RTB_BIDID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_RTB_BIDID].pointer = static_cast<void *>(&ctReq.m_sBidID);

	m_mParse[AD_PROTOCOL_KEY_RTB_UID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_RTB_UID].pointer = static_cast<void *>(&ctReq.m_sUserID);
		
	m_mParse[AD_PROTOCOL_KEY_RTB_IP].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_RTB_IP].pointer = static_cast<void *>(&ctReq.m_sUserIP);

	m_mParse[AD_PROTOCOL_KEY_ADV_PID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_PID].pointer = static_cast<void *>(&ctReq.m_sPId);
	
	m_mParse[AD_PROTOCOL_KEY_MOBILE_APP].type = AD_PROTOCOL_PARSE_TYPE_LIST_STRING;
	m_mParse[AD_PROTOCOL_KEY_MOBILE_APP].pointer = static_cast<void *>(&ctReq.m_ltSiteCategory);

	m_mParse[AD_PROTOCOL_KEY_ADV_SIZE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_SIZE].pointer = static_cast<void *>(&ctReq.m_sSize);

	m_mParse[AD_PROTOCOL_KEY_RTB_USERAGENT].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_RTB_USERAGENT].pointer = static_cast<void *>(&ctReq.m_sUserAgent);

	m_mParse[AD_PROTOCOL_KEY_RTB_USER_URL].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_RTB_USER_URL].pointer = static_cast<void *>(&ctReq.m_sUserUrl);

	m_mParse[AD_PROTOCOL_KEY_DEVICE_UNIQUE_TYPE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_DEVICE_UNIQUE_TYPE].pointer = static_cast<void *>(&ctReq.m_sDevUniqueType);

	m_mParse[AD_PROTOCOL_KEY_ADV_VIEW_SCREEN].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_VIEW_SCREEN].pointer = static_cast<void *>(&ctReq.m_sAdViewScreen);

	m_mParse[AD_PROTOCOL_KEY_ADV_VIEW_TYPE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_VIEW_TYPE].pointer = static_cast<void *>(&ctReq.m_sAdViewType);
	
	m_mParse[AD_PROTOCOL_KEY_ADV_EXEC_FILTER].type = AD_PROTOCOL_PARSE_TYPE_LIST_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_EXEC_FILTER].pointer = static_cast<void *>(&ctReq.m_ltCreate);

	m_mParse[AD_PROTOCOL_KEY_AD_CATEGORY].type = AD_PROTOCOL_PARSE_TYPE_LIST_STRING;
	m_mParse[AD_PROTOCOL_KEY_AD_CATEGORY].pointer = static_cast<void *>(&ctReq.m_ltCategory);

	m_mParse[AD_PROTOCOL_KEY_USERAGENT_ID].type = AD_PROTOCOL_PARSE_TYPE_LIST_STRING;
	m_mParse[AD_PROTOCOL_KEY_USERAGENT_ID].pointer = static_cast<void *>(&ctReq.m_ltUalist);

	
	m_mParse[AD_PROTOCOL_KEY_MATERIAL_FORMAT].type = AD_PROTOCOL_PARSE_TYPE_LIST_STRING;
	m_mParse[AD_PROTOCOL_KEY_MATERIAL_FORMAT].pointer = static_cast<void *>(&ctReq.m_ltMaterialFormat);

	m_mParse[AD_PROTOCOL_KEY_SENSITIVE_CATEGORY].type = AD_PROTOCOL_PARSE_TYPE_LIST_STRING;
	m_mParse[AD_PROTOCOL_KEY_SENSITIVE_CATEGORY].pointer = static_cast<void *>(&ctReq.m_ltSensitive);

	m_mParse[AD_PROTOCOL_KEY_ADV_MIN_CPM_PRICE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_MIN_CPM_PRICE].pointer = static_cast<void *>(&ctReq.m_sMinPrice);

	m_mParse[AD_PROTOCOL_KEY_TENCENT_DURATION_MIN].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_TENCENT_DURATION_MIN].pointer = static_cast<void *>(&ctReq.m_sVideoMinTime);

	m_mParse[AD_PROTOCOL_KEY_TENCENT_DURATION_MAX].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_TENCENT_DURATION_MAX].pointer = static_cast<void *>(&ctReq.m_sVideoMaxTime);

	m_mParse[AD_PROTOCOL_KEY_LINEARITY].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_LINEARITY].pointer = static_cast<void *>(&ctReq.m_sLinearity);
	
	m_mParse[AD_PROTOCOL_KEY_MOBILE_OPERATOR].type = AD_PROTOCOL_PARSE_TYPE_LIST_STRING;
	m_mParse[AD_PROTOCOL_KEY_MOBILE_OPERATOR].pointer = static_cast<void *>(&ctReq.m_ltMobileOperator);

	m_mParse[AD_PROTOCOL_KEY_DEVICE_TYPE].type = AD_PROTOCOL_PARSE_TYPE_LIST_STRING;
	m_mParse[AD_PROTOCOL_KEY_DEVICE_TYPE].pointer = static_cast<void *>(&ctReq.m_ltMobileDevice);

	m_mParse[AD_PROTOCOL_KEY_MOBILE_CONNECT_TYPE].type = AD_PROTOCOL_PARSE_TYPE_LIST_STRING;
	m_mParse[AD_PROTOCOL_KEY_MOBILE_CONNECT_TYPE].pointer = static_cast<void *>(&ctReq.m_ltMobileConnect);

	m_mParse[AD_PROTOCOL_KEY_TRANSACTION_TYPE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_TRANSACTION_TYPE].pointer = static_cast<void *>(&ctReq.m_sTransaction);

	m_mParse[AD_PROTOCOL_KEY_DEVICE_IDFA].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_DEVICE_IDFA].pointer = static_cast<void *>(&ctReq.m_sIDFA);
	
	m_mParse[AD_PROTOCOL_KEY_DEVICE_IMEI].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_DEVICE_IMEI].pointer = static_cast<void *>(&ctReq.m_sIMEI);

	m_mParse[AD_PROTOCOL_KEY_DEVICE_ANDROID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_DEVICE_ANDROID].pointer = static_cast<void *>(&ctReq.m_sAndroidID);

}

int AdRtbProtocol::TlvPacketLen(char * pData, int iLen)
{
	UCHAR ucFlag = *pData;
	
	if(iLen < AD_PROTOCOL_HEAD_LEN)
	{
		return AD_FAILURE;
	}
	
	if (AD_PROTOCOL_FLAG == ucFlag ) 
	{
		pData += sizeof(UCHAR);
	}
	else 
	{
		return AD_FAILURE;
	}
	return ntohl(*(int*)(pData + 8));
}


int AdRtbProtocol::TlvToInternal(AdRtbRequest & ctReq, char * pdata,int iLen, bool bPrint)
{
	int kvLen;
	char * pBody;
	char* pCur;
	map<UCHAR, struct parser>::iterator  iter;
	char sUnCompress[AD_KBYTES8];

	ctReq.clear();

	AdRtbProtocolInitParser(ctReq);

	memset(&m_stHead, 0, sizeof(m_stHead));

	if(iLen < AD_PROTOCOL_HEAD_LEN)
	{
		return AD_FAILURE;
	}

	pCur = pdata;
	
	m_stHead.ucFlag = *pCur;
	if (AD_PROTOCOL_FLAG == m_stHead.ucFlag ) 
	{
		pCur += sizeof(UCHAR);
	}
	else 
	{
		return AD_FAILURE;
	}
	m_stHead.ucVersion = *pCur >> 4;
	m_stHead.ucProtocol = (*pCur & 0x0f) >> 3;
	m_stHead.ucWide = (*pCur & 0x07) >> 2;
	m_stHead.ucResponse = (*pCur & 0x03) >> 1;
	m_stHead.ucExpand = *pCur & 0x01;
	pCur += sizeof(UCHAR);

	m_stHead.ucType = *pCur;
	pCur += sizeof(UCHAR);

	m_stHead.ucVerb = *pCur;
	pCur += sizeof(UCHAR);

	m_stHead.ucAttr = *pCur;
	pCur += sizeof(UCHAR);

	m_stHead.ucResult = *pCur;
	pCur += sizeof(UCHAR);

	m_stHead.usTotal = ntohs(*(USHORT*)pCur);
	pCur += sizeof(USHORT);

	m_stHead.iLength = ntohl(*(int*)pCur);
	pCur += sizeof(int);

	pBody =  pCur;

	
	if (AD_PROTOCOL_EXPPAND_1 == m_stHead.ucExpand)
	{
		m_sExHead.assign(const_cast<const char *>(pBody), AD_PROTOCOL_HEAD_EXPAND_LEN);
		pBody +=  AD_PROTOCOL_HEAD_EXPAND_LEN;
		if(iLen < AD_PROTOCOL_HEAD_LEN + AD_PROTOCOL_HEAD_EXPAND_LEN + m_stHead.iLength)
			return AD_FAILURE;
	}
	else
	{
		if(iLen < AD_PROTOCOL_HEAD_LEN + m_stHead.iLength)
			return AD_FAILURE;
	}

	if(m_stHead.ucAttr == AD_PROTOCOL_BODY_COMP)
	{
		ULONG  iDst =AD_KBYTES4;
		uncompress((Bytef *)sUnCompress, (ULONG*)&iDst, (Bytef *)pBody, (ULONG)m_stHead.iLength);
		//uncompress(m_revUnConpress, (ULONG*)&iDst, (const Bytef*)pBody, (ULONG)m_stHead.iLength);
		pBody = sUnCompress;
		m_stHead.iLength = iDst;
	}
	
	USHORT  usTotal = 0;
	UCHAR  ucKey;
	
	string  str;
	str.reserve(AD_BYTE256);
	
	pCur = pBody;
	while ((int)(pCur - pBody) < m_stHead.iLength)
	{
		if(usTotal >= m_stHead.usTotal)
			break;
		ucKey = *pCur;
		pCur += sizeof(UCHAR);
		
		kvLen= ntohl(*(int *)pCur);
		pCur += sizeof(int);
		
		iter = m_mParse.find(ucKey);
		if(iter != m_mParse.end())
		{
			switch((iter->second).type)
			{
				case AD_PROTOCOL_PARSE_TYPE_STRING:
					(static_cast<string*>((iter->second).pointer))->assign(const_cast<const char *>(pCur), kvLen);
					break;
				case AD_PROTOCOL_PARSE_TYPE_LIST_STRING:
					str.assign(const_cast<const char *>(pCur), kvLen);
					(static_cast<list<string> *>((iter->second).pointer))->push_back(str);
					break;
				default:
					break;
			}
		}
		pCur += kvLen ;
		usTotal ++;
	}

	ctReq.m_uiAdx = m_stHead.ucType;
	char sBuf[AD_BYTE128];
	snprintf(sBuf, AD_BYTE128, "%d", AD_MODE_BID_PRICE);
	ctReq.m_sTransaction = sBuf;
	if(bPrint == true)
	{
		ctReq.display();
	}
	return AD_SUCCESS;
}

int AdRtbProtocol::InternalToTlv(AdRtbResponse & ctRes, char * pdata,int iLen, bool bPrint)
{
	char* pCur;
	USHORT  usCount = 0;
	char sCompress[AD_KBYTES8];
	char* pSend = pdata;
	int iSpace;
	int dataLen = 0;
	int headSize = AD_PROTOCOL_HEAD_LEN;
	if (AD_PROTOCOL_EXPPAND_1 == m_stHead.ucExpand)
	{
		headSize += AD_PROTOCOL_HEAD_EXPAND_LEN;
	}

	m_stHead.ucType = ctRes.m_uiAdx;

	if(ctRes.m_uiAdx != 0)
	{
		pCur = pSend + headSize;
		iSpace = iLen - headSize;
			/* region */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADIRECT_REGION, ctRes.m_sArea, iSpace);
		/* core device */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_DEVICE, ctRes.m_sRegion, iSpace);
		/* hot url */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_HOT_URL, ctRes.m_sHotUrl, iSpace);
		/* hot time */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_HOT_TIME, ctRes.m_sHotTime, iSpace);
		/* hot tag */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_HOT_TAG, ctRes.m_sHotTag, iSpace);
		/* bid */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_RTB_BIDID, ctRes.m_sBidID, iSpace);
		/* executeID */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADIRECT_EXEC_ID, ctRes.m_sExecID, iSpace);
		/* user id */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_RTB_UID, ctRes.m_sUserID, iSpace);
		/* user ip */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_RTB_IP, ctRes.m_sUserIP, iSpace);
		/* pid */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_PID, ctRes.m_sPId, iSpace);
		/* size */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_SIZE, ctRes.m_sSize, iSpace);
		/* orderID */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADIRECT_ORDER_ID, ctRes.m_sOrderID, iSpace);
		/* creative_id */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_CREATIVE_ID, ctRes.m_sCreativeID, iSpace);
		/* create group id */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_CREATE_GROUP_ID, ctRes.m_sCreateGID, iSpace);	
		/* materialID */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADIRECT_MATERIAL_ID, ctRes.m_sMaterialID, iSpace);
		/* allyes creative id */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_CREATIVE_ID, ctRes.m_sUud, iSpace);
		/* creative */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_CREATIVE_TYPE, ctRes.m_sCreative, iSpace);	
		/* billing_mode */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADIRECT_BILLING_MODE, ctRes.m_sTargetPriceType, iSpace);
		/* max_cpm_price */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_MAX_CPM_PRICE, ctRes.m_sMaxPrice, iSpace);
		/* category */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_RTB_CATEGORY, ctRes.m_sCategory, iSpace);
		/* html_snippet */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_HTML, ctRes.m_sHtml, iSpace);
		/* html_snippet_mraid */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_MRAID_MATERIAL, ctRes.m_sHtmlMraid, iSpace);
		/* destination_url */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_DESTINATION_URL, ctRes.m_sDestUrl, iSpace);
		/* click_through_url */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_CLICK_URL, ctRes.m_sClickUrl, iSpace);
		/* flash duaration */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_DURATION, ctRes.m_sDuration, iSpace);
		/* advertiser id */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADVERTISER_ID, ctRes.m_sAdvertiserId, iSpace);
		/* target price */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_TARGET_PRICE, ctRes.m_sTargetPrice, iSpace);
		/* formate id */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_MATERIAL_FORMAT, ctRes.m_sMaterailFormat, iSpace);
		/* geo uid */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADIRECT_USER_ID, ctRes.m_sAdirectID, iSpace);
		/* geo cookie id */
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADIRECT_COOKIE_ID, ctRes.m_sCookID, iSpace);
		/* tanx adx id*/
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_RTB_ANONYMOUS_ID, ctRes.m_sTanxID, iSpace);
		/* os / device type*/
		AD_PROTOCOL_KV(AD_PROTOCOL_KEY_DEVICE_TYPE, ctRes.m_sDevType, iSpace); // AD_PROTOCOL_KEY_DEVICE_TYPE 
		if( 0 == strncmp(ctRes.m_sHotKey.c_str(), "KW_", 3))
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_KEYWORD, ctRes.m_sHotKey, iSpace);
		}
		else
		{
			/* hot key */
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_HOT_KEY, ctRes.m_sHotKey, iSpace);
		}
	
		list<string>::iterator iter;
		/* monitor url */
		for(iter = ctRes.m_ltMonitorUrl.begin();  iter != ctRes.m_ltMonitorUrl.end(); iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_MONITOR_URL,  (*iter), iSpace);
		}

		dataLen = pCur - pSend - headSize;
		if(m_stHead.ucAttr == AD_PROTOCOL_BODY_COMP)
		{
			ULONG  iDst =AD_KBYTES4;
			compress((Bytef *)(sCompress), (ULONG*)&iDst, 
				(const Bytef*)(pSend +headSize), (ULONG)dataLen);
			if((int)iDst + headSize > iLen)
				return AD_FAILURE;
			memcpy(pSend +headSize, sCompress, iDst);
			*(pSend + headSize + iDst) = '\0';
			dataLen = iDst;
		}
	}
	else
	{
		m_stHead.ucResult = AD_PROTOCOL_RESULT_UNKNOW;
	}

	m_stHead.iLength = dataLen;
	m_stHead.usTotal = usCount;
	m_stHead.ucVerb  = AD_PROTOCOL_KEY_VERB_RESPONSE;

	pCur = pSend;
	
	*pCur = AD_PROTOCOL_FLAG;
	pCur += sizeof(UCHAR);
	
	*pCur = m_stHead.ucVersion << 4;
	*pCur |= m_stHead.ucProtocol << 3;
	*pCur |= m_stHead.ucWide << 2;
	*pCur |= m_stHead.ucResponse << 1;
	*pCur |= m_stHead.ucExpand;
	pCur += sizeof(UCHAR);
	
	*pCur = m_stHead.ucType;
	pCur += sizeof(UCHAR);
	
	*pCur = m_stHead.ucVerb;
	pCur += sizeof(UCHAR);
	
	*pCur = m_stHead.ucAttr;
	pCur += sizeof(UCHAR);
	
	*pCur = m_stHead.ucResult;
	pCur += sizeof(UCHAR);
	
	*(USHORT*)pCur = htons(m_stHead.usTotal);
	pCur += sizeof(USHORT);
	
	*(int*)pCur = htonl(m_stHead.iLength);
	pCur += sizeof(int);

	dataLen += headSize;
	if (AD_PROTOCOL_EXPPAND_1 == m_stHead.ucExpand)
	{
		memcpy(pCur, m_sExHead.c_str(), m_sExHead.size());
		dataLen += m_sExHead.size();
	}

	if(bPrint == true)
	{
		ctRes.display();
	}

	return dataLen;
}

void AdRtbProtocol::AdRtbProtocolInitParser(AdRtbResponse& ctRes)
{
	m_mParse.clear();
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_REGION].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_REGION].pointer = static_cast<void *>(&ctRes.m_sArea);

	m_mParse[AD_PROTOCOL_KEY_DEVICE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_DEVICE].pointer = static_cast<void *>(&ctRes.m_sRegion);

	m_mParse[AD_PROTOCOL_KEY_HOT_URL].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_HOT_URL].pointer = static_cast<void *>(&ctRes.m_sHotUrl);

	m_mParse[AD_PROTOCOL_KEY_HOT_TIME].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_HOT_TIME].pointer = static_cast<void *>(&ctRes.m_sHotTime);

	m_mParse[AD_PROTOCOL_KEY_HOT_TAG].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_HOT_TAG].pointer = static_cast<void *>(&ctRes.m_sHotTag);

	m_mParse[AD_PROTOCOL_KEY_RTB_BIDID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_RTB_BIDID].pointer = static_cast<void *>(&ctRes.m_sBidID);

	m_mParse[AD_PROTOCOL_KEY_ADIRECT_EXEC_ID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_EXEC_ID].pointer = static_cast<void *>(&ctRes.m_sExecID);

	m_mParse[AD_PROTOCOL_KEY_RTB_UID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_RTB_UID].pointer = static_cast<void *>(&ctRes.m_sUserID);

	m_mParse[AD_PROTOCOL_KEY_RTB_IP].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_RTB_IP].pointer = static_cast<void *>(&ctRes.m_sUserIP);

	m_mParse[AD_PROTOCOL_KEY_ADV_PID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_PID].pointer = static_cast<void *>(&ctRes.m_sPId);

	m_mParse[AD_PROTOCOL_KEY_ADV_SIZE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_SIZE].pointer = static_cast<void *>(&ctRes.m_sSize);

	m_mParse[AD_PROTOCOL_KEY_ADIRECT_ORDER_ID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_ORDER_ID].pointer = static_cast<void *>(&ctRes.m_sOrderID);

	m_mParse[AD_PROTOCOL_KEY_ADV_CREATIVE_ID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_CREATIVE_ID].pointer = static_cast<void *>(&ctRes.m_sCreativeID);

	m_mParse[AD_PROTOCOL_KEY_CREATE_GROUP_ID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_CREATE_GROUP_ID].pointer = static_cast<void *>(&ctRes.m_sCreateGID);

	m_mParse[AD_PROTOCOL_KEY_ADIRECT_MATERIAL_ID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_MATERIAL_ID].pointer = static_cast<void *>(&ctRes.m_sMaterialID);

	m_mParse[AD_PROTOCOL_KEY_CREATIVE_ID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_CREATIVE_ID].pointer = static_cast<void *>(&ctRes.m_sUud);

	m_mParse[AD_PROTOCOL_KEY_ADV_CREATIVE_TYPE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_CREATIVE_TYPE].pointer = static_cast<void *>(&ctRes.m_sCreative);

	m_mParse[AD_PROTOCOL_KEY_ADIRECT_BILLING_MODE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_BILLING_MODE].pointer = static_cast<void *>(&ctRes.m_sTargetPriceType);

	m_mParse[AD_PROTOCOL_KEY_ADV_MAX_CPM_PRICE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_MAX_CPM_PRICE].pointer = static_cast<void *>(&ctRes.m_sMaxPrice);

	m_mParse[AD_PROTOCOL_KEY_RTB_CATEGORY].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_RTB_CATEGORY].pointer = static_cast<void *>(&ctRes.m_sCategory);

	m_mParse[AD_PROTOCOL_KEY_ADV_HTML].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_HTML].pointer = static_cast<void *>(&ctRes.m_sHtml);

	m_mParse[AD_PROTOCOL_KEY_MRAID_MATERIAL].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_MRAID_MATERIAL].pointer = static_cast<void *>(&ctRes.m_sHtmlMraid);

	m_mParse[AD_PROTOCOL_KEY_ADV_DESTINATION_URL].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_DESTINATION_URL].pointer = static_cast<void *>(&ctRes.m_sDestUrl);

	m_mParse[AD_PROTOCOL_KEY_ADV_CLICK_URL].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADV_CLICK_URL].pointer = static_cast<void *>(&ctRes.m_sClickUrl);

	m_mParse[AD_PROTOCOL_KEY_DURATION].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_DURATION].pointer = static_cast<void *>(&ctRes.m_sDuration);
	
	m_mParse[AD_PROTOCOL_KEY_ADVERTISER_ID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADVERTISER_ID].pointer = static_cast<void *>(&ctRes.m_sAdvertiserId);
	
	m_mParse[AD_PROTOCOL_KEY_TARGET_PRICE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_TARGET_PRICE].pointer = static_cast<void *>(&ctRes.m_sTargetPrice);
	
	m_mParse[AD_PROTOCOL_KEY_MATERIAL_FORMAT].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_MATERIAL_FORMAT].pointer = static_cast<void *>(&ctRes.m_sMaterailFormat);
	
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_USER_ID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_USER_ID].pointer = static_cast<void *>(&ctRes.m_sAdirectID);
	
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_COOKIE_ID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_ADIRECT_COOKIE_ID].pointer = static_cast<void *>(&ctRes.m_sCookID);
	
	m_mParse[AD_PROTOCOL_KEY_RTB_ANONYMOUS_ID].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_RTB_ANONYMOUS_ID].pointer = static_cast<void *>(&ctRes.m_sTanxID);
	
	m_mParse[AD_PROTOCOL_KEY_DEVICE_TYPE].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_DEVICE_TYPE].pointer = static_cast<void *>(&ctRes.m_sDevType);

	m_mParse[AD_PROTOCOL_KEY_KEYWORD].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_KEYWORD].pointer = static_cast<void *>(&ctRes.m_sKeyWord);

	m_mParse[AD_PROTOCOL_KEY_HOT_KEY].type = AD_PROTOCOL_PARSE_TYPE_STRING;
	m_mParse[AD_PROTOCOL_KEY_HOT_KEY].pointer = static_cast<void *>(&ctRes.m_sHotKey);
	
	m_mParse[AD_PROTOCOL_KEY_MONITOR_URL].type = AD_PROTOCOL_PARSE_TYPE_LIST_STRING;
	m_mParse[AD_PROTOCOL_KEY_MONITOR_URL].pointer = static_cast<void *>(&ctRes.m_ltMonitorUrl);
}


int AdRtbProtocol::TlvToInternal(AdRtbResponse& ctRes, char * pdata, int iLen, bool bPrint)
{
	int kvLen;
	char * pBody;
	char* pCur;
	map<UCHAR, struct parser>::iterator  iter;
	char sUnCompress[AD_KBYTES8];
	int ret = AD_SUCCESS;

	USHORT  usTotal = 0;
	UCHAR  ucKey;
	string  str;
	str.reserve(AD_BYTE256);

	ctRes.clear();

	AdRtbProtocolInitParser(ctRes);

	memset(&m_stHead, 0, sizeof(m_stHead));

	pCur = pdata;
	
	m_stHead.ucFlag = *pCur;
	if (AD_PROTOCOL_FLAG == m_stHead.ucFlag ) 
	{
		pCur += sizeof(UCHAR);
	}
	else 
	{
		ret = AD_FAILURE;
		goto RETURN;
	}
	m_stHead.ucVersion = *pCur >> 4;
	m_stHead.ucProtocol = (*pCur & 0x0f) >> 3;
	m_stHead.ucWide = (*pCur & 0x07) >> 2;
	m_stHead.ucResponse = (*pCur & 0x03) >> 1;
	m_stHead.ucExpand = *pCur & 0x01;
	pCur += sizeof(UCHAR);

	m_stHead.ucType = *pCur;
	pCur += sizeof(UCHAR);

	m_stHead.ucVerb = *pCur;
	pCur += sizeof(UCHAR);

	m_stHead.ucAttr = *pCur;
	pCur += sizeof(UCHAR);

	m_stHead.ucResult = *pCur;
	pCur += sizeof(UCHAR);

	m_stHead.usTotal = ntohs(*(USHORT*)pCur);
	pCur += sizeof(USHORT);

	m_stHead.iLength = ntohl(*(int*)pCur);
	pCur += sizeof(int);

	pBody =  pCur;
	
	if (AD_PROTOCOL_EXPPAND_1 == m_stHead.ucExpand)
	{
		m_sExHead.assign(const_cast<const char *>(pBody), AD_PROTOCOL_HEAD_EXPAND_LEN);
		pBody +=  AD_PROTOCOL_HEAD_EXPAND_LEN;
		if(iLen < AD_PROTOCOL_HEAD_LEN + AD_PROTOCOL_HEAD_EXPAND_LEN + m_stHead.iLength)
		{
			ret = AD_FAILURE;
			goto RETURN;
		}
	}
	else
	{
		if(iLen < AD_PROTOCOL_HEAD_LEN + m_stHead.iLength)
		{
			ret = AD_FAILURE;
			goto RETURN;
		}
	}

	if(m_stHead.ucResult != AD_PROTOCOL_RESULT_OK)
	{
		ret = AD_FAILURE;
		goto RETURN;
	}

	if(m_stHead.ucAttr == AD_PROTOCOL_BODY_COMP)
	{
		ULONG  iDst =AD_KBYTES4;
		uncompress((Bytef *)sUnCompress, (ULONG*)&iDst, (Bytef *)pBody, (ULONG)m_stHead.iLength);
		pBody = sUnCompress;
		m_stHead.iLength = iDst;
	}
	
	pCur = pBody;
	while ((int)(pCur - pBody) < m_stHead.iLength)
	{
		if(usTotal >= m_stHead.usTotal)
			break;
		ucKey = *pCur;
		pCur += sizeof(UCHAR);
		
		kvLen= ntohl(*(int *)pCur);
		pCur += sizeof(int);
		
		iter = m_mParse.find(ucKey);
		if(iter != m_mParse.end())
		{
			switch((iter->second).type)
			{
				case AD_PROTOCOL_PARSE_TYPE_STRING:
					(static_cast<string*>((iter->second).pointer))->assign(const_cast<const char *>(pCur), kvLen);
					break;
				case AD_PROTOCOL_PARSE_TYPE_LIST_STRING:
					str.assign(const_cast<const char *>(pCur), kvLen);
					(static_cast<list<string> *>((iter->second).pointer))->push_back(str);
					break;
				default:
					break;
			}
		}
		pCur += kvLen ;
		usTotal ++;
	}

RETURN:

	ctRes.m_uiAdx = m_stHead.ucType;
	if(bPrint == true)
	{
		ctRes.display();
	}
	return ret;
}

int AdRtbProtocol::InternalToTlv(AdRtbRequest& ctReq, char * pdata,int iLen, bool bPrint)
{
	char* pCur;
	USHORT  usCount = 0;
	char sBuff[AD_KBYTES8];
	char* pSend = pdata;
	int dataLen;
	int iSpace;
	int headSize = AD_PROTOCOL_HEAD_LEN;
	
	m_stHead.ucType = ctReq.m_uiAdx;
	
	pCur = pSend + headSize;
	iSpace = iLen - headSize;

	
	if(!ctReq.m_sFlow.empty())													
	{																	
		*pCur = AD_PROTOCOL_KEY_ADIRECT_FLOW_TYPE;															
		pCur += sizeof(UCHAR);												
		*(int *)pCur =htonl (ctReq.m_sFlow.size()); 										
		pCur += sizeof(int);													
		memcpy(pCur,  ctReq.m_sFlow.c_str(),   ctReq.m_sFlow.size());									
		pCur += ctReq.m_sFlow.size();														
		usCount ++; 														
	}																		

	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADIRECT_FLOW_TYPE, ctReq.m_sFlow, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_RTB_BIDID, ctReq.m_sBidID, iSpace);
	AD_PROTOCOL_KV( AD_PROTOCOL_KEY_RTB_UID,  ctReq.m_sUserID, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_RTB_IP ,  ctReq.m_sUserIP, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_PID ,  ctReq.m_sPId, iSpace);
	if(!ctReq.m_ltSiteCategory.empty())
	{
		list<string>::iterator iter;
		for(iter = ctReq.m_ltSiteCategory.begin(); iter != ctReq.m_ltSiteCategory.end(); iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_MOBILE_APP,  (*iter), iSpace);
		}
	}
	AD_PROTOCOL_KV( AD_PROTOCOL_KEY_ADV_SIZE,  ctReq.m_sSize, iSpace);
	AD_PROTOCOL_KV( AD_PROTOCOL_KEY_RTB_USERAGENT,  ctReq.m_sUserAgent, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_RTB_USER_URL ,  ctReq.m_sUserUrl, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_DEVICE_UNIQUE_TYPE ,  ctReq.m_sDevUniqueType, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_VIEW_SCREEN, ctReq.m_sAdViewScreen, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_VIEW_TYPE, ctReq.m_sAdViewType, iSpace);

	if(!ctReq.m_ltCreate.empty())
	{
		list<string>::iterator iter;
		for(iter = ctReq.m_ltCreate.begin(); iter != ctReq.m_ltCreate.end();  iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_EXEC_FILTER, (*iter), iSpace);
		}
	}

	//CROWD_CATEGORY  for icache,  capture data
	if(!ctReq.m_ltCrowdCategory.empty())
	{
		list<string>::iterator iter;
		for(iter = ctReq.m_ltCrowdCategory.begin(); iter != ctReq.m_ltCrowdCategory.end();  iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_CROWD_CATEGORY, (*iter), iSpace);
		}
	}
	
	if(!ctReq.m_ltCategory.empty())
	{
		list<string>::iterator iter;
		for(iter = ctReq.m_ltCategory.begin(); iter != ctReq.m_ltCategory.end();  iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_AD_CATEGORY, (*iter), iSpace);
		}
	}

	if(!ctReq.m_ltUalist.empty())
	{
		list<string>::iterator iter;
		for(iter = ctReq.m_ltUalist.begin(); iter != ctReq.m_ltUalist.end();  iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_USERAGENT_ID, (*iter), iSpace);
		}
	}

	
	if(!ctReq.m_ltMaterialFormat.empty())
	{
		list<string>::iterator iter;
		for(iter = ctReq.m_ltMaterialFormat.begin(); iter != ctReq.m_ltMaterialFormat.end();  iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_MATERIAL_FORMAT, (*iter), iSpace);
		}
	}

	if(!ctReq.m_ltSensitive.empty())
	{
		list<string>::iterator iter;
		for(iter = ctReq.m_ltSensitive.begin(); iter != ctReq.m_ltSensitive.end();  iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_SENSITIVE_CATEGORY, (*iter), iSpace);
		}
	}
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_ADV_MIN_CPM_PRICE ,  ctReq.m_sMinPrice, iSpace);
	AD_PROTOCOL_KV( AD_PROTOCOL_KEY_TENCENT_DURATION_MIN,  ctReq.m_sVideoMinTime, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_TENCENT_DURATION_MAX,  ctReq.m_sVideoMaxTime, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_LINEARITY ,  ctReq.m_sLinearity, iSpace);
	
	if(!ctReq.m_ltMobileOperator.empty())
	{
		list<string>::iterator iter;
		for(iter = ctReq.m_ltMobileOperator.begin(); iter != ctReq.m_ltMobileOperator.end();	iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_MOBILE_OPERATOR, (*iter), iSpace);
		}
	}

	if(!ctReq.m_ltMobileDevice.empty())
	{
		list<string>::iterator iter;
		for(iter = ctReq.m_ltMobileDevice.begin(); iter != ctReq.m_ltMobileDevice.end();	iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_DEVICE_TYPE, (*iter), iSpace);
		}
	}

	if(!ctReq.m_ltMobileConnect.empty())
	{
		list<string>::iterator iter;
		for(iter = ctReq.m_ltMobileConnect.begin(); iter != ctReq.m_ltMobileConnect.end();	iter ++)
		{
			AD_PROTOCOL_KV(AD_PROTOCOL_KEY_MOBILE_CONNECT_TYPE, (*iter), iSpace);
		}
	}

	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_TRANSACTION_TYPE ,  ctReq.m_sTransaction, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_DEVICE_IDFA, ctReq.m_sIDFA, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_DEVICE_IMEI, ctReq.m_sIMEI, iSpace);
	AD_PROTOCOL_KV(AD_PROTOCOL_KEY_DEVICE_ANDROID, ctReq.m_sAndroidID, iSpace);

	dataLen= pCur - pSend - headSize;
	if(m_stHead.ucAttr == AD_PROTOCOL_BODY_COMP)
	{
		ULONG  iDst =AD_KBYTES4;
		compress((Bytef *)(sBuff ), (ULONG*)&iDst, 
			(const Bytef*)(pSend +headSize), (ULONG)dataLen);
		memcpy(pSend +headSize, sBuff, iDst);
		*(pSend + headSize + iDst) = '\0';
		dataLen = iDst;
	}

	m_stHead.iLength = dataLen;
	m_stHead.usTotal = usCount;

	pCur = pSend;
	
	*pCur = AD_PROTOCOL_FLAG;
	pCur += sizeof(UCHAR);
	
	*pCur = m_stHead.ucVersion << 4;
	*pCur |= m_stHead.ucProtocol << 3;
	*pCur |= m_stHead.ucWide << 2;
	*pCur |= m_stHead.ucResponse << 1;
	*pCur |= m_stHead.ucExpand;
	pCur += sizeof(UCHAR);
	
	*pCur = m_stHead.ucType;
	pCur += sizeof(UCHAR);
	
	*pCur = m_stHead.ucVerb;
	pCur += sizeof(UCHAR);
	
	*pCur = m_stHead.ucAttr;
	pCur += sizeof(UCHAR);
	
	*pCur = m_stHead.ucResult;
	pCur += sizeof(UCHAR);
	
	*(USHORT*)pCur = htons(m_stHead.usTotal);
	pCur += sizeof(USHORT);
	
	*(int*)pCur = htonl(m_stHead.iLength);
	pCur += sizeof(int);

	dataLen += AD_PROTOCOL_HEAD_LEN;
	if (AD_PROTOCOL_EXPPAND_1 == m_stHead.ucExpand)
	{
		memcpy(pCur, m_sExHead.c_str(), m_sExHead.size());
		dataLen += m_sExHead.size();
	}

	if(bPrint == true)
	{
		ctReq.display();
	}

	return dataLen;
}
