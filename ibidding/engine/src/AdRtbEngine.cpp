/***************************************
*	Description: Ad rtb Engine module,   system main module, include all bidding details.
*	Author: wilson
*	Date: 2015/07/25
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <stdlib.h>
#include "AdRtbEngine.h"
#include "AdTcp.h"

AdRtbEngine::AdRtbEngine()
{
	string sName = "iBidding";
	SetName(sName);
}

AdRtbEngine::~AdRtbEngine()
{
}

void AdRtbEngine::AdRtbEngineAddStatus(AdTask & ctTask)
{
	AdBiddingTaskStatus * pStatus;
	if(ctTask.m_pState == NULL)
	{
		pStatus = new AdBiddingTaskStatus;
		if(pStatus == NULL)
		{
			AD_ERROR("bidding Status Alloc Error\n");
			return;
		}
		ctTask.m_pState = static_cast<AdTaskState *> (pStatus);
	}
}

int AdRtbEngine:: AdRtbEngineInit(string & confFile)
{
	int ret;

	m_sConfigFile = confFile;

	AdRtbConfig & ctConf = m_ctConfig.GetBack();
	
	cout<<"    Engine Config load........."<<endl;
	ret = ctConf.Load(m_sConfigFile);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Adconfig Load error\n");
		return ret;
	}

	string sSec = "[ADAPTER]";
	ADCONFIG & AdpServer = ctConf.Get(sSec);
	ADCONFIG_ITER  iter;
	for(iter = AdpServer.begin(); iter != AdpServer.end(); iter ++)
	{
		m_ltServer.push_back(iter->second);
	}

	sSec = "[GLOBAL]";
	ADCONFIG & global = ctConf.Get(sSec);
	cout<<"    Engine PlanningData Init........."<<endl;
	m_ctData.AdPlanningDataInit(global);

	m_ctConfig.Switch();
	return ret;
}

int AdRtbEngine:: AdRtbEngineSetLocalMode(int iPort)
{
	m_iPort = iPort;
	return AD_SUCCESS;
}

list<string> & AdRtbEngine:: AdRtbEngineGetAdapter()
{
	return m_ltServer;
}


int AdRtbEngine:: AdRtbEngineUpdate()
{
	int ret;
	UINT  uiData = 0;
	AdRtbConfig & ctConf = m_ctConfig.Get();

	ret = m_ctData.DataUpdate(&uiData);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("update data Error\n");
		return ret;
	}
	else if(uiData != 0)
	{
		//advanced price data update follow base data update
		ret = m_ctData.DataUpdateAdvancedPriceData();
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("update advance filter data Error\n");
			return ret;
		}
	}
	
	ret = m_ctData.DataUpdateAdvancedFilterResult();
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("update advance filter data Error\n");
		return ret;
	}

	ret = ctConf.Check();
	if(ret > 0)
	{
		if(ctConf.FileIsChanged(m_sConfigFile))
		{
			return AD_RTB_ENGINE_REINIT;
		}

		ret = m_ctDealRank.AdRtbDealRankInit(ctConf);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("Init Deal rank module Error\n");
			return ret;
		}
	}

	ret = m_ctFilters.FiltersUpdate(m_ctData.DataGetCache(), ctConf);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Filter update data Error\n");
		return ret;
	}
	return ret;
}

int AdRtbEngine:: AdRtbEngineRun(char * pPacket, int iLen, int iMaxLen, AdTask & ctTask)
{
	AD_RESULT  mResult;
	AD_RESULT_ITER   mIter;
	AdUnitPrice stUnitPrice;
	AdRtbRequest  ctRequest;
	AdRtbResponse ctResponse;
	int ret;
	AdRtbProtocol   ctTlvProtocol;

	AdRtbConfig & ctConf = m_ctConfig.Get();
	string sSec = "[GLOBAL]";
	ADCONFIG & gConf = ctConf.Get(sSec);
	
	//get Request Data, and response struct;
	ret = ctTlvProtocol.TlvToInternal(ctRequest, pPacket, iLen, (ctTask.m_uiPrint & AD_ENGINE_PRINT_REQUEST) != 0);
	if(ret != AD_SUCCESS)
	{
		return AD_SUCCESS;
	}

	ctTask.m_uiLoop ++;
	ctRequest.m_uiRequestId = ctTask.m_uiLoop;

	AdBiddingTaskStatus * pStatus = static_cast<AdBiddingTaskStatus *>(ctTask.m_pState);
	if(pStatus == NULL)
	{
		AdRtbEngineAddStatus(ctTask);
		pStatus = static_cast<AdBiddingTaskStatus *>(ctTask.m_pState);
	}
	if(pStatus != NULL)
		pStatus->AdRtbStatusRequestAdd();
	
	m_ctFilters.FiltersMatch(static_cast<void *>(&ctRequest),mResult, 
			(ctTask.m_uiPrint & AD_ENGINE_PRINT_FILTER) != 0);

	if(mResult.empty())
	{
		goto ERROR;
	}

	if( ctTask.m_uiPrint & AD_ENGINE_PRINT_REQUEST)
	{
		AD_PRINT("Filter Match:  %d(%d ns)\n", static_cast<int>(mResult.size()), ctRequest.getLife());
		Result::display(mResult);
	}

	for(mIter = mResult.begin(); mIter != mResult.end(); mIter ++)
	{
		AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(mIter->first);
		if(pStatus != NULL)
			pStatus->AdRtbStatusReadyAdd(pUnit->m_stOrder.m_uiExecuteId);
	}

	//bid price
	ret = m_ctDealRank.AdRtbRankChoice(&ctRequest, mResult, &stUnitPrice, 
			(ctTask.m_uiPrint & AD_ENGINE_PRINT_RANK) != 0);
	if(ret != AD_SUCCESS)
	{
		goto ERROR;
	}

	ret = AdRtbEngineBuild(ctRequest, ctResponse, stUnitPrice);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Response Build Error\n");
		goto ERROR;
	}

	if(pStatus != NULL)
	{
		pStatus->AdRtbStatusResponseAdd();
		pStatus->AdRtbStatusUpdate(gConf["REDIS_REALTIME"]);
	}

ERROR:
	ret= ctTlvProtocol.InternalToTlv(ctResponse, pPacket, iMaxLen, (ctTask.m_uiPrint & AD_ENGINE_PRINT_RESPONSE) != 0);
	if((ctTask.m_uiPrint & AD_ENGINE_PRINT_RESPONSE) != 0)
		AD_PRINT("AD  iBidding Complete(%d)---Ms: %d--------------- \n", ret, ctRequest.getLife());
	return ret;
}


int AdRtbEngine:: AdRtbEngineRun(AdRtbRequest & ctRequest, AdRtbResponse & ctResponse, AdTask & ctTask)
{
	AD_RESULT  mResult;
	AD_RESULT_ITER   mIter;
	AdUnitPrice stUnitPrice;
	AdRtbProtocol   ctTlvProtocol;
	int ret;

	AdRtbConfig & ctConf = m_ctConfig.Get();
	string sSec = "[GLOBAL]";
	ADCONFIG & gConf = ctConf.Get(sSec);
	
	ctTask.m_uiLoop ++;
	ctRequest.m_uiRequestId = ctTask.m_uiLoop;
	
	if((ctTask.m_uiPrint & AD_ENGINE_PRINT_REQUEST) != 0)
		ctRequest.display();

	AdBiddingTaskStatus * pStatus = static_cast<AdBiddingTaskStatus *>(ctTask.m_pState);
	if(pStatus == NULL)
	{
		AdRtbEngineAddStatus(ctTask);
		pStatus = static_cast<AdBiddingTaskStatus *>(ctTask.m_pState);
	}
	if(pStatus != NULL)
		pStatus->AdRtbStatusRequestAdd();
	
	m_ctFilters.FiltersMatch(static_cast<void *>(&ctRequest),mResult, 
			(ctTask.m_uiPrint & AD_ENGINE_PRINT_FILTER) != 0);

	if(mResult.empty()||(ctRequest.checkLife()))
	{
		return AD_FAILURE;
	}

	if( ctTask.m_uiPrint & AD_ENGINE_PRINT_REQUEST)
	{
		AD_PRINT("Filter Match:  %d(%d ns)\n", static_cast<int>(mResult.size()), ctRequest.getLife());
		Result::display(mResult);
	}

	for(mIter = mResult.begin(); mIter != mResult.end(); mIter ++)
	{
		AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(mIter->first);
		if(pStatus != NULL)
			pStatus->AdRtbStatusReadyAdd(pUnit->m_stOrder.m_uiExecuteId);
	}

	//bid price
	ret = m_ctDealRank.AdRtbRankChoice(&ctRequest, mResult, &stUnitPrice, 
			(ctTask.m_uiPrint & AD_ENGINE_PRINT_RANK) != 0);
	if(ret != AD_SUCCESS)
	{
		return AD_FAILURE;
	}

	ret = AdRtbEngineBuild(ctRequest, ctResponse, stUnitPrice);
	if(ret != AD_SUCCESS)
	{
		return ret;
	}

	if((ctTask.m_uiPrint & AD_ENGINE_PRINT_RESPONSE) != 0)
	{
		ctResponse.display();
	}
	if(pStatus != NULL)
	{
		pStatus->AdRtbStatusResponseAdd();
		pStatus->AdRtbStatusUpdate(gConf["REDIS_REALTIME"]);
	}
	
	if((ctTask.m_uiPrint & AD_ENGINE_PRINT_RESPONSE) != 0)
		AD_PRINT("AD  iBidding Complete---ns: %d--------------- \n", ctRequest.getLife());
	return AD_SUCCESS;
}

int AdRtbEngine::AdRtbEngineLoop(AdTask & ctTask)
{
	int len;
	char  sBuf[AD_KBYTES4];
	AdTcp ctTcp;
	int ret;
	struct InternalPacket * pPacket;
	int iReqNum;

	ret = ctTcp.Connect("127.0.0.1", m_iPort);
	if(ret <= 0)
	{
		AD_ERROR("Connect Adapter error\n");
		return ret;
	}

	sprintf(sBuf, "%s  0x%x", AD_IBIDDING_ENG_LOCAL, ctTask.m_uiTaskId);
	
	//sprintf(sBuf, "%s  0x%x", AD_IBIDDING_ENG_REMOTE, ctTask.m_uiTaskId);

	ret =  ctTcp.Write(sBuf, strlen(sBuf));
	while(ret == 0)
		ret =  ctTcp.Write(sBuf, strlen(sBuf));
	if(ret  == AD_FAILURE)
	{
		ctTcp.Disconnect();
		return ret;
	}

	while(ctTask.m_bTaskRun)
	{
		ctTask.SetTaskWaitting(true);
		len = ctTcp.Read(sBuf,  AD_KBYTES4);
		if(len == AD_FAILURE)
		{
			ctTask.SetTaskWaitting(false);
			break;
		}
		if(len == 0)
			continue;
		ctTask.SetTaskWaitting(false);

		iReqNum = len / sizeof(struct InternalPacket);
		pPacket = (struct InternalPacket *) sBuf;
		while(iReqNum > 0)
		{
			ret = AdRtbEngineRun(*(pPacket->pRequest),*(pPacket->pResponse), ctTask);

			ret =  ctTcp.Write((char *)pPacket,  sizeof(struct InternalPacket));
			while(ret == 0)
				ret =  ctTcp.Write((char *)pPacket, sizeof(struct InternalPacket));
			if(ret  == AD_FAILURE)
			{
				break;
			}
			iReqNum --;
			pPacket ++;
		}
	}
	ctTcp.Disconnect();
	return AD_FAILURE;
}



AdMaterialInfo* AdRtbEngine::AdRtbEngineGetMaterials(AdCreativeInfo & ctCreate, string & sSize)
{
	vector<string> pVec;
	UINT height;
	UINT width;
	list<AdMaterialInfo>::iterator iter;	// material 		

	AdString::StrSplit(sSize, pVec, "x");
	if(pVec.size() == 2)
	{
		width = atoi(pVec[0].c_str());
		height = atoi(pVec[1].c_str());
	}
	else
		return NULL;
	for(iter = ctCreate.m_ltMaterials.begin();  iter != ctCreate.m_ltMaterials.end(); iter ++)
	{
		if(((*iter).m_uiWidth == width) && ((*iter).m_uiHeight == height))
			return &(*iter);
	}
	return NULL;
}
int AdRtbEngine::AdRtbEngineBuild(AdRtbRequest & ctReq, AdRtbResponse & ctRes, AdUnitPrice & stPrice)
{
	char buf[AD_BYTE256];
	list<string>::iterator iter;
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(stPrice.pPlaningData);

	if(pUnit->m_mCreativeInfo.find(ctReq.m_uiAdx) == pUnit->m_mCreativeInfo.end())
	{
		return AD_FAILURE;
	}

	AdCreativeInfo &ctCreate = pUnit->m_mCreativeInfo[ctReq.m_uiAdx];

	AdMaterialInfo *pMate = AdRtbEngineGetMaterials(ctCreate, ctReq.m_sSize);
	
	if(pMate == NULL)
		return AD_FAILURE;
	
	snprintf(buf, AD_BYTE256, "%d", stPrice.uiPrice);
	ctRes.m_sMaxPrice = buf;

	ctRes.m_uiAdx = ctReq.m_uiAdx;
	ctRes.m_sFlow = ctReq.m_sFlow;
	ctRes.m_sBidID = ctReq.m_sBidID;
	ctRes.m_sPId =  ctReq.m_sPId;

	ctRes.m_sArea = ctReq.m_sArea;
	ctRes.m_sClickUrl = pMate->m_sClickUrl;
	ctRes.m_sDestUrl = pMate->m_sDestinationUrl;
	ctRes.m_sHtml = pMate->m_sMaterialUrl;
	ctRes.m_sHtmlMraid = pMate->m_sMraidMaterialUrl;
	ctRes.m_sCategory = pMate->m_sCategory;
	ctRes.m_sCreateGID = ctCreate.m_sCreativeGroupId;
	ctRes.m_sCreativeID = pMate->m_sCreateId;
	ctRes.m_sUserID = ctReq.m_sUserID;
	ctRes.m_sUserIP = ctReq.m_sUserIP;
	ctRes.m_sSize = ctReq.m_sSize;
	ctRes.m_sUud = pMate->m_sUuId;
	ctRes.m_sMaterialID = pMate->m_sMaterialId;

	iter = ctReq.m_ltMobileDevice.begin();
	if(iter != ctReq.m_ltMobileDevice.end())
	{
		snprintf(buf, AD_BYTE256, "%s", iter->c_str());
		ctRes.m_sDevType = buf;
	}

	AdRtbConfig & ctConf = m_ctConfig.Get();
	string sSec = "[GLOBAL]";
	ADCONFIG & gConf = ctConf.Get(sSec);
	ctRes.m_sRegion = gConf["DEV"];
	snprintf(buf, AD_BYTE256, "%d", pMate->m_uiVideoTime);
	ctRes.m_sDuration = buf;
	snprintf(buf, AD_BYTE256, "%d", pMate->m_uiFormat);
	ctRes.m_sMaterailFormat = buf;
	snprintf(buf, AD_BYTE256, "%d", pMate->m_uiCreativeType);
	ctRes.m_sCreative = buf;
	snprintf(buf, AD_BYTE256, "%d", pUnit->m_stOrder.m_uiTargetPriceType);
	ctRes.m_sTargetPriceType = buf;
	snprintf(buf, AD_BYTE256, "%d", pUnit->m_stOrder.m_uiExecuteId);
	ctRes.m_sExecID = buf;
	snprintf(buf, AD_BYTE256, "%d", pUnit->m_stOrder.m_uiAdvertiser);
	ctRes.m_sAdvertiserId = buf;
	snprintf(buf, AD_BYTE256, "%d", pUnit->m_stOrder.m_uiTargetPrice);
	ctRes.m_sTargetPrice = buf;

	ctRes.m_sTanxID = pUnit->m_stOrder.m_sTanxAdId;
	
	ctRes.m_ltMonitorUrl = pMate->m_ltMonitorUrl;
#if 0
	list<string>::iterator iter;
	k
	for(iter = pMate->m_ltMoniterUrl.begin(); iter != pMate->m_ltMoniterUrl.end(); iter ++)
	{
	}
	for(uiCount = 0; uiCount < GT_RA_MON; uiCount++)
	{
		if( *(pstMate->m_szMon[uiCount].m_szImp) != 0){
		strcpy(pstRes->m_szMon[uiCount].m_szImp, pstMate->m_szMon[uiCount].m_szImp);
		}
	}
#endif

	return AD_SUCCESS;
}


int AdRtbEngine::Run(AdCache * pCache, AdSession * pSession, AdTask & ctTask)
{
	int len;
	if(pCache == NULL)
		return AD_FAILURE;
	len = AdRtbEngineRun(pCache->Get(), pCache->Size(), pCache->Max(), ctTask);
	if(len > 0)
	{
		pCache->Set(len);
		pSession->AdSessionWrite(*pCache);
	}
	return AD_SUCCESS;
}
