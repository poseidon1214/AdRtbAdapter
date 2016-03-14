/***************************************
*	Description: Ad filter:  mark,  get date from redis which store people label,  
*	Author: wilson
*	Date: 2015/08/07
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#include <stdlib.h>	
#include "AdFilterMark.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"

AdFilterMark::AdFilterMark()
{
	m_sName = "Mark";
}

AdFilterMark::~AdFilterMark()
{
	//clear Audience Info
	list<AdAudienceInfo *>::iterator ltItAudi;
	for(ltItAudi = m_ltAudience.begin(); ltItAudi != m_ltAudience.end(); ltItAudi ++)
	{
		delete *ltItAudi;
	}
	m_ltAudience.clear();

	//clear Mark Info
	list<AdMarkInfo *>::iterator ltItMark;
	for(ltItMark = m_ltMark.begin(); ltItMark != m_ltMark.end(); ltItMark ++)
	{
		delete *ltItMark;
	}
	m_ltMark.clear();
}

int AdFilterMark::AdFilterMarkInit(map<string, string>&  mCache)
{
	m_mSerever = mCache;
	return m_ctUser.AdUserInfoInit( m_mSerever);
}

int AdFilterMark::AdFilterMarkInit(AdFilterMark *  pMark)
 {
 	m_mSerever = pMark->m_mSerever;
	return m_ctUser.AdUserInfoInit( m_mSerever);
}



/**************marks   operater**********************/

int AdFilterMark::AdFilterMarkMatchMark(AdUserData& ctMark,  set<void *> & setReady, AD_RESULT & mResult)
{	
	//acbm result info
	map<void *, list<string> >  mMatch;
	map<void *, list<string> >::iterator  mItMatch;
	list<string>::iterator  lsIter;
	set<void *> setMatch;
	
	 //user info
	multimap<UINT,  AdUserMark>::iterator  mItMark;
	set<void *> setMerge;
	insert_iterator<set<void *, less<void *> > >  res_ins(setMerge, setMerge.begin());

	AdMarkInfo * pInfo;
	if(setReady.empty())
		return AD_SUCCESS;

	//match marks
	for(mItMark = ctMark.m_mMark.begin(); mItMark != ctMark.m_mMark.end();  mItMark ++)
	{
		m_ctAcbm.AdAcbmSearch(mItMark->second.m_sMark, mMatch);
		if(mMatch.empty())
			continue;

		//get fitable mark 
		for(mItMatch = mMatch.begin(); mItMatch != mMatch.end(); mItMatch ++)
		{
			pInfo = static_cast<AdMarkInfo *>(mItMatch->first);
			if((pInfo->m_uiType == AD_KEYS_ACC_CUS)
				&&(pInfo->m_sKey.size() == mItMark->second.m_sMark.size()))
			{
				setMatch.insert(pInfo->m_pData);
				continue;
			}
			else if(pInfo->m_uiType == AD_KEYS_DIM_CUS)
			{
				setMatch.insert(pInfo->m_pData);
				continue;
			}
			else if((mItMark->second.m_ucTag == AD_CACHE_TAG)
					&& ((pInfo->m_uiType == AD_MARK_PUBLIC) || (pInfo->m_uiType == AD_MARK_PRIVATE)))
			{
				setMatch.insert(pInfo->m_pData);
				continue;
			}
			else
			{
				continue;
			}
			#if 0
							if( 0 == stMatch[iCount].m_ulOffset && \
								uiHot > strlen(pstInfo->m_szHot) && \
								pstHot->m_szHot[strlen(pstInfo->m_szHot)] == GT_RH_SPLT){
								#ifdef _GT_DEBUG_
									GT_PRINT("Match Acc Space After \n");
								#endif								
								iReturn = GT_SUCCESS;
							}
							else if( 0 < stMatch[iCount].m_ulOffset && \
								uiHot > strlen(pstInfo->m_szHot) && \
								pstHot->m_szHot[stMatch[iCount].m_ulOffset - 1] == GT_RH_SPLT){
								#ifdef _GT_DEBUG_
									GT_PRINT("Match Acc Space Before \n");
								#endif
								iReturn = GT_SUCCESS;
							}
			#endif
		}
		if(setMatch.empty())
			continue;
		//match result,  get hit order
		set_intersection(setReady.begin(), setReady.end(),setMatch.begin(), setMatch.end(), res_ins);
		if((!setMerge.empty()) &&( setMerge.size() >0 ))
		{
			//search hit order, put result for result
			set<void *>::iterator setIter;
			for(setIter = setMerge.begin(); setIter != setMerge.end(); setIter ++)
			{
				AdRtbFilterResult & ctResult = mResult[*setIter];
				mItMatch = mMatch.find(*setIter);
				if(mItMatch == mMatch.end())
					continue;
				for(lsIter = mItMatch->second.begin();lsIter != mItMatch->second.end();lsIter ++)
				{
					AdFilterMarkInfo  sInfo;
					sInfo.m_uiType = mItMark->second.m_ucTag;
					sInfo.m_sMark = *lsIter;
					ctResult.m_ltMarkInfo.push_back(sInfo);
				}
			}
		}
		setMerge.clear();
		mMatch.clear();
		setMatch.clear();
	}
	
	return AD_SUCCESS;
}

int AdFilterMark::AdFilterMarkGetStr(string & sStr)
{
	string::iterator it = sStr.begin();
	string http = "http://";
	string https = "https://";
	
	while(it != sStr.end())
	{
		if( *it == '*' )
		{
			sStr.erase(it);
			continue;
		}
		else
		{
			it ++;
		}
	}

	if(0 == sStr.find(http))
	{
		sStr.erase(0, http.size());
	}

	if(0 == sStr.find(https))
	{
		sStr.erase(0, https.size());
	}
	return AD_SUCCESS;
}

int AdFilterMark::AdFilterMarkAddMark(AdPlanningUnit * pUnit)
{
	list<AdMarkNode>::iterator iter;
	
	int ret  = AD_SUCCESS;
	AdMarkInfo  * pMark;

	for(iter = pUnit->m_ltKeyWord.begin(); iter != pUnit->m_ltKeyWord.end(); iter ++)
	{
		pMark = new AdMarkInfo;
		if(pMark == NULL)
		{
			AD_ERROR("mem not enough\n");
			break;
		}
		pMark->m_sKey= (*iter).m_sKey;
		pMark->m_uiType = (*iter).m_uiType;
		pMark->m_pData = static_cast<void *>(pUnit);
		AdFilterMarkGetStr(pMark->m_sKey);
		m_ltMark.push_back(pMark);
		ret = m_ctAcbm.AdAcbmTreeAdd(pMark->m_sKey,  static_cast<void *>( pMark));
		if(ret != AD_SUCCESS)
			break;
	}
	return ret;
}

/**************audience   operater**********************/

int AdFilterMark::AdFilterMarkAddAudience(AdPlanningUnit * pUnit)
{
	list<AdAudienceNode>::iterator laIter;
	map<string, UINT>::iterator  mIter;

	AdAudienceInfo *  pInfo;
	int ret  = AD_SUCCESS;
	string  sStr;
	UINT i;
	UINT uiSearchMask;
	UINT uiVisitMask;
	UINT uiUnVisitMask;

	if(empty())
	{
		m_uiStrMinLen = 0xffff;
		m_uiStrMaxLen = 0;
	}

	for(laIter = pUnit->m_ltAudience.begin(); laIter != pUnit->m_ltAudience.end(); laIter ++)
	{
		uiSearchMask = 0;
		uiVisitMask = 0;
		uiUnVisitMask = 0;
		for(i = 0; i <  (*laIter).m_mSearch.size(); i ++)
		{
			if(i >= (sizeof(UINT) * 8))
				break;
			uiSearchMask |= 1 << i;
		}
		for(i = 0; i <  (*laIter).m_mVisit.size(); i ++)
		{
			if(i >= (sizeof(UINT) * 8))
				break;
			uiVisitMask |= 1 << i;
		}
		for(i = 0; i <  (*laIter).m_mUnVisit.size(); i ++)
		{
			if(i >= (sizeof(UINT) * 8))
				break;
			uiUnVisitMask |= 1 << i;
		}
		
		i = 0;
		for(mIter = (*laIter).m_mSearch.begin(); mIter != (*laIter).m_mSearch.end(); mIter ++)
		{
			if(i >= (sizeof(UINT) * 8))
				break;
			pInfo = new AdAudienceInfo;
			if(pInfo == NULL)
			{
				AD_ERROR("mem not enough\n");
				break;
			}

			pInfo->m_uiType = (*laIter).m_uiType;
			pInfo->m_uiEffectiveTime = mIter->second;
			pInfo->m_uiSearchMatch = 1 << i;
			pInfo->m_uiSearchMask= uiSearchMask;
			pInfo->m_uiVisitMask= uiVisitMask;
			pInfo->m_uiUnVisitMask = uiUnVisitMask;
			pInfo->m_pData = static_cast<void *>( pUnit);
			m_ltAudience.push_back(pInfo);
			sStr = mIter->first;
			update(sStr, static_cast<void *>(pInfo));
			if((mIter->first).size() < m_uiStrMinLen)
				m_uiStrMinLen = (mIter->first).size();
			if((mIter->first).size() > m_uiStrMaxLen)
				m_uiStrMaxLen = (mIter->first).size();
			i ++;
		}
		
		i = 0;
		for(mIter = (*laIter).m_mVisit.begin(); mIter != (*laIter).m_mVisit.end(); mIter ++)
		{
			if(i >= (sizeof(UINT) * 8))
				break;
			pInfo = new AdAudienceInfo;
			if(pInfo == NULL)
			{
				AD_ERROR("mem not enough\n");
				break;
			}

			pInfo->m_uiType = (*laIter).m_uiType;
			pInfo->m_uiEffectiveTime = mIter->second;
			pInfo->m_uiVisitMatch= 1 << i;
			pInfo->m_uiSearchMask= uiSearchMask;
			pInfo->m_uiVisitMask= uiVisitMask;
			pInfo->m_uiUnVisitMask = uiUnVisitMask;
			pInfo->m_pData = static_cast<void *>( pUnit);
			m_ltAudience.push_back(pInfo);
			sStr = mIter->first;
			update(sStr, static_cast<void *>(pInfo));
			if((mIter->first).size() < m_uiStrMinLen)
				m_uiStrMinLen = (mIter->first).size();
			if((mIter->first).size() > m_uiStrMaxLen)
				m_uiStrMaxLen = (mIter->first).size();
			i ++;
		}
		
		i = 0;
		for(mIter = (*laIter).m_mUnVisit.begin(); mIter != (*laIter).m_mUnVisit.end(); mIter ++)
		{
			if(i >= (sizeof(UINT) * 8))
				break;
			pInfo = new AdAudienceInfo;
			if(pInfo == NULL)
			{
				AD_ERROR("mem not enough\n");
				break;
			}

			pInfo->m_uiType = (*laIter).m_uiType;
			pInfo->m_uiEffectiveTime = mIter->second;
			pInfo->m_uiUnVisitMatch= 1 << i;
			pInfo->m_uiSearchMask= uiSearchMask;
			pInfo->m_uiVisitMask= uiVisitMask;
			pInfo->m_uiUnVisitMask = uiUnVisitMask;
			pInfo->m_pData = static_cast<void *>( pUnit);
			m_ltAudience.push_back(pInfo);
			sStr = mIter->first;
			update(sStr, static_cast<void *>(pInfo));
			if((mIter->first).size() < m_uiStrMinLen)
				m_uiStrMinLen = (mIter->first).size();
			if((mIter->first).size() > m_uiStrMaxLen)
				m_uiStrMaxLen = (mIter->first).size();
			i ++;
		}
	}
	return ret;
}

int AdFilterMark::AdFilterMarkMatchAudience(AdUserData & ctMark,  AD_RESULT &  mReady, AD_RESULT & mResult)
{
	map<UINT,  AdUserMark>::iterator  mItMark;
	
	map<void *,  AdAudienceInfo>  mMatch;
	multimap<void *,  AdAudienceInfo>::iterator  mItMatch;
	
	AD_ITER  adIter;

	list<void *>::iterator lItFind;

	AdAudienceInfo *  pInfo;

	string sStr;

	UINT i;

	if(mReady.empty())
		return AD_SUCCESS;
	
	//match marks
	for(mItMark = ctMark.m_mAudience.begin(); mItMark != ctMark.m_mAudience.end();  mItMark ++)
	{
		for(i = m_uiStrMinLen; i < mItMark->second.m_sMark.size(); i ++)
		{
			if(i > m_uiStrMaxLen)
				break;
			sStr.assign(mItMark->second.m_sMark.c_str(), i);
			adIter = find(sStr);
			if(adIter != end())
			{
				for(lItFind = adIter->second.begin(); lItFind != adIter->second.end(); lItFind ++)
				{
					pInfo = static_cast<AdAudienceInfo *>(*lItFind);
					if(pInfo->m_uiEffectiveTime <  mItMark->second.m_uiTime)
						continue;
	
					AdAudienceInfo & ctInfo = mMatch[pInfo->m_pData];

					ctInfo.m_uiType = pInfo->m_uiType;
					ctInfo.m_uiSearchMask = pInfo->m_uiSearchMask;
					ctInfo.m_uiSearchMatch |= pInfo->m_uiSearchMatch;
					ctInfo.m_uiUnVisitMask = pInfo->m_uiUnVisitMask;
					ctInfo.m_uiUnVisitMatch |= pInfo->m_uiUnVisitMatch;
					ctInfo.m_uiVisitMask = pInfo->m_uiVisitMask;
					ctInfo.m_uiVisitMatch |= pInfo->m_uiVisitMatch;

					ctInfo.m_pData = pInfo->m_pData;
				}
			}
		}
	}

	
	mItMatch = mMatch.begin(); 
	while(mItMatch != mMatch.end())
	{
		if(mItMatch->second.m_uiType == AD_AUDIENCE_MERGE_AND)
		{
			if((mItMatch->second.m_uiSearchMask == mItMatch->second.m_uiSearchMatch)  
				&& (mItMatch->second.m_uiVisitMask == mItMatch->second.m_uiVisitMatch)   
				&&(0  == mItMatch->second.m_uiUnVisitMatch))
			{
				if(mReady.find(mItMatch->second.m_pData) != mReady.end())
				{
					AdRtbFilterResult  ctRes;
					mResult[mItMatch->second.m_pData] = ctRes;
				}
				mItMatch ++;
			}
			else
			{
				mMatch.erase(mItMatch ++);
			}
		}
		else if(mItMatch->second.m_uiType == AD_AUDIENCE_MERGE_OR)
		{
			if((0 != mItMatch->second.m_uiSearchMatch)
				||  (0 != mItMatch->second.m_uiUnVisitMatch)
				|| (mItMatch->second.m_uiUnVisitMatch == 0))
			{
				if(mReady.find(mItMatch->second.m_pData) != mReady.end())
				{
					AdRtbFilterResult  ctRes;
					mResult[mItMatch->second.m_pData] = ctRes;
				}
				mItMatch ++;
			}
			else
			{
				mMatch.erase(mItMatch ++);
			}
		}
		else
		{
			mMatch.erase(mItMatch ++);
		}
	}
	return AD_SUCCESS;
}

/**************base   operater**********************/

int AdFilterMark::match(void * data, AD_RESULT & mResult)
{
	//Unit info
	AD_RESULT_ITER mItRes;			
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);
	AdPlanningUnit * pUnit;
	set<void *> setMark;
	AD_RESULT  mAudience;
	
	 //user info
	AdUserData  ctData;
	

	char buff[AD_BYTE128];
	int ret;

	//init user info
	ctData.m_sUserID = pRequest->m_sUserID;
	ctData.m_sUserIE = pRequest->m_sUserID;
	ctData.m_sUserIP = pRequest->m_sUserIP;
	sprintf(buff, "%d",pRequest->m_uiAdx);
	ctData.m_sLeague = buff;
	if(atoi(pRequest->m_sFlow.c_str()) != AD_PROTOCOL_FLOW_MOBILE)
	{
		ctData.m_sUserAg = pRequest->m_sUserAgent;
	}

	//get marks order
	mItRes = mResult.begin();
	while(mItRes != mResult.end())
	{	
		pUnit=static_cast<AdPlanningUnit *>(mItRes->first);
			
		if (pUnit->m_stOrder.m_uiNice == AD_ORDER_NICE) 
		{
			setMark.insert(mItRes->first);
			mResult.erase(mItRes ++);
		}
		else if (pUnit->m_stOrder.m_uiNice == AD_ORDER_BASE)
		{
			mItRes ++;
		}
		else if(pUnit->m_stOrder.m_uiNice == AD_ORDER_AUDIENCE)
		{
			mAudience[mItRes->first] = mItRes->second;;
			mResult.erase(mItRes ++);
		}
		else
		{
			mResult.erase(mItRes ++);
		}
	}

	//get user marks
	ret = m_ctUser.AdUserInfoGet(ctData);
	if(ret != AD_SUCCESS)
	{
		// add adx request marks
		if ( pRequest->m_ltCrowdCategory.empty() )
		{
			return AD_FAILURE;
		}
		else
		{
			list<string>::iterator iter;
			for( iter = pRequest->m_ltCrowdCategory.begin(); iter != pRequest->m_ltCrowdCategory.end(); iter++)
			{
				m_ctUser.AdUserInfoAddRequestMark( (*iter), ctData.m_mMark, static_cast<UCHAR>(AD_CACHE_TAG));
			}
		}
	}
	
	if(pRequest->checkLife())
	{
		mResult.clear();
		return AD_SUCCESS;
	}
	
	AdFilterMarkMatchMark(ctData, setMark, mResult);

	AdFilterMarkMatchAudience(ctData, mAudience, mResult);
	if( ret == AD_SUCCESS){
		AdFilterMarkVipUserTag(ctData, mResult);
	}

	return AD_SUCCESS;
}


int AdFilterMark::AddData(void * data)
{
	int ret;
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	
	ret = AdFilterMarkAddMark(pUnit);
	if(ret != AD_SUCCESS)
		return ret;
	
	ret = AdFilterMarkAddAudience(pUnit);
	if(ret != AD_SUCCESS)
		return ret;
	return ret;
}

int AdFilterMark::AdFilterMarkVipUserTag(AdUserData& ctMark, AD_RESULT & mResult)
{
	
	if ( !ctMark.m_sExecuteID.empty() ){
		AD_RESULT_ITER  iter = mResult.begin();
		while(iter != mResult.end()){
			AdPlanningUnit *AdPlanUnit = (AdPlanningUnit*)iter->first;
			if ( (UINT)atoi(ctMark.m_sExecuteID.c_str()) == AdPlanUnit->m_stOrder.m_uiExecuteId ){
				iter->second.m_bVip = true;
			}
			iter++;
		}
	}
	
	return AD_FAILURE;
}

