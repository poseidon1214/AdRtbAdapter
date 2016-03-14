/***************************************
*	Description: Ad deal rank module: compute order price, select order for bidding.
*	Author: wangyuanji
*	Date: 2015/08/03j
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdRtbDealRank.h"
#include "AdRtbEngCtr.h"
#include "AdTime.h"


AdRtbDealRank::AdRtbDealRank()
{
    m_ctCtrModel = NULL;
    m_ctCtrModelA = NULL;
    m_ctCtrModelB = NULL;
}

AdRtbDealRank::~AdRtbDealRank()
{
}

int AdRtbDealRank::AdRtbDealRankCtrInit(string & sFile)
{
	int ret;
	AdRtbCtr  & ctCtr = m_ctCtr.GetBack();
	
	ret = ctCtr.AdRtbCtrInit( sFile );
	if(ret != AD_SUCCESS)
	{
		return ret;
	}

	m_ctCtr.Switch();
	return AD_SUCCESS;
}

AdRtbCtrData * AdRtbDealRank::AdRtbDealRankCtrSearch(string & sKey)
{
	return m_ctCtr.Get().AdRtbCtrSearch(sKey);
}

int AdRtbDealRank::AdRtbCtrCompute(AdRtbRequest * pRequest, 
											AdPlanningUnit *AdUnit, UINT *uiPrice)
{
	int iRet = AD_FAILURE;
	
	AdRtbCtrData *adCtr = AdRtbDealRankCtrSearch(pRequest->m_sPId);
	if( adCtr != NULL ){
		*uiPrice = 1000 * adCtr->m_fCtrVal * AdUnit->m_stOrder.m_uiTargetPrice;
		iRet = AD_SUCCESS;
	}
	else
	{
		map<UINT, float> & ctDefaultCtr = m_mDefaultCtr.Get();
		map<UINT, float>::iterator iter = ctDefaultCtr.find(pRequest->m_uiAdx);
		if (  iter != ctDefaultCtr.end() )
		{
			*uiPrice = 1000 * iter->second * AdUnit->m_stOrder.m_uiTargetPrice;
			iRet = AD_SUCCESS;
		}
		else
		{
			iRet = AD_FAILURE;
		}
	}
	return iRet;
}



int AdRtbDealRank::AdRtbDealRankCtrModelInit(string & sDir)
{
    int ret;
    Time::wait(10);
    AdRtbEngCtrModel **ppCtrModelTmp = NULL;
    if(m_ctCtrModel != m_ctCtrModelA){
        ppCtrModelTmp = &m_ctCtrModelA;
    }else if(m_ctCtrModel != m_ctCtrModelB){
        ppCtrModelTmp = &m_ctCtrModelB;
    }else{
        ppCtrModelTmp = &m_ctCtrModelA;
    }

    if(*ppCtrModelTmp != NULL){
        (*ppCtrModelTmp)->AdRtbEngDeinit();
        delete *ppCtrModelTmp, *ppCtrModelTmp = NULL;
    }

    *ppCtrModelTmp = new AdRtbEngCtrModel;
    ret = (*ppCtrModelTmp)->AdRtbEngInit(sDir);
    if(ret){
	    AD_PRINT("[ERROR] Load ctr file error");
	    return AD_FAILURE;
    }

    m_ctCtrModel = *ppCtrModelTmp;

    return AD_SUCCESS;
}

int AdRtbDealRank::AdRtbDealRankInit(AdRtbConfig & ctConfig)
{

	int iRet = AD_FAILURE;
	string sSec = "[CONFIG]";
	ADCONFIG & Config = ctConfig.Get(sSec);
	sSec = "[GLOBAL]";
	ADCONFIG & global = ctConfig.Get(sSec);
	// config
	string sFile = "CTRM";
	m_fWeight = atof(global["WEIGHT"].c_str());
    	m_sCtrModelFile = Config[sFile];
	m_sCtrFile = Config["CTRS"];

	sSec = "[DEFAULTCTR]";
	ADCONFIG & ctrConfig = ctConfig.Get(sSec);
	ADCONFIG_ITER iter;
	map<UINT, float> & ctDefaultCtr = m_mDefaultCtr.GetBack();
	ctDefaultCtr.clear();
	for(iter = ctrConfig.begin(); iter != ctrConfig.end(); iter ++)
	{
		ctDefaultCtr[atoi(iter->first.c_str())] = atof(iter->second.c_str());
	}
	m_mDefaultCtr.Switch();
	
	if(ctConfig.FileIsChanged(sFile))
	{
		if(AD_SUCCESS != AdRtbDealRankCtrModelInit( m_sCtrModelFile )){
			AD_PRINT("AdRtbCtrModeInit Err\n");
			return iRet;
		}
	}

	if(AD_SUCCESS != AdRtbDealRankCtrInit( m_sCtrFile )){
		AD_PRINT("AdRtbCtrInit Err\n");
		return iRet;
	}

	return AD_SUCCESS;
}

int AdRtbDealRank::AdRtbCtrModel(AdRtbRequest * pRequest, AdPlanningUnit *AdUnit, list<AdFilterMarkInfo> &m_stMark,UINT *uiPrice)
{
    if(m_ctCtrModel == NULL){
        return AD_FAILURE;
    }
    
    int iRet = 0;
    AD_RESULT_ITER  iter;
    AdRtbEngCtrModel *m_ctCtrModelTmp = m_ctCtrModel;
    
    if(!m_stMark.empty()){
        m_ctCtrModelTmp->AdRtbEngCtrGet(pRequest, AdUnit, &(m_stMark.front().m_sMark), uiPrice);
    }else{
        m_ctCtrModelTmp->AdRtbEngCtrGet(pRequest, AdUnit, NULL, uiPrice);
    }
    
    return iRet;
}

int AdRtbDealRank::AdRtbAdvanceStrategy(AdRtbRequest * pRequest, 
											AdPlanningUnit *AdUnit, UINT *uiPrice)
{
	if( AdUnit->m_pAdvancedPrice != NULL ){
		map<string,  AdAdvancedPrice>::iterator iter ;
	    iter = AdUnit->m_pAdvancedPrice->find(pRequest->m_sPId);
		if( iter != AdUnit->m_pAdvancedPrice->end() ){
			*uiPrice = iter->second.m_iEprice;
			return AD_SUCCESS;
		}
		else{
			return AD_FAILURE;
		}
	}
	return AD_FAILURE;
}


bool AdRtbDealRank::AdRtbDealVipUser(bool bVip, AdUnitPrice* stRetAdUnit, AdUnitPrice* AdUnit)
{
	if (bVip){
		AdUnit->uiPrice = AD_VIP_PRICE;
		memcpy(stRetAdUnit, AdUnit, sizeof(AdUnitPrice));
	}
	else{
	}
	return bVip;
}

int AdRtbDealRank::AdRtbRankStrategy(vector<AdUnitPrice>& v_rankVec, AdUnitPrice* stRetAdUnit)
{
	
	int iSize = 0;
	int iReturn = AD_SUCCESS;
	int vec_size = v_rankVec.size();
	AdUnitPrice *pAdUnitPrice = NULL;
	vector<AdUnitPrice>::iterator it;
	map< int, vector<AdUnitPrice> , CmpByPrice> m_Price;
	map< int, vector<AdUnitPrice> , CmpByPrice>::iterator it_Price;	
	
	if( vec_size == 0){
		//return iReturn;
		iReturn = AD_FAILURE;
	}
	else if( vec_size == 1){
		memcpy(stRetAdUnit, &v_rankVec[0], sizeof(AdUnitPrice));
	}
	else if( vec_size <= 3){
		pAdUnitPrice = &v_rankVec.at(AdRtbRandWet(vec_size, m_fWeight));
		memcpy(stRetAdUnit, pAdUnitPrice, sizeof(AdUnitPrice));		
	}
	else{
		for(it = v_rankVec.begin(); it != v_rankVec.end(); it++){
			vector<AdUnitPrice>& vect = m_Price[(*it).uiPrice];
			vect.push_back(*it);
		}
		// map size
		iSize = m_Price.size();

		if( iSize > 0){
            int i = 0;
			iSize = m_Price.size() >= 3 ? 3:m_Price.size();
			int index = AdRtbRandWet(iSize, m_fWeight);
            for( it_Price = m_Price.begin(); it_Price != m_Price.end(); it_Price++ ){
                if (i == index){
                    vector<AdUnitPrice>& vect = it_Price->second;
                    // random choice
                    srand((UINT)time(NULL));
                    memcpy(stRetAdUnit, &vect.at( rand() % vect.size() ), sizeof(AdUnitPrice));
                   break; 
                }
                i++;
            }
		}
		else{
			iReturn = AD_FAILURE;
		}
	}

	return iReturn;
}

int AdRtbDealRank::AdRtbRankChoice(AdRtbRequest * pRequest, AD_RESULT & mResult, 
					AdUnitPrice *pstRetAdUnit, bool bPrint)
{
	int iRet = AD_SUCCESS;
	AD_RESULT_ITER  iter = mResult.begin();
	vector<AdUnitPrice> v_rankVec;
	if( bPrint )
	{
		AD_PRINT("RankChoice %d-------------------------\n", static_cast<int>(mResult.size()));
	}
	while(iter != mResult.end())
	{
		AdPlanningUnit *AdPlanUnit = (AdPlanningUnit*)iter->first;

		AdUnitPrice stAdUnit;
		memset(&stAdUnit, '\0', sizeof(AdUnitPrice));
		stAdUnit.pPlaningData = (void*)AdPlanUnit;
		stAdUnit.uiEid = AdPlanUnit->m_stOrder.m_uiExecuteId;
		stAdUnit.uiPriceType = AdPlanUnit->m_stOrder.m_uiTargetPriceType;

		// vip 
		if (AdRtbDealVipUser(iter->second.m_bVip, pstRetAdUnit, &stAdUnit)){
			if( bPrint ){
				AD_PRINT("User is VIP, RankResult:%d[%c/B/%d]. \n", pstRetAdUnit->uiEid,\
					(  pstRetAdUnit->uiPriceType == AD_ORDER_CPM ) ? 'M':'C',pstRetAdUnit->uiPrice);

			}
			return AD_SUCCESS;
		}
		
		// advance search
		if ( AD_SUCCESS == AdRtbAdvanceStrategy( pRequest, AdPlanUnit, &stAdUnit.uiPrice ) ){
			//
		}
		else if ( stAdUnit.uiPriceType == AD_ORDER_CPM ){
			// cpm
			stAdUnit.uiPrice = AdPlanUnit->m_stOrder.m_uiTargetPrice;			
		}
		else if ( stAdUnit.uiPriceType == AD_ORDER_CPC){
			// ctr search or ctr model
			#if 1
			AdRtbCtrModel(pRequest, AdPlanUnit, iter->second.m_ltMarkInfo, &stAdUnit.uiPrice);
			#else
			if(AdPlanUnit->m_stOrder.m_dMinCtr > 0.0){
               			 AdRtbCtrModel(pRequest, AdPlanUnit, iter->second.m_ltMarkInfo, &stAdUnit.uiPrice);
			}
			else 
			{
				AdRtbCtrCompute( pRequest, AdPlanUnit, &stAdUnit.uiPrice );
			}
			#endif
		}

		// price confirm
		// add youku miaozhen
		UINT uiMinPrice = atoi(pRequest->m_sMinPrice.c_str());
		if( stAdUnit.uiPrice > 0 && stAdUnit.uiPrice > uiMinPrice){
			v_rankVec.push_back(stAdUnit);
		}
		else if( atoi(pRequest->m_sFlow.c_str()) == AD_PROTOCOL_FLOW_VIDEO && \
			(pRequest->m_uiAdx == AD_PROTOCOL_KEY_ADX_YUKU || \
			pRequest->m_uiAdx == AD_PROTOCOL_KEY_ADX_MIAOZHEN )) {
			v_rankVec.push_back(stAdUnit);
		}

		if( bPrint ){
			AD_PRINT("%d[%c/B/%d] ", stAdUnit.uiEid,\
				(  stAdUnit.uiPriceType == AD_ORDER_CPM ) ? 'M':'C',stAdUnit.uiPrice);
		}
		
		iter++;
	}
	
	// deal
	if ( AD_SUCCESS == AdRtbRankStrategy( v_rankVec,  pstRetAdUnit))
	{
		if( bPrint ){
			AD_PRINT("\n RankResult:%d[%c/B/%d]. \n", pstRetAdUnit->uiEid,\
				(  pstRetAdUnit->uiPriceType == AD_ORDER_CPM ) ? 'M':'C',pstRetAdUnit->uiPrice);
		}
	}
	else{
		iRet = AD_FAILURE;

		if( bPrint ){
			AD_PRINT("\n No RankResult. \n");
		}
	}
	
	return iRet;	
}



int AdRtbRandWet(UINT rCount, float rWet)

{
	int ranum;
    int i,j = 0;
    int num = rCount;
    float wt = rWet;
    int choice[100] = {0};
    int chnum = 0;
    int tmptol = 0;
    const int total = 100;
	
	if(rCount == 1){
		return 0;
	}

    for(i = 0; i < num ; i++ ){
        if(i < num - 1){
            chnum = float(pow((1-wt),i))*wt*100;
            for(j = tmptol; j < chnum+tmptol; j ++){
                choice[j] = i;
            }
            tmptol += chnum;
        }
        else{
            for(j = tmptol; j < total; j++){
                choice[j] = i;
            }
        }
    }

	srand((UINT)time(NULL));
	if ( (ranum = rand()%total) < total){
		return choice[ranum];
	}else{
		return 0;
	}
}

