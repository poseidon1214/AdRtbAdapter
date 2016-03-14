/********************************************************/
/*	Geo Polymerization Technology Co, Ltd		*/
/*	Project:	GtRtbEng-6.6.0			*/
/*	Author:		zp			*/
/*	Date:		2015_06_09			*/
/*	File:		GtRtbEngCtr.cpp		*/
/********************************************************/
//#include "GtLog.h"
#include "AdRtbEngCtr.h"

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>


#define GT_URL_HEAD "http://"
#define GT_HOST_LEN 128

AdRtbEngCtrModel::AdRtbEngCtrModel()
{
    pClassifier = NULL;
}

AdRtbEngCtrModel::~AdRtbEngCtrModel()
{
    pClassifier = NULL;
}

int AdRtbEngCtrModel::AdRtbEngInit(string & sDir)
{
    int ret = 0;
    gfs::Classifier **pClassifierTmp = &pClassifier;

    *pClassifierTmp = new gfs::Classifier();
    ret = (*pClassifierTmp)->init(sDir);
    if(!ret){
	    AD_PRINT("[ERROR] Load ctr file error");
	    return AD_FAILURE;
    }

    return AD_SUCCESS;
}

int AdRtbEngCtrModel::AdRtbEngDeinit()
{
    gfs::Classifier **pClassifierTmp = &pClassifier;
    if(*pClassifierTmp != NULL)
	    delete *pClassifierTmp, *pClassifierTmp = NULL;
    return AD_SUCCESS;
}

int AdRtbEngCtrModel::GtRtbEngGetHost(char *url, char *host)
{
    unsigned int uiPos = 0, uiStPos = 0;
    if(strncmp(url, GT_URL_HEAD, strlen(GT_URL_HEAD)) == 0){
        uiStPos = strlen(GT_URL_HEAD);
        uiPos = uiStPos;
        while((url[uiPos] != '/')&&(url[uiPos] != 0)&&(uiPos < GT_HOST_LEN)){uiPos++;}
        if((uiPos - uiStPos) < GT_HOST_LEN){
            memcpy(host, &url[uiStPos], (uiPos - uiStPos));
        }
    }else{
        while((url[uiPos] != '/')&&(url[uiPos] != 0)&&(uiPos < GT_HOST_LEN)){uiPos++;}
         if((uiPos - uiStPos) < GT_HOST_LEN){
            memcpy(host, &url[uiStPos], (uiPos - uiStPos));
        }
    }
    return 0;
}

int AdRtbEngCtrModel::AdRtbEngPrice(AdRtbRequest * pRequest, AdPlanningUnit *AdUnit, UINT *uiPrice, double ctr)
{
	UINT m_uiTgPrice;
	
    if((ctr/10) < AdUnit->m_stOrder.m_dMinCtr){
	    *uiPrice = 0;
	    return 0;
    }
	m_uiTgPrice = AdUnit->m_stOrder.m_uiTargetPrice * (1000 / 10); //this 10 is from  experience
    *uiPrice = (UINT)(ctr * m_uiTgPrice);
    if(*uiPrice > (AdUnit->m_stOrder.m_uiTargetPrice * 5)){
	    *uiPrice = AdUnit->m_stOrder.m_uiTargetPrice * 5;
    }
    return 0;
}

int AdRtbEngCtrModel::AdRtbEngCtrGet(AdRtbRequest * pRequest, AdPlanningUnit *AdUnit, std::string *m_stMark,UINT *uiPrice)
{
    char szAdv[16] = {0}, szOs[8] = {0}, szScreen[8] = {0}, szPT[8] = {0};
    char szHost[GT_HOST_LEN] = {0};
    list<UINT>::iterator iter;
    gfs::Classifier *tmp_class = pClassifier;
	gfs::RawInstance ins;
    if(pRequest->m_sArea.c_str()[0] == 0){
        ins.region = "NULL";
    }else{
        ins.region  =  pRequest->m_sArea;
    }
    
    if(!pRequest->m_ltMobileDevice.empty()){
        if(pRequest->m_ltMobileDevice.front() == "0"){
             ins.os = "NULL";
        }else{
            sprintf(szOs, "%s", (pRequest->m_ltMobileDevice.front()).c_str());
            ins.os      =  szOs;
        }
    }else{
        ins.os = "NULL";
    }

    if(m_stMark != NULL){
        if(m_stMark->c_str()[0] == 0){
            ins.user_tags = "NULL";
        }else{
            ins.user_tags   =  *m_stMark;
        }
    }else{
        ins.user_tags = "NULL";
    }
    
    if(pRequest->m_sUserUrl.c_str()[0] == 0){
        ins.host    =  "NULL";
        strcpy(szHost, "NULL");
    }else{
        GtRtbEngGetHost((char *)pRequest->m_sUserUrl.c_str(), szHost);
        ins.host    =  szHost;
    }

    if(pRequest->m_sPId.c_str()[0] == 0){
        ins.pid = "NULL";
    }else{
        ins.pid     =  pRequest->m_sPId;
    }

    sprintf(szPT, "%s", pRequest->m_sAdViewScreen.c_str());
    ins.page_type = szPT;
    sprintf(szScreen, "%s", pRequest->m_sAdViewType.c_str());
	ins.view_screen	=  szScreen;
    sprintf(szAdv, "%d", AdUnit->m_stOrder.m_uiAdvertiser);
	ins.adver_id	=  szAdv;
    
	double ctr = tmp_class->predict_ctr(ins);
    
    AdRtbEngPrice(pRequest, AdUnit, uiPrice, ctr);

//    GtRtbEngLog(pstReq, pstExec, stHit, rHotInfo, CCtr, ctr, szHost, ins);
    return AD_SUCCESS;
}

