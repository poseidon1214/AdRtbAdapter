/***************************************
*	Description: ssh madule, transfer protocol  send for adx
*	Author: wilson
*	Date: 2016/03/03
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <errno.h>
#include "AdAdx.h"
#include "AdParaList.h"
#include "json/json.h"
#include "AdAes.h"

AdAdx::AdAdx(AdMult * pMult):m_ctFreePara(AD_KBYTES4 * 400)
{
	string sName = "Ssp";
	SetName(sName);

	m_pMult = pMult;
}

AdAdx::~AdAdx()
{
	AdRtbPara * pPara;
	while(m_ctFreePara.tryPop(pPara))
	{
		delete pPara;
	}
}

void AdAdx::AddStatus(AdTask & ctTask)
{
	AdAdxTaskStatus * pStatus;
	if(ctTask.m_pState == NULL)
	{
		pStatus= new AdAdxTaskStatus;
		if(pStatus == NULL)
		{
			AD_ERROR("Task State Alloc Error\n");
			return;
		}
		ctTask.m_pState = static_cast<AdTaskState *>(pStatus);
	}
}

AdRtbPara *AdAdx:: AllocPara()
{
	AdRtbPara * pPara = NULL;
	if(m_ctFreePara.tryPop(pPara))
	{
		pPara->clear();
	}
	else
	{
		pPara = new AdRtbPara;
		if(pPara == NULL)
		{
			AD_ERROR("Alloc Parameter Error\n");
		}
	}
	
	return pPara;
}

void  AdAdx::FreePara(AdRtbPara *pPara)
{
	if(!m_ctFreePara.tryPush(pPara))
	{
		delete pPara;
	}
};


int AdAdx::Init(string & sConf)
{
	int ret;
	ADCONFIG_ITER iter;
	if(!sConf.empty())
		m_sConfigFile = sConf;

	AdRtbConfig & ctConf = m_ctConfs.GetBack();

	cout<<"Adx Loading config......................................"<<endl;
	ret = ctConf.Load(m_sConfigFile);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("Config load error\n");
		return ret;
	}
	ctConf.Check();

	ret = m_cSsp.Init(ctConf);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("SspData Init error\n");
		return ret;
	}

	map<string, int>  &ctDebugUser = m_ctDebugUser.GetBack();
	string sSec = "[DEBUGUSER]";
	ADCONFIG & debug = ctConf.Get(sSec);
	for(iter = debug.begin(); iter != debug.end(); iter ++)
	{
		ctDebugUser[iter->second] = atoi(iter->first.c_str());
	}
	m_ctDebugUser.Switch();
	
	sSec = "[DSPSERVER]";
	ADCONFIG & dspServer = ctConf.Get(sSec);

	AdSession * pSession;
	ADCONFIG & sOldServer = m_cDspServer.GetBack();
	for(iter = dspServer.begin(); iter != dspServer.end(); iter ++)
	{
		if(sOldServer.find(iter->second) == sOldServer.end())
		{
			string  sServer = iter->second;
			pSession = m_pMult->CreateSession(sServer);
			if(pSession == NULL)
			{
				AD_ERROR("CreateSession Fail\n");
				return AD_FAILURE;
			}
			AdParaList<AdRtbPara, AdAdx> * pList = new AdParaList<AdRtbPara, AdAdx>;
			if(pList == NULL)
			{
				AD_ERROR("DSP Session Init Error\n");
				pSession->AdSessionClose();
				return AD_FAILURE;
			}
			pSession->AdSessionSetPrivate(static_cast<void *>(pList));
			pSession->AdSessionActive();
			m_ctDspSession.push(static_cast<void *>(pSession));
		}
	}

	for(iter = sOldServer.begin(); iter != sOldServer.end(); iter ++)
	{
		if(dspServer.find(iter->second) == dspServer.end())
		{
			string  sServer = iter->second;
			m_pMult->DelSession(sServer);
		}
	}
	sOldServer = dspServer;
	
	m_cDspServer.Switch();
	
	m_ctConfs.Switch();
	return ret;
}

int AdAdx::Update(void)
{
	int ret;
	AdRtbConfig & ctConf = m_ctConfs.Get();
	ret = ctConf.Check();
	if(ret  > 0)
	{
		if(ctConf.FileIsChanged(m_sConfigFile))
		{
			AD_INFO("Adapter Update Config\n");
			string sConf;
			ret = Init(sConf);
		}
	}
	else
	{

		//update ssp data
		ret = m_cSsp.Update();
	
		//check Dsp session ok, if not,  reconnect
		ADCONFIG_ITER iter;
		AdSession * pSession;
		ADCONFIG & sOldServer = m_cDspServer.Get();
		for(iter = sOldServer.begin(); iter != sOldServer.end(); iter ++)
		{
			string  sServer = iter->second;
			pSession = m_pMult->GetSession(sServer);
			if(pSession == NULL)
			{
				pSession = m_pMult->CreateSession(sServer);
				if(pSession == NULL)
				{
					AD_ERROR("CreateSession Fail\n");
					return AD_FAILURE;
				}
				AdParaList<AdRtbPara, AdAdx> * pList = new AdParaList<AdRtbPara, AdAdx>;
				if(pList == NULL)
				{
					AD_ERROR("DSP Session Init Error\n");
					pSession->AdSessionClose();
					return AD_FAILURE;
				}
				pSession->AdSessionSetPrivate(static_cast<void *>(pList));
				pSession->AdSessionActive();
				m_ctDspSession.push(static_cast<void *>(pSession));
			}
		}

	}
	
	return ret;
}

unsigned char AdAdx::ToHex(unsigned char x) 
{ 
	return  x > 9 ? x + 55 : x + 48; 
}

unsigned char AdAdx::FromHex(unsigned char x) 
{ 
	unsigned char y;
	if (x >= 'A' && x <= 'Z') 
		y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') 
		y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') 
			y = x - '0';
   		return y;
};


void AdAdx::UrlEncode(const std::string& str, std::string & sDst)
{
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) || (str[i] == '-') || (str[i] == '_') || 
			(str[i] == '.') || (str[i] == '~'))
		{
			sDst += str[i];
		}
		else if (str[i] == ' ')
			sDst += "+";
		else
		{
 			sDst	 += '%';
			sDst += ToHex((unsigned char)str[i] >> 4);
			sDst += ToHex((unsigned char)str[i] % 16);
		}
	}
};

void AdAdx::UrlDecode(const std::string& str,  std::string & sDst)
{
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+')
			sDst += ' ';
		else if (str[i] == '%')
		{
			if(i + 2 < length)
			{
				unsigned char high = FromHex((unsigned char)str[++i]);
				unsigned char low = FromHex((unsigned char)str[++i]);
				sDst += high*16 + low;
			}
		}
		else 
			sDst += str[i];
	}
};


int AdAdx::SendResponse(AdRtbPara & ctPara, AdAdxTaskStatus * pState)
{
	CHAR8  cTemp[AD_BYTE512];
	CHAR8  cPrice[AD_BYTE256];
	UCHAR  cKey[] = "aitoudsp-20160311";
	AES  cAes(cKey);
	
	DOUBLE fPrice = ctPara.m_ctResponse.m_vSeatBid[0].m_vBid[0].m_fPrice;

	//cpm price, for evevry  PV
	snprintf(cTemp, AD_BYTE512, "%f", fPrice/1000);

	cAes.Bm53Cipher(cTemp, cPrice);
	
	//replace macro
	string sReplace = ctPara.m_ctResponse.m_vSeatBid[0].m_vBid[0].m_sAdm;
	string sResult;
	string sMacro = "%%WINNING_PRICE%%"; 
	string sNew = cPrice;

	AdString::StrReplace(sReplace, sMacro,  sNew, sResult);

	sReplace = sResult;
	sResult.clear();
	sMacro = "%%CLICK_URL_ESC%%";
	snprintf(cTemp, AD_BYTE512, "http://114.113.235.101/click?pid=%s&mid=%d&cu=", 
		ctPara.m_ctRequest.m_vImp[0].m_sTagId.c_str(), ctPara.m_iMid);
	sNew = cTemp;
	string sEnc;
	UrlEncode(sNew, sEnc);
	AdString::StrReplace(sReplace, sMacro,  sEnc, sResult);

	ctPara.m_ctResponse.m_vSeatBid[0].m_vBid[0].m_sAdm = sResult;

	m_cSsp.ToHttp(ctPara);
	
	ctPara.m_pSrcSession->AdSessionWrite(ctPara.m_ctCacheOut);
	
	//update status
	if(pState != NULL)
	{
		pState->StatusAddResponse();
		string sDspId = "AiTou";
		pState->StatusAddCharge(sDspId, fPrice);
		pState->StatusAddPV(ctPara.m_iMid, ctPara.m_ctRequest.m_vImp[0].m_sTagId);
		string sSec = "[CONFIG]";
		pState->StatusUpdate(m_ctConfs.Get().Get(sSec)["RTREDIS"]);
	}

	return AD_SUCCESS;
}

int AdAdx::DspResponse(AdCache * pCache, AdSession *pSession, AdTask & ctTask)
{
	int ret;
	AdParaList<AdRtbPara, AdAdx> *pList;
	AdHttp cHttp;
	AdRtbPara * pPara = NULL;
	AdRtbResponse cRes;
	
	INT32 iLen;
	CHAR8 * pBody;

	Json::Reader reader;
	Json::Value value;

	// check session is Dsp or not
	if(!m_ctDspSession.check(static_cast<void *> (pSession))) 
	{
		//no data session,  idle msg, return.
		return AD_FAILURE;
	}

	//get status
	AdAdxTaskStatus * pState = static_cast<AdAdxTaskStatus *>(ctTask.m_pState);
	if(pState == NULL)
	{
		AddStatus(ctTask);
		pState = static_cast<AdAdxTaskStatus *>(ctTask.m_pState);
	}
	
	//session error, clear info,return
	if(!pSession->AdSessionOk())
	{
		//get Para list delete
		pList = static_cast<AdParaList<AdRtbPara, AdAdx> *>(pSession->AdSessionGetPrivate(true));
		if(pList != NULL)
		{
			pList->clear(this, ctTask);
			delete pList;
		}
				
		//DspSession error, delete;
		m_ctDspSession.del(static_cast<void *> (pSession));
		pSession->AdSessionIdle();
		return AD_SUCCESS;
	}
	
	pList = static_cast<AdParaList<AdRtbPara, AdAdx> *>(pSession->AdSessionGetPrivate());
	//check session info, timeout 
	pList->check(this,ctTask, true);

	//no data session,  idle msg, return.
	if(pCache == NULL)
	{
		return AD_SUCCESS;
	}

	ret = cHttp.ParseResponse(*pCache);
	if(ret == AD_FAILURE)
	{
		//update status
		if(pState != NULL)
			pState->StatusAddResError();
		goto RETURN;
	}

	if(!cHttp.CheckResState(HTTP_OK))
	{
	 	goto RETURN;
	}

	//get body data, check length
	iLen = cHttp.GetLen();
	pBody = cHttp.GetBody(true);
	if((pBody == NULL) || (iLen <= 0))
	{
		//update status
		if(pState != NULL)
			pState->StatusAddResError();
		goto RETURN;
	}

	if (true != reader.parse(pBody, value))
	{
		//update status
		if(pState != NULL)
			pState->StatusAddResError();
		goto RETURN;
	}

	cRes.parseJson(value);

	pPara = pList->del(atol(cRes.m_vSeatBid[0].m_vBid[0].m_sImpId.c_str()));
	if(pPara == NULL)
	{
		//update status
		if(pState != NULL)
			pState->StatusAddSysError();
		goto RETURN;
	}
	//parse Json
	pPara->m_ctResponse.parseJson(value);

	//build Response, 
	SendResponse(*pPara, pState);
	return AD_SUCCESS;

RETURN:
	return AD_SUCCESS;
}

int AdAdx::SessionTimeout(AdRtbPara & ctPara, AdTask & ctTask, AdSession * pSession)
{

	if(ctPara.m_bTimeout)
	{
		AdAdxTaskStatus * pState = static_cast<AdAdxTaskStatus *>(ctTask.m_pState);
		if(pState == NULL)
		{
			AddStatus(ctTask);
			pState = static_cast<AdAdxTaskStatus *>(ctTask.m_pState);
		}
		
		if(pState != NULL)
			pState->StatusAddTimeout();
	}

	m_cSsp.EmptyToHttp(ctPara);

	//print output info
	if((AD_ADX_PRINT_OUTPUT & ctTask.m_uiPrint) != 0)
	{
		AD_PRINT("AD  ADX Output Data:%d---Ms: %d---Ignore--------- \n", ctPara.m_ctCacheOut.Size(),
			ctPara.m_ctRequest.getLife());
		AD_PRINT("%s\n", ctPara.m_ctCacheOut.Get());
	}

	if(pSession != NULL)
	{
		pSession->AdSessionWrite(ctPara.m_ctCacheOut);
		pSession->AdSessionIdle();
	}
	
	return AD_SUCCESS;
}

int AdAdx::RedirectLandingPage(AdRtbPara & ctPara, AdSession * pSession, AdTask & ctTask)
{
	int ret;

	map<string, string>::iterator iter;

	string sUri;

	ret = ctPara.m_cHttp.GetUri(AD_FAILURE, sUri);
	if((ret != AD_SUCCESS) || (sUri != "/click"))
	{
		return AD_FAILURE;
	}

	string sPid;
	//http Para
	ret = ctPara.m_cHttp.GetPara("pid", sPid);
	if((ret != AD_SUCCESS) && ( sPid.empty()))
	{
		return AD_FAILURE;
	}

	string sMid;
	//http Para
	ret = ctPara.m_cHttp.GetPara("mid", sMid);
	if((ret != AD_SUCCESS) && ( sMid.empty()))
	{
		return AD_FAILURE;
	}

	string sLandingPage;
	//http Para
	ret = ctPara.m_cHttp.GetPara("cu", sLandingPage);
	if((ret != AD_SUCCESS) && ( sLandingPage.empty()))
	{
		return AD_FAILURE;
	}

	//get status
	AdAdxTaskStatus * pState = static_cast<AdAdxTaskStatus *>(ctTask.m_pState);
	if(pState == NULL)
	{
		AddStatus(ctTask);
		pState = static_cast<AdAdxTaskStatus *>(ctTask.m_pState);
	}

	if(pState != NULL)
		pState->StatusAddClick(atoi(sMid.c_str()), sPid);

	string sDec;
	UrlDecode(sLandingPage, sDec);
	if(sDec.empty())
	{
		if(pState != NULL)
			pState->StatusAddReqError();
	}

	AdCache ctCache;
	ctPara.m_cHttp.GetRedirect(HTTP_MOV_TEMP, sDec, ctPara.m_ctCacheOut);

	pSession->AdSessionWrite(ctPara.m_ctCacheOut);
	
	return AD_SUCCESS;
}


/*************Net Interface************************/
int AdAdx::Run(AdCache * pCache, AdSession * pSession, AdTask & ctTask)
{
	INT32 ret;
	
	//get status
	AdAdxTaskStatus * pState = static_cast<AdAdxTaskStatus *>(ctTask.m_pState);
	if(pState == NULL)
	{
		AddStatus(ctTask);
		pState = static_cast<AdAdxTaskStatus *>(ctTask.m_pState);
	}

	//check ibiding session
	ret = DspResponse(pCache, pSession, ctTask);
	if(ret == AD_SUCCESS)
		return ret;
	
	if(pCache == NULL)
	{
		return AD_SUCCESS;
	}

	//new request, allac para
	AdRtbPara * pPara = AllocPara();
	if(pPara == NULL)
	{
		AD_ERROR("Mem not enough, parameter new error\n");
		return AD_FAILURE;
	}
	//init Para Input cache
	pPara->m_pCacheIn = pCache;

	ret = pPara->m_cHttp.Parse(*(pPara->m_pCacheIn));
	if (ret == AD_FAILURE) 
	{
		string & sStatus = pPara->m_cHttp.GetStatus();
		AD_ERROR("http Parse  error  %s\n", sStatus.c_str());
		//update status
		if(pState != NULL)
			pState->StatusAddReqError();
		goto RETURN;
	}
	
	//http to internal struct
	ret = RedirectLandingPage(*pPara, pSession, ctTask);
	if(ret ==  AD_SUCCESS)
	{
		return  AD_SUCCESS;
	}
	
       ret = SendDsp(pPara, pSession, ctTask);
	return ret;
	
RETURN:
	ret = SessionTimeout(*pPara, ctTask, pSession);
	FreePara(pPara);
	return AD_FAILURE;
}

/*************Net Interface************************/
int AdAdx::SendDsp(AdRtbPara * pPara, AdSession * pSession, AdTask & ctTask)
{
	int ret, i;
	void * pData;
	AdSession * pDspSession = NULL;
	AdCache cOutDsp;

	AdParaList<AdRtbPara, AdAdx> * pList;
	
	//get status
	AdAdxTaskStatus * pState = static_cast<AdAdxTaskStatus *>(ctTask.m_pState);
	if(pState == NULL)
	{
		AddStatus(ctTask);
		pState = static_cast<AdAdxTaskStatus *>(ctTask.m_pState);
	}
	
	//lock session
	pSession->AdSessionActive();

	string sIp;

	pSession->GetIp(sIp);
	
	//http to internal struct
	ret = m_cSsp.ToInternal(*pPara, sIp);
	if(ret !=  AD_SUCCESS)
	{
		//update status
		if(pState != NULL)
			pState->StatusAddReqError();
		goto RETURN;
	}
	
       //Find Ibiding Session
	for(i = m_ctDspSession.size()/2;  i >= 0 ; i --)
   	{
		//debug user, remote ibiding First
		pData = m_ctDspSession.find(ctTask.m_uiLoop);
		if(pData != NULL)
		{
			pDspSession = static_cast<AdSession * >(pData);
		}

		//loop count
		ctTask.m_uiLoop ++;
		
		//no found pBidSession
		if(pDspSession == NULL)
		{
			//update status
			if(pState != NULL)
				pState->StatusAddSysError();
			goto RETURN;
		}

		pList = static_cast<AdParaList<AdRtbPara, AdAdx> *>(pDspSession->AdSessionGetPrivate());
		//check Para list exist
		if(pList == NULL)
		{
			AD_ERROR("Bid Session Error, no Para list, close\n");
			pDspSession->AdSessionClose();
			//update status
			if(pState != NULL)
				pState->StatusAddSysError();
			goto RETURN;
		}

		//check session ok, disconnect, clear Para list
		if(!pDspSession->AdSessionOk())
		{
			AD_ERROR("Dsp Session Connect Error\n");
			if(pState != NULL)
				pState->StatusAddSysError();
			goto RETURN;
		}
		break;
       }

	//update status
	if(pState != NULL)
		pState->StatusAdd();

	//check Para list, timeout no send
	if( i >= 0)
	{
		//up session  active counter,  wait response
		pDspSession->AdSessionActive();
		pDspSession->AdSessionSetTimeout(10);
		pPara->m_pSrcSession = pSession;
		pPara->m_pDstSession = pDspSession;

		pList->push(pPara);
		Json::Value cJson;
		std::string sJson;
		Json::FastWriter writer;
		pPara->m_ctRequest.toJson(cJson);
		sJson = writer.write(cJson);

		pPara->m_cHttp.GetRequest(HTTP_POST,"/mzbid",sJson.c_str(), (INT32)sJson.size(),cOutDsp);

		pDspSession->AdSessionWrite(cOutDsp);
		return AD_SUCCESS;
	}
RETURN:
	ret = SessionTimeout(*pPara, ctTask, pSession);
	FreePara(pPara);
	return AD_FAILURE;
}


int AdAdx::PacketLen(AdCache * pCache)
{
	char * pBody;
	char * pData;
	int iBody;
	
	pData =pCache->Get();

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

