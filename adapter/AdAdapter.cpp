/***************************************
*	Description: Adapter madule, transfer protocol  send for ibiding
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <errno.h>
#include "AdAdapter.h"
#include "AdUdp.h"
#include "AdParaList.h"
#include "AdRtbNetProtocol.h"

AdAdapter::AdAdapter():m_ctFreePara(AD_KBYTES4 * 400)
{
	m_pEngine = NULL;
	string sName = "Adapter";
	SetName(sName);
}

AdAdapter::~AdAdapter()
{
	if(m_pEngine != NULL)
	{
		delete m_pEngine;
		m_pEngine = NULL;
	}

	AdAdapterPara * pPara;
	while(m_ctFreePara.tryPop(pPara))
	{
		delete pPara;
	}
}

void AdAdapter::AdAdapterAddStatus(AdTask & ctTask)
{
	AdAdapterTaskStatus * pStatus;
	if(ctTask.m_pState == NULL)
	{
		pStatus= new AdAdapterTaskStatus;
		if(pStatus == NULL)
		{
			AD_ERROR("Task State Alloc Error\n");
			return;
		}
		ctTask.m_pState = static_cast<AdTaskState *>(pStatus);
	}
}

AdAdapterPara *AdAdapter:: AllocPara()
{
	AdAdapterPara * pPara = NULL;
	if(m_ctFreePara.tryPop(pPara))
	{
		pPara->clear();
	}
	else
	{
		pPara = new AdAdapterPara;
		if(pPara == NULL)
		{
			AD_ERROR("Alloc Parameter Error\n");
		}
	}
	
	return pPara;
}

void  AdAdapter::FreePara(AdAdapterPara *pPara)
{
	if(!m_ctFreePara.tryPush(pPara))
	{
		delete pPara;
	}
}


int AdAdapter::AdAdapterInit(string & sConf, bool m_bBidding)
{
	int ret;
	ADCONFIG_ITER iter;
	if(!sConf.empty())
		m_sConfigFile = sConf;

	AdRtbConfig & ctConf = m_ctConfs.GetBack();

	cout<<"Adapter Loading config......................................"<<endl;
	ret = ctConf.Load(m_sConfigFile);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Config load error\n");
		return ret;
	}
	ctConf.Check();

	cout<<"Adapter Loading Ban list...................................."<<endl;
	string sSec = "[GLOBAL]";
	ADCONFIG & gConf = ctConf.Get(sSec);
	ret = m_ctBanFilter.Init(gConf["REDIS"]);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Ban Filter Init error\n");
		return ret;
	}

	cout<<"Adapter Init Method  ...................................."<<endl;
	m_ctMethod.Switch();
	Time::wait(1);
	ret = m_ctMethod.GetBack().Init(ctConf);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Adapter Method Init error\n");
		return ret;
	}
	m_ctMethod.Switch();

	cout<<"Adapter Init IpArea  ...................................."<<endl;
	string sFile = "AREA";
	if(ctConf.FileIsChanged(sFile))
	{
		string sSec = "[GLOBAL]";
		ret = m_ctIpArea.GetBack().Init(ctConf.Get(sSec)["AREA"]);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("Adapter IpArea Init error\n");
			return ret;
		}
		m_ctIpArea.Switch();		
	}

	cout<<"Adapter Init ProcessMsg  ...................................."<<endl;
	string path = AD_PROCESS_MSG_PATH;
	ret = m_ctProcessMsg.AdProcessMsgInit(path, AD_PROCESS_MSG_KEY,  AD_PROCESS_MSG_NUM);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Process Msg Error\n");
		return ret;
	}

	if(m_bBidding)
	{
		ret = AdAdapterBiddingInit();
	}

	map<string, int>  &ctDebugUser = m_ctDebugUser.GetBack();
	sSec = "[DEBUGUSER]";
	ADCONFIG & config = ctConf.Get(sSec);
	for(iter = config.begin(); iter != config.end(); iter ++)
	{
		ctDebugUser[iter->second] = atoi(iter->first.c_str());
	}
	m_ctDebugUser.Switch();
	m_ctConfs.Switch();
	return ret;
}

int AdAdapter::AdAdapterBiddingInit()
{
	int ret;
	AdRtbConfig & ctConf = m_ctConfs.Get();
	string sSec = "[GLOBAL]";
	ADCONFIG & gConf = ctConf.Get(sSec);
	if((!gConf["ENGINECONF"].empty()) && (m_pEngine == NULL))
	{
		cout<<"Adapter Init AdRtbEngine ...................................."<<endl;
		m_pEngine = new AdRtbEngine;
		if(m_pEngine == NULL)
		{
			AD_ERROR("Mem Not enough\n");
			return AD_FAILURE;
		}
		ret = m_pEngine->AdRtbEngineInit(gConf["ENGINECONF"]);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("AdRtbEngine init error\n");
			delete m_pEngine;
			m_pEngine = NULL;
			return AD_FAILURE;
		}
	
		ret = m_pEngine->AdRtbEngineUpdate();
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("AdRtbEngine Update error\n");
			delete m_pEngine;
			m_pEngine = NULL;
			return AD_FAILURE;
		}
	}
	return AD_SUCCESS;
}

int AdAdapter::AdAdapterUpdate(void)
{
	int ret;
	AdRtbConfig & ctConf = m_ctConfs.Get();
	string sSec = "[GLOBAL]";
	ADCONFIG & gConf = ctConf.Get(sSec);
	ret = ctConf.Check();
	if(ret  > 0)
	{
		AD_INFO("Adapter Update Config\n");
		string sConf;
		ret = AdAdapterInit(sConf);
	}
	if(m_pEngine != NULL)
	{
		ret = m_pEngine->AdRtbEngineUpdate();
		if(ret == AD_RTB_ENGINE_REINIT)
		{
			ret = m_pEngine->AdRtbEngineInit(gConf["ENGINECONF"]);
			if(ret != AD_SUCCESS)
			{
				AD_ERROR("AdRtbEngine init error\n");
				return ret;
			}
			ret = m_pEngine->AdRtbEngineUpdate();
			if(ret != AD_SUCCESS)
			{
				AD_ERROR("AdRtbEngine Update error\n");
				return ret;
			}
		}
	}
	return ret;
}

AdRtbEngine *AdAdapter::AdAdapterGetEngine()
{
	return m_pEngine;
}


int AdAdapter::AdAdapterPutMsg(AdAdapterPara& ctPara)
{
	int ret;
	//put msg for record
	if ((ctPara.m_ctCacheReq.m_ucType == AD_MSG_TYPE_REQUEST) 
		|| (ctPara.m_ctCacheReq.m_ucType == AD_MSG_TYPE_REQUEST_BIDDING))
	{
		ret = m_ctProcessMsg.AdProcessMsgPut(static_cast<void *>(&ctPara.m_ctCacheReq), sizeof(AdLogMsg));
	}

	//put msg for record
	if (ctPara.m_ctCacheRes.m_ucType == AD_MSG_TYPE_RESPONSE) 
	{
		ret = m_ctProcessMsg.AdProcessMsgPut(static_cast<void *>(&ctPara.m_ctCacheRes), sizeof(AdLogMsg));
	}
	return AD_SUCCESS;
}

int AdAdapter::AdAdapterIBiddingResponse(AdCache * pCache, AdSession *pSession, AdTask & ctTask)
{
	int ret;
	bool bTlv;
	bool bBid = false;
	AdAdapterPara * pPara;
	AdParaList *pList;

	// check session is ibiding or not
	if(m_ctLocalSession.check(static_cast<void *> (pSession))) 
	{
		bBid = true;
		bTlv = false;
	}
	else if(m_ctNetSession.check(static_cast<void *> (pSession)))
	{
		bBid = true;
		bTlv = true;
	}

	////ibiding Session, check info, if timeout, return empty
	if(bBid == true)
	{
		//session error, clear info,return
		 if(!pSession->AdSessionOk())
		 {
		 	//get Para list delete
		 	pList = static_cast<AdParaList *>(pSession->AdSessionGetPrivate(true));
			if(pList != NULL)
			{
		 		pList->clear(this, ctTask);
				delete pList;
			}
			
			//BidSession error, delete;
			if(bTlv)
				m_ctNetSession.del(static_cast<void *> (pSession));
			else
				m_ctLocalSession.del(static_cast<void *> (pSession));
			pSession->AdSessionIdle();
			return AD_SUCCESS;
		 }
		 pList = static_cast<AdParaList *>(pSession->AdSessionGetPrivate());
		 //check session info, timeout 
		 pList->check(this,ctTask);

		//no data session,  idle msg, return.
		 if(pCache == NULL)
		 {
		 	return AD_SUCCESS;
		 }


		 //ibidding session,  deal return packet	
		AdSession * pAdxSession;
		//Local ibiding
		if(bTlv == false)
		{
			struct InternalPacket * pInter = (struct InternalPacket *)pCache->Get();
			int iPacket = pCache->Size()/sizeof(struct InternalPacket);
			while(iPacket > 0)
			{
				if(pInter->uiFlags != AD_INTER_PACKET_FLAGS)
				{
					iPacket --;
					pInter ++;
					continue;
				}
				pPara = static_cast<AdAdapterPara *>(pInter->pData);
				//delete session info
				ret = pList->del( pPara);
				if(ret == AD_SUCCESS)
				{
					pSession->AdSessionIdle();
					pAdxSession = static_cast<AdSession *>(pPara->m_pSrcSession);
					if(pPara->m_bTimeout == false)
					{
						//build http packet
						ret = AdAdapterToHttp(*pPara, ctTask, bTlv);
						if(ret > 0)
						{
							pAdxSession->AdSessionWrite(pPara->m_ctCacheOut);
							pAdxSession->AdSessionIdle();
						}
						else
						{
							ret = SessionTimeout(*pPara, ctTask, pAdxSession);
						}
					 }
					FreePara(pPara);
				}
				else
				{
					AD_ERROR("Bid response packet error, no found\n");
				}		
				iPacket --;
				pInter ++;
			}
		}
		else //net ibidding  use tlv protocol
		{
			int iOffset = 0;
			AdRtbProtocol ctProtocol;
			int iLen;
			do
			{
				iLen = ctProtocol.TlvPacketLen(pCache->Get() + iOffset, pCache->Size() - iOffset);
				//delete session info
				pPara = pList->pop();
				if((pPara != NULL) && (iLen > 0))
				{
					pAdxSession = static_cast<AdSession *>(pPara->m_pSrcSession);
					pSession->AdSessionIdle();

					//send response
					if(pPara->m_bTimeout == false)
					{
						memcpy(pPara->m_ctCacheRes.m_uMsg, pCache->Get() + iOffset, pCache->Size() - iOffset);
						pPara->m_ctCacheRes.m_iLength = pCache->Size() - iOffset;
						
						//build http packet
						ret = AdAdapterToHttp(*pPara, ctTask, bTlv);
						if(ret > 0)
						{
							pAdxSession->AdSessionWrite(pPara->m_ctCacheOut);
							pAdxSession->AdSessionIdle();
						}
						else
						{
							ret = SessionTimeout(*pPara, ctTask, pAdxSession);
						}
					}
					FreePara(pPara); 
					iOffset += iLen;
				}
				else
				{
					break;
				}
			}while(iOffset < pCache->Size());
		}

		if(pList->size() == 0)
			pSession->AdSessionSetTimeout(0);
		
		return AD_SUCCESS;
	}

	 if(pCache == NULL)
	{
		 return AD_SUCCESS;
	}
	if(pCache->Size()<= 32)
	{
		//find ibiding device
		if(AD_SUCCESS == strncmp(pCache->Get(),  AD_IBIDDING_ENG_LOCAL, strlen(AD_IBIDDING_ENG_LOCAL)))
		{
			pList = new AdParaList;
			if(pList == NULL)
			{
				AD_ERROR("Local Session Init Error\n");
				pSession->AdSessionClose();
				return AD_FAILURE;
			}
			pSession->AdSessionSetPrivate(static_cast<void *>(pList));
			pSession->AdSessionActive();
			m_ctLocalSession.push(static_cast<void *>(pSession));
			return AD_SUCCESS;
		}

		if(AD_SUCCESS == strncmp(pCache->Get(), AD_IBIDDING_ENG_REMOTE, strlen(AD_IBIDDING_ENG_REMOTE)))
		{
			pList = new AdParaList;
			if(pList == NULL)
			{
				AD_ERROR("Remote Session Init Error\n");
				pSession->AdSessionClose();
				return AD_FAILURE;
			}
			pSession->AdSessionSetPrivate(static_cast<void *>(pList));
			pSession->AdSessionActive();
			m_ctNetSession.push(static_cast<void *>(pSession));
			return AD_SUCCESS;
		}
	}
	return AD_FAILURE;
}

int AdAdapter::SessionTimeout(AdAdapterPara & ctPara, AdTask & ctTask, AdSession * pSession)
{
	int ret;

	if(ctPara.m_bTimeout)
	{
		AdAdapterTaskStatus * pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
		if(pState == NULL)
		{
			AdAdapterAddStatus(ctTask);
			pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
		}
		
		if(pState != NULL)
			pState->StatusAddTimeout();
	}

	//Get adx Method
	AdProtocolBase * pTrans = static_cast<AdProtocolBase *>(ctPara.m_pAdxMethod);
	if(pTrans == NULL)
	{
		goto RETURN;
	}
	
	//empty msg, return
	ret = pTrans->EmptyToHttp(&ctPara);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Protocol error\n");
		goto RETURN;
	}
	//print output info
	if((AD_ADAPTER_PRINT_OUTPUT & ctTask.m_uiPrint) != 0)
	{
		AD_PRINT("AD  Adapter Output Data:%d---Ms: %d---Ignore--------- \n", ctPara.m_ctCacheOut.Size(),
			ctPara.m_ctRequest.getLife());
		AD_PRINT("%s\n", ctPara.m_ctCacheOut.Get());
	}
	AdAdapterPutMsg(ctPara);

	if(pSession != NULL)
	{
		pSession->AdSessionWrite(ctPara.m_ctCacheOut);
	}
	
RETURN:
	if(pSession != NULL)
	{
		pSession->AdSessionIdle();
	}
	return AD_SUCCESS;
}


int AdAdapter::AdAdapterToInternal(AdAdapterPara & ctPara, AdTask & ctTask)
{
	int ret;

	AdRtbProtocol  ctTlvProtocol;

	map<string, string>::iterator iter; 

	AdAdapterTaskStatus * pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
	if(pState == NULL)
	{
		AdAdapterAddStatus(ctTask);
		pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
	}

	if(pState != NULL)
		pState->StatusAdd();
	
	//Get method
	ret = m_ctMethod.Get().Get(ctPara);
	if (ret != AD_SUCCESS)
	{
		AD_ERROR("Method Get  error  %d\n", ctPara.m_ctRequest.m_uiAdx);
		return AD_FAILURE;
	}

	AdProtocolBase * pTrans =  static_cast<AdProtocolBase *>(ctPara.m_pAdxMethod);
	if(pTrans == NULL)
	{
		AD_ERROR("Method  error  %d\n", ctPara.m_ctRequest.m_uiAdx);
		return AD_FAILURE;
	}
	
	//print msg
	if((AD_ADAPTER_PRINT_INPUT & ctTask.m_uiPrint) != 0)
	{
		AD_PRINT("AD  Adapter Input Data:%d--------------------\n", ctPara.m_pCacheIn->Size());
		AD_PRINT("%s\n", ctPara.m_pCacheIn->Get());
	}
	
	//http to internal
	ret = pTrans->HttpToInternal(&ctPara);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Protocol error\n");
		return AD_FAILURE;
	}

	//Ip to Area transfer
	ret = m_ctIpArea.Get().Get(ctPara.m_ctRequest.m_sUserIP, ctPara.m_ctRequest.m_sArea);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Ip to Area error\n");
	}
	//if area empty,  default all
	if(ctPara.m_ctRequest.m_sArea.empty())
	{
		ctPara.m_ctRequest.m_sArea = AD_AREA_ALL;
	} 
	
	//store request num
	if(pState != NULL)
		pState->StatusAddRequest();
	
	//tag request ok
	ctPara.m_ctCacheReq.m_ucType = AD_MSG_TYPE_REQUEST;
	
	//internal to tlv
	ctPara.m_ctCacheReq.m_iLength = ctTlvProtocol.InternalToTlv(ctPara.m_ctRequest,
		ctPara.m_ctCacheReq.m_uMsg, AD_KBYTES8,
		(AD_ADAPTER_PRINT_REQUEST& ctTask.m_uiPrint) != 0);
	if(ctPara.m_ctCacheReq.m_iLength <= 0)
	{
		AD_ERROR("Internal Protocol error\n");
		return AD_FAILURE;
	}
	
	//check debug user
	map<string, int>  &ctDebugUser = m_ctDebugUser.Get();
	if(ctDebugUser.find(ctPara.m_ctRequest.m_sUserIP) != ctDebugUser.end())
	{
		ctPara.m_bDebugUser = true;
	}

	//match ban list
	if ( m_ctBanFilter.Match(&ctPara.m_ctRequest))
	{
		return AD_FAILURE;	
	}

	return AD_SUCCESS;
}

int AdAdapter::AdAdapterToHttp(AdAdapterPara & ctPara, AdTask & ctTask, bool bTlv)
{
	int ret;
	AdRtbProtocol  ctTlvProtocol;
	AdProtocolBase * pTrans;

	//Get adx Method
	pTrans =  static_cast<AdProtocolBase *>(ctPara.m_pAdxMethod);
	if(pTrans == NULL)
	{
		AD_ERROR("Method  error  %d\n", ctPara.m_ctRequest.m_uiAdx);
		return AD_FAILURE;
	}

	//find ibiding address
	if (bTlv)
	{
		//tvl to internal protocol
		ret = ctTlvProtocol.TlvToInternal(ctPara.m_ctResponse,ctPara.m_ctCacheRes.m_uMsg,
			ctPara.m_ctCacheRes.m_iLength, (AD_ADAPTER_PRINT_RESPONSE & ctTask.m_uiPrint) != 0);
		if(ret !=  AD_SUCCESS)
		{
			//AD_ERROR("Internal Protocol error\n");
			return AD_FAILURE;;
		}
	}
	else
	{
		if(ctPara.m_ctResponse.m_uiAdx == 0)
			return AD_FAILURE;
		if((AD_ADAPTER_PRINT_RESPONSE & ctTask.m_uiPrint) != 0)
			AD_PRINT("Bid Time %d\n", ctPara.m_ctRequest.getLife());
			
		//internal to tlv; log msg needed
		ctPara.m_ctCacheRes.m_iLength = ctTlvProtocol.InternalToTlv(ctPara.m_ctResponse,ctPara.m_ctCacheRes.m_uMsg,
			AD_KBYTES8, (AD_ADAPTER_PRINT_RESPONSE & ctTask.m_uiPrint) != 0);
		if(ctPara.m_ctCacheRes.m_iLength <= 0)
		{
			//AD_ERROR("Internal Protocol error\n");
			return AD_FAILURE;;
		}
	}
	
	//internal to http
	ret = pTrans->InternalToHttp(&ctPara);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Protocol Error\n");
		return AD_FAILURE;;
	}

	//get status
	AdAdapterTaskStatus * pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
	if(pState == NULL)
	{
		AdAdapterAddStatus(ctTask);
		pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
	}
	//update status
	if(pState != NULL)
		pState->StatusAddResponse();
	
	//print output info
	if((AD_ADAPTER_PRINT_OUTPUT & ctTask.m_uiPrint) != 0)
	{
		AD_PRINT("AD  Adapter Output Data:%d---Ms: %d---Bidding--------- \n", ctPara.m_ctCacheOut.Size(),
			ctPara.m_ctRequest.getLife());
		AD_PRINT("%s\n", ctPara.m_ctCacheOut.Get());
	}

	ctPara.m_ctCacheReq.m_ucType = AD_MSG_TYPE_REQUEST_BIDDING;
	ctPara.m_ctCacheRes.m_ucType = AD_MSG_TYPE_RESPONSE;
	AdAdapterPutMsg(ctPara);
	return ctPara.m_ctCacheOut.Size();
}

/*************Net Interface************************/
int AdAdapter::Run(AdCache * pCache, AdSession * pSession, AdTask & ctTask)
{
	int ret, i;
	void * pData;
	AdSession * pBidSession = NULL;
	bool bRemote;

	AdParaList * pList;

	//check ibiding session
	ret = AdAdapterIBiddingResponse(pCache, pSession, ctTask);
	if(ret == AD_SUCCESS)
		return ret;

	//new request, allac para
	AdAdapterPara * pPara = AllocPara();
	if(pPara == NULL)
	{
		AD_ERROR("Mem not enough, parameter new error\n");
		return AD_FAILURE;
	}
	//init Para Input cache
	pPara->m_pCacheIn = pCache;
	
	//lock session
	pSession->AdSessionActive();

	//http to internal struct
	ret = AdAdapterToInternal(*pPara, ctTask);
	if(ret !=  AD_SUCCESS)
	{
		//get status
		AdAdapterTaskStatus * pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
		if(pState == NULL)
		{
			AdAdapterAddStatus(ctTask);
			pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
		}
		//update status
		if(pState != NULL)
			pState->StatusAddError();
		goto RETURN;
	}

       //Find Ibiding Session
	for(i = m_ctLocalSession.size()/2;  i >= 0 ; i --)
   	{
		if(pPara->m_bDebugUser)
		{
			//debug user, remote ibiding First
			pData = m_ctNetSession.find(ctTask.m_uiLoop);
			if(pData != NULL)
			{
				pBidSession = static_cast<AdSession * >(pData);
				bRemote = true;
			}
			else
			{
				pData = m_ctLocalSession.find(ctTask.m_uiLoop);
				if(pData != NULL)
				{
					pBidSession = static_cast<AdSession * >(pData);
					bRemote = false;
				}
			}
		}
		else
		{
			//Normal local ibiding First
			pData = m_ctLocalSession.find(ctTask.m_uiLoop);
			if(pData != NULL)
			{
				pBidSession = static_cast<AdSession * >(pData);
				bRemote = false;
			}
			else
			{
				pData = m_ctNetSession.find(ctTask.m_uiLoop);
				if(pData != NULL)
				{
					pBidSession = static_cast<AdSession * >(pData);
					bRemote = true;
				}
			}
		}

		//loop count
		ctTask.m_uiLoop ++;
		
		//no found pBidSession
		if(pBidSession == NULL)
		{
					//get status
			AdAdapterTaskStatus * pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
			if(pState == NULL)
			{
				AdAdapterAddStatus(ctTask);
				pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
			}
			//update status
			if(pState != NULL)
				pState->StatusAddError();
			goto RETURN;
		}

		pList = static_cast<AdParaList *>(pBidSession->AdSessionGetPrivate());
		//check Para list exist
		if(pList == NULL)
		{
			AD_ERROR("Bid Session Error, no Para list, close\n");
			pBidSession->AdSessionClose();
					//get status
			AdAdapterTaskStatus * pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
			if(pState == NULL)
			{
				AdAdapterAddStatus(ctTask);
				pState = static_cast<AdAdapterTaskStatus *>(ctTask.m_pState);
			}
			//update status
			if(pState != NULL)
				pState->StatusAddError();
			goto RETURN;
		}

		//check session ok, disconnect, clear Para list
		if(!pBidSession->AdSessionOk())
		{
			AD_ERROR("Bid Session Connect Error\n");
			goto RETURN;
		}

		if(pList->size() == 0)
		{
			break;
		}
       	}

	//check Para list, timeout no send
	if( i >= 0)
	{
		//up session  active counter,  wait response
		pBidSession->AdSessionActive();
		pBidSession->AdSessionSetTimeout(3);
		pPara->m_pSrcSession = static_cast<void *>(pSession);
		pPara->m_pDstSession = static_cast<void *>(pBidSession);
		pList->push(pPara);
		//send data for ibidding
		if(bRemote)
		{
			//if(pList->check(this, ctTask) == 0)
			pBidSession->AdSessionWrite(pPara->m_ctCacheReq.m_uMsg, pPara->m_ctCacheReq.m_iLength);
		}
		else
		{
			struct InternalPacket ctInter;
			ctInter.uiFlags = AD_INTER_PACKET_FLAGS;
			ctInter.pRequest = &(pPara->m_ctRequest);
			ctInter.pResponse= &(pPara->m_ctResponse);
			ctInter.pData = static_cast<void *>(pPara);
			pBidSession->AdSessionWrite((char *)(&ctInter), sizeof(ctInter));
		}
		return AD_SUCCESS;
	}
RETURN:
	ret = SessionTimeout(*pPara, ctTask, pSession);
	FreePara(pPara);
	return AD_FAILURE;
}

int AdAdapter::PacketLen(AdCache * pCache)
{
	char * pBody;
	char * pData;
	int iBody;
	int ret;
	
	pData =pCache->Get();
	string sValue;
	//check http head
	ret = AdHttp::GetPostUri(*pCache, sValue);
	if (AD_SUCCESS == ret) 
	{
		iBody = AdHttp::GetLen(*pCache);
		if(iBody <= 0)
			return AD_FAILURE;

		pBody = AdHttp::GetBody(*pCache);
		if (NULL == pBody) 
		{
			return AD_FAILURE;;
		}

		return pBody - pData + iBody;
	}
	return AD_FAILURE;
}
