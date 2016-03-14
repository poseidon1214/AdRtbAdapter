/***************************************
*	Description:  user info: included  marks 
*	Author: wilson
*	Date: 2015/08/10
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#ifndef  __AD_USER_INFO_H_
#define __AD_USER_INFO_H_
#include <map>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include "AdGlobal.h"
#include "conhash.h"
#include "AdUtil.h"


using namespace std;

#define	 AD_CACHE_LEAGUE			0x01
#define	 AD_CACHE_USER_ID			0x02
#define	 AD_CACHE_USER_IP			0x03
#define	 AD_CACHE_USERAGENT		    0x04
#define	 AD_CACHE_KEY				0x05
#define	 AD_CACHE_URL				0x06
#define  AD_CACHE_TAG				0x07	// tag
#define  AD_CACHE_FLAG				0x08
#define  AD_CACHE_USER_IE			0x09
#define  AD_CACHE_ADIRECTS_ID		0x0a
#define  AD_CACHE_COOKIE_ID		    0x0b	// cookie id
#define  AD_CACHE_COOKIE_VALUE	    0x0c	// cookie value
#define  AD_CACHE_PID				0x0d	// spid
#define  AD_CACHE_EXEC_ID			0x0e	// eid
#define  AD_CACHE_CREATE_ID			0x0f	// create id

class AdUserMark
{
public:
	AdUserMark()
	{
		clear();
	};
	virtual ~AdUserMark()
	{
		clear();
	};

	void clear()
	{
		m_sMark.clear();
		m_uiTime = 0;
		m_ucTag = 0;
	}
	string m_sMark;
	UINT m_uiTime;
	UCHAR m_ucTag;
};

class  AdUserData
{
public:
	AdUserData()
	{
		clear();
	};
	virtual  ~AdUserData()
	{
		clear();
	};

	void clear()
	{
		m_uiFlagID = 0;
		m_sAdUID.clear();
		m_sLeague.clear();
		m_sUserID.clear();
		m_sUserIE.clear();
		m_sUserIP.clear();
		m_sUserAg.clear();
		m_sCookID.clear();
		m_sPID.clear();
		m_sCreativeID.clear();
		m_sExecuteID.clear();
		m_mMark.clear();
		m_mAudience.clear();
	};
	UINT m_uiFlagID;
	string m_sAdUID;
	string m_sLeague;
	string m_sUserID;
	string m_sUserIE;
	string m_sUserIP;
	string m_sUserAg;
	string m_sCookID;
	string m_sPID;
	string m_sCreativeID;
	string m_sExecuteID;
	multimap<UINT,  AdUserMark, greater<UINT> >  m_mMark;
	multimap<UINT,  AdUserMark, greater<UINT> > m_mAudience;
};


#define AD_CACHE_REV_MAX                   (AD_KBYTES8 * 10)
class AdUserInfo
{
public:
	AdUserInfo();
	virtual ~AdUserInfo();
	int AdUserInfoGet(AdUserData & ctUser);
	int AdUserInfoInit(map<string, string>&  mCache);
	void AdUserInfoAddRequestMark(string &sStr, multimap<UINT,  AdUserMark, greater<UINT> >& mMark, UCHAR ucTag);

private:	
	struct conhash_s * m_pHash;
	list<struct node_s*>  m_ltNode;

	int AdUserInfoBuildRequest(AdUserData & ctUser, char* pPacket, int iLen);
	void AdUserInfoAddMark(string &sStr, multimap<UINT,  AdUserMark, greater<UINT> >& mMark, UCHAR ucTag);
	int AdUserInfoParse(AdUserData & ctUser, char* pPacket, int iLen, int iMaxLen);
	
};

#endif

