/***************************************
*	Description:  user info: included  marks 
*	Author: wilson
*	Date: 2015/08/10
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdUserInfo.h"
#include "AdUdp.h"
#include "AdRtbNetProtocol.h"
#define AD_TLV_HEAD_LEN    12

#define	AD_PROTOCOL_FLAG				0xe8

/* verb */
#define	AD_CACHE_CREATE			0x01		// create
#define	AD_CACHE_DESTROY			0x02		// destroy
#define	AD_CACHE_SELECT			0x03		// select
#define	AD_CACHE_INSERT			0x04		// insert
#define	AD_CACHE_DELETE			0x05		// delete
#define	AD_CACHE_UPDATE 			0x06		// update


#define	AD_CONSISTENCY_HASH			200


AdUserInfo::AdUserInfo()
{
	m_pHash = NULL;
}

AdUserInfo::~AdUserInfo()
{
	list<struct node_s*>::iterator   iter;
	
	if(m_pHash != NULL)
	{
		conhash_fini(m_pHash);
		for(iter = m_ltNode.begin(); iter != m_ltNode.end(); iter ++)
		{
			free(*iter);
		}
		m_pHash = NULL;
		m_ltNode.clear();
	}
}

int AdUserInfo::AdUserInfoInit(map<string, string>&  mCache)
{
	int iReturn = AD_SUCCESS;
	struct node_s* pNode = NULL;
	map<string, string>::iterator   iter;

	if (NULL != (m_pHash = conhash_init(NULL))) 
	{
		for(iter = mCache.begin(); iter != mCache.end(); iter ++)
		{
			pNode = (struct node_s*)malloc(sizeof(struct node_s));
			if (NULL != pNode)
			{
				conhash_set_node(pNode, (iter->first).c_str(), AD_CONSISTENCY_HASH, 
						(void *)((iter->second).c_str()));
				conhash_add_node(m_pHash, pNode);
				m_ltNode.push_back(pNode);
			}
			else 
			{
				iReturn = AD_FAILURE;
				break;
			}
		}
	}
	else 
	{
		iReturn = AD_FAILURE;
	}

	return iReturn;
}

#define	AD_PROTOCOL_KV(K, V, L)		do {						\
	if(!V.empty())														\
	{																	\
		if(L < (int)(sizeof(UCHAR) + sizeof(int) +  V.size()))							\
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



int AdUserInfo::AdUserInfoBuildRequest(AdUserData & ctUser, char* pPacket, int iLen)
{
	char* pCur;
	int iSpace;
	USHORT  usCount = 0;
	int len;
	int headSize = AD_PROTOCOL_HEAD_LEN;

	pCur = pPacket + headSize;
	iSpace = iLen - headSize;

	//league
	AD_PROTOCOL_KV(AD_CACHE_LEAGUE,  ctUser.m_sLeague, iSpace);
	/* user id */
	AD_PROTOCOL_KV(AD_CACHE_USER_ID, ctUser.m_sUserID, iSpace);
	/* user ip */
	AD_PROTOCOL_KV(AD_CACHE_USER_IP, ctUser.m_sUserIP, iSpace);
	//user agent
	AD_PROTOCOL_KV(AD_CACHE_USERAGENT, ctUser.m_sUserAg, iSpace);

	len = pCur - pPacket - headSize;

	pCur = pPacket;
	
	*pCur = AD_PROTOCOL_FLAG;
	pCur += sizeof(UCHAR);
	
	*pCur = 1 << 4;
	*pCur |= 1 << 3;
	*pCur |= 0;
	*pCur |= 1 << 1;
	*pCur |= 0;
	pCur += sizeof(UCHAR);
	
	*pCur = 0;
	pCur += sizeof(UCHAR);
	
	*pCur = AD_CACHE_SELECT;
	pCur += sizeof(UCHAR);
	
	*pCur = 0;
	pCur += sizeof(UCHAR);
	
	*pCur = 0;
	pCur += sizeof(UCHAR);
	
	*(USHORT*)pCur = htons(usCount);
	pCur += sizeof(USHORT);
	
	*(int*)pCur = htonl(len);
	pCur += sizeof(int);

	return  len + headSize;

}

void AdUserInfo::AdUserInfoAddMark(string &sStr, multimap<UINT,  AdUserMark, greater<UINT> >& mMark, UCHAR ucTag)
{
	AdUserMark ctMark;
	vector<string> vStr1;
	vector<string> vStr2;
	string sNode;
	vector<string>::iterator  iter;
	Time tNow;
	time_t tCapture;

	AdString::StrSplit(sStr, vStr1, "|");
	if(vStr1.empty())
	{
		return;
	}

	for(iter = vStr1.begin(); iter != vStr1.end(); iter ++)
	{
		sNode = *iter;
		AdString::StrSplit(sNode, vStr2, ":");
		if(vStr2.size() == 2)
		{
			tCapture = atoi(vStr2[1].c_str());
			if(tCapture < tNow.getSecond())
			{
				ctMark.m_ucTag = ucTag;
				ctMark.m_uiTime = ((tNow.getSecond()- tCapture)/AD_DAY_SECONDS)  + 1;
				ctMark.m_sMark = vStr2[0];
				mMark.insert(pair<UINT,  AdUserMark>(ctMark.m_uiTime,ctMark));
			}
		}
		vStr2.clear();
	}

	return;
}


void AdUserInfo::AdUserInfoAddRequestMark(string &sStr, multimap<UINT,  AdUserMark, greater<UINT> >& mMark, UCHAR ucTag)
{
	AdUserMark ctMark;

	ctMark.m_ucTag = ucTag;
	ctMark.m_uiTime = 1;
	ctMark.m_sMark = sStr;
	mMark.insert(pair<UINT,  AdUserMark>(ctMark.m_uiTime,ctMark));

	return;
}


int AdUserInfo::AdUserInfoParse(AdUserData & ctUser, char* pPacket, int iLen, int iMaxLen)
{
	USHORT usTotal = 0;
	int kvLen;
	char * pBody;
	char * pCur;
	ADPROHEAD_S stHead;
	UCHAR ucKey;
	string sStr;

	memset(&stHead, 0, sizeof(stHead));

	pCur = pPacket;
	
	stHead.ucFlag = *pCur;
	if (AD_PROTOCOL_FLAG == stHead.ucFlag ) 
	{
		pCur += sizeof(UCHAR);
	}
	else 
	{
		return AD_FAILURE;
	}
	stHead.ucVersion = *pCur >> 4;
	stHead.ucProtocol = (*pCur & 0x0f) >> 3;
	stHead.ucWide = (*pCur & 0x07) >> 2;
	stHead.ucResponse = (*pCur & 0x03) >> 1;
	stHead.ucExpand = *pCur & 0x01;
	pCur += sizeof(UCHAR);

	stHead.ucType = *pCur;
	pCur += sizeof(UCHAR);

	stHead.ucVerb = *pCur;
	pCur += sizeof(UCHAR);

	stHead.ucAttr = *pCur;
	pCur += sizeof(UCHAR);

	stHead.ucResult = *pCur;
	pCur += sizeof(UCHAR);

	stHead.usTotal = ntohs(*(USHORT*)pCur);
	pCur += sizeof(USHORT);

	stHead.iLength = ntohl(*(int*)pCur);
	pCur += sizeof(int);

	pBody = pCur;
	
	if( iLen - stHead.iLength != AD_TLV_HEAD_LEN)
	{
		AD_ERROR("Length Error: %d - %d\n", iLen, stHead.iLength);
		return AD_FAILURE;
	}

	if (stHead.usTotal == 0)
	{
		//AD_ERROR("No result\n");
		return AD_FAILURE;
	}

	
	while ((int)(pCur - pBody + 1) < stHead.iLength)
	{
		if(usTotal >= stHead.usTotal)
			break;
		ucKey = *pCur;
		pCur += sizeof(UCHAR);
			
		kvLen= ntohl(*(int *)pCur);
		pCur += sizeof(int);
		if(kvLen <= 0)
			break;
		if((int)(pCur - pPacket + 1 + kvLen)> iLen)
			break;
		switch (ucKey) 
		{
			case AD_CACHE_PID:
				ctUser.m_sPID.assign(const_cast<const char *>(pCur), kvLen);
				break;						
			case AD_CACHE_CREATE_ID:
				ctUser.m_sCreativeID.assign(const_cast<const char *>(pCur), kvLen);
				break;						
			case AD_CACHE_EXEC_ID:
				ctUser.m_sExecuteID.assign(const_cast<const char *>(pCur), kvLen);
				break;	
			case AD_CACHE_LEAGUE:
				ctUser.m_sLeague.assign(const_cast<const char *>(pCur), kvLen);
				break;	
			case AD_CACHE_ADIRECTS_ID:
				ctUser.m_sUserID.assign(const_cast<const char *>(pCur), kvLen);
				break;	
			case AD_CACHE_COOKIE_ID:
				ctUser.m_sCookID.assign(const_cast<const char *>(pCur), kvLen);
				break;	
			case AD_CACHE_USER_IP:
				ctUser.m_sUserIP.assign(const_cast<const char *>(pCur), kvLen);
				break;	
			case AD_CACHE_USERAGENT:
				ctUser.m_sUserAg.assign(const_cast<const char *>(pCur), kvLen);
				break;	
			case AD_CACHE_KEY:
				sStr.assign(const_cast<const char *>(pCur), kvLen);
				AdUserInfoAddMark(sStr, ctUser.m_mMark, static_cast<UCHAR>(AD_CACHE_KEY));
				if(0 == sStr.find("KW_"))
				{
					AdUserInfoAddMark(sStr, ctUser.m_mAudience, static_cast<UCHAR>(AD_CACHE_TAG));
				}
				break;
			case AD_CACHE_URL:
				sStr.assign(const_cast<const char *>(pCur), kvLen);
				AdUserInfoAddMark(sStr, ctUser.m_mMark, static_cast<UCHAR>(AD_CACHE_URL));
				break;
			case AD_CACHE_TAG:
				sStr.assign(const_cast<const char *>(pCur), kvLen);
				AdUserInfoAddMark(sStr, ctUser.m_mMark, static_cast<UCHAR>(AD_CACHE_TAG));
				AdUserInfoAddMark(sStr, ctUser.m_mAudience, static_cast<UCHAR>(AD_CACHE_TAG));
				break;
			case AD_CACHE_COOKIE_VALUE:
				sStr.assign(const_cast<const char *>(pCur), kvLen);
				AdUserInfoAddMark(sStr, ctUser.m_mAudience, static_cast<UCHAR>(AD_CACHE_TAG));
				break;
			case AD_CACHE_FLAG:
				sStr.assign(const_cast<const char *>(pCur), kvLen);		
				ctUser.m_uiFlagID= atoi(sStr.c_str());
				break;
			default:
				goto RETURN;
				break;
		}				
		pCur += kvLen ;
		usTotal ++;
	}

RETURN:
	return AD_SUCCESS;
}


int AdUserInfo::AdUserInfoGet(AdUserData & ctUser)
{
	AdUdp cUdp;

	int iLength;

	char usRecv[AD_CACHE_REV_MAX];
	char usSend[AD_KBYTES8];
	
	string sUrl;
	vector<string> vStr;
	
	char szKey[AD_BYTE256];

	const struct node_s* pNode ;

	snprintf(szKey, AD_BYTE256, "%s+%s", ctUser.m_sLeague.c_str(), ctUser.m_sUserID.c_str());

	pNode = conhash_lookup(m_pHash, szKey);
	if (NULL == pNode) 
	{
		return AD_FAILURE;
	}
	
	sUrl = (char *)pNode->value;
	
	//AD_PRINT("iCacheServer:%s\n", sUrl.c_str());
	
	AdString::StrSplit(sUrl,vStr, ":");
	if(vStr.empty())
		return AD_FAILURE;

	if (AD_SUCCESS == cUdp.UdpInit(const_cast<char *>(vStr[0].c_str()), atoi(vStr[1].c_str())))
	{
		//CUdp.GtNetSetTimeOut(0, g_stConf.m_uiTime);
		iLength = AdUserInfoBuildRequest(ctUser, usSend, AD_KBYTES8);
		if (iLength <=  0 )
		{
			return AD_FAILURE;
		}
		
		if (cUdp.UdpSend(usSend, iLength) <= 0) 
		{
			return AD_FAILURE;
		}

		iLength = cUdp.UdpRev(usRecv, AD_CACHE_REV_MAX, 2000);
		if (iLength <= 0)
		{
			return AD_FAILURE;
		}
		
		if (AD_SUCCESS == AdUserInfoParse(ctUser, usRecv, iLength, AD_CACHE_REV_MAX)) 
		{
			return  AD_SUCCESS;
		}
	}

	return AD_FAILURE;
}
