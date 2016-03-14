/***************************************
*	Description: Ad deal rank module: compute order price, select order for bidding.
*	Author: wangyuanji
*	Date: 2015/08/03j
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/


#ifndef	_AD_RTB_DEALRANK_H
#define	_AD_RTB_DEALRANK_H

#include <map>
#include <list>
#include <math.h>
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "AdRtbConfig.h"
#include "AdGlobal.h"
#include "AdRtbCtr.h"
#include "AdRtbProtect.h"
#include "AdPlanningData.h"
#include "AdRtbFilters.h"
#include "AdRtbBase.h"
#include "AdRtbEngCtr.h"
#include "AdUtil.h"
#include "AdBuff.h"


#define AD_VIP_PRICE	5000

typedef struct tagAdUnitPrice
{
	void* pPlaningData;
	UINT uiEid;
	UINT uiPriceType;		// cpm / cpc
	UINT uiEidType;	// nice / base / audi
	UINT uiPrice;

}AdUnitPrice;



class AdRtbDealRank
{
 public:
    AdRtbDealRank();
    virtual ~AdRtbDealRank();
	
	int AdRtbDealRankInit(AdRtbConfig & ctConfig);	
	int AdRtbRankChoice(AdRtbRequest * pRequest, AD_RESULT & mResult, AdUnitPrice *pstRetAdUnit, bool bPrint = false);

protected:
    int AdRtbAdvanceStrategy(AdRtbRequest * pRequest, AdPlanningUnit *AdUnit, UINT *m_uiPrice);
    int AdRtbRankStrategy(vector<AdUnitPrice>& v_rankVec, AdUnitPrice* stRetAdUnit);
	int AdRtbDealRankCtrInit(string & sFile);
	AdRtbCtrData * AdRtbDealRankCtrSearch(string & sKey);
	int AdRtbCtrCompute(AdRtbRequest * pRequest, AdPlanningUnit *AdUnit, UINT *uiPrice);
	bool AdRtbDealVipUser(bool bVip, AdUnitPrice* stRetAdUnit, AdUnitPrice* AdUnit);
    int AdRtbDealRankCtrModelInit(string & sDir);
    int AdRtbCtrModel(AdRtbRequest * pRequest, AdPlanningUnit * AdUnit, list < AdFilterMarkInfo > & m_stMark, UINT * uiPrice);
	
private: 
	
	float m_fWeight;	
	string m_sCtrModelFile;	
    AdRtbEngCtrModel *m_ctCtrModel;
    AdRtbEngCtrModel *m_ctCtrModelA;
    AdRtbEngCtrModel *m_ctCtrModelB;
	
	AdPingPongBuffer<AdRtbCtr> m_ctCtr;
	string m_sCtrFile;
	AdPingPongBuffer<map<UINT, float> >  m_mDefaultCtr;
};


 struct CmpByPrice {  
  bool operator()(const int& p1, const int& p2) {  
    return p1 > p2;  
  }  
};

typedef map< int, vector<AdUnitPrice> , CmpByPrice> M_PRICE;

int AdRtbRandWet(UINT rCount, float rWet);


#endif
