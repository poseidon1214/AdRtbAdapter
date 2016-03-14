/***************************************
*	Description: Ad filter:  mark,  get date from redis which store people label,  
*	Author: wilson
*	Date: 2015/08/07
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_FILTER_MARK_H
#define _AD_FILTER_MARK_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <algorithm> 
#include "AdGlobal.h"
#include "AdMap.h"
#include "AdAcbm.h"
#include "AdUserInfo.h"

using namespace std;

#define  AD_MARK_REMAIN_DAYS                                 30

class AdAudienceInfo
{
public:
	AdAudienceInfo()
	{
		clear();
	};
	virtual ~AdAudienceInfo()
	{
		clear();
	};

	void clear()
	{
		m_uiType = 0;
		m_uiEffectiveTime = 0;
		m_uiVisitMask = 0;
		m_uiVisitMatch = 0;
		m_uiUnVisitMask = 0;
		m_uiUnVisitMatch = 0;
		m_uiSearchMask = 0;
		m_uiSearchMatch = 0;
		m_pData = NULL;
	};

	UINT m_uiType;
	UINT m_uiEffectiveTime;
	UINT m_uiVisitMask;
	UINT m_uiVisitMatch;
	UINT m_uiUnVisitMask;
	UINT m_uiUnVisitMatch;
	UINT m_uiSearchMask;
	UINT m_uiSearchMatch;
	void * m_pData;
};

class AdMarkInfo
{
public:
	AdMarkInfo()
	{
		clear();
	};
	~AdMarkInfo()
	{
		clear();
	};
	void clear()
	{
		m_uiType = 0;
		m_sKey.clear();
		m_pData = NULL;
	};
	UINT  m_uiType;
	string m_sKey;
	void * m_pData;
};


class AdFilterMark:public AdMap
{
public:
	AdFilterMark();
	virtual ~AdFilterMark();
	int AdFilterMarkInit(map<string, string>&  mCache);
	int AdFilterMarkInit(AdFilterMark *  pMark);
	virtual int match(void * data, AD_RESULT & mResult);
	virtual int AddData(void * data);
	virtual string & name()
	{
		return m_sName;
	};

	map<string, string>  m_mSerever;

private:
	string m_sName;

	//user info, get marks 
	AdUserInfo   m_ctUser;
	
	//acbm tree
	AdAcbm   m_ctAcbm;
	//Mark
	list<AdMarkInfo * >  m_ltMark;
	int AdFilterMarkMatchMark(AdUserData& ctMark,  set<void *> & setReady, AD_RESULT & mResult);
	int AdFilterMarkGetStr(string & sStr);
	int AdFilterMarkAddMark(AdPlanningUnit * pUnit);


	//audience
	list<AdAudienceInfo * >  m_ltAudience;
	int AdFilterMarkAddAudience(AdPlanningUnit * pUnit);
	int AdFilterMarkMatchAudience(AdUserData & ctMark,  AD_RESULT &  mReady, AD_RESULT & mResult);

	// vip user
	int AdFilterMarkVipUserTag(AdUserData& ctMark, AD_RESULT & mResult);

	UINT m_uiStrMinLen;
	UINT m_uiStrMaxLen;
};

#endif

