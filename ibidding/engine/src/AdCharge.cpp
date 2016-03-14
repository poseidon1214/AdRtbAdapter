/***************************************
*	Description: Ad Charge System, check bid Uinit money.
*	Author: wilson
*	Date: 2016/01/28
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#include "AdCharge.h"
#include "AdTcp.h"

void AdChargePreSend(AdSession * pSession);

AdCharge::AdCharge():m_ctClients(NULL)
{
	string sName = "Charge";
	SetName(sName);
}

AdCharge::~AdCharge()
{
	//clear old server and session
	list<string>::iterator iterList;
	for(iterList = m_ltServer.begin(); iterList != m_ltServer.end(); iterList ++)
	{
		m_ctClients.AdClientDelSession(*iterList);
	}
	m_ltServer.clear();
}

int AdCharge::ChargeInit(const AdRtbConfig & ctConfig, map<UINT, AdTask *> & TaskList)
{
	int ret;

	//get charge server info
	string sSec = "[ChargeServer]";
	ADCONFIG & server = ctConfig.Get(sSec);
	ADCONFIG_ITER iterConf;

	//init client session
	if(m_ltServer.empty())
	{
		//init clients
		ret = m_ctClients.AdClientInit(m_ltServer, 1, 0);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("AdClient init error\n");
			return ret;
		}

		//create net (server,  work) task
		ret = AdControlNetCreateTask(static_cast<AdNetBase *>(&m_ctClients),
			static_cast<AdNetTaskInterface *>(this), 1, 0);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("AdServer Create error\n");
			return ret;
		}	
	}
	else
	{
		//clear old server and session
		list<string>::iterator iterList;
		for(iterList = m_ltServer.begin(); iterList != m_ltServer.end(); iterList ++)
		{
			m_ctClients.AdClientDelSession(*iterList);
		}
		m_ltServer.clear();
	}

	//create session
	for(iterConf = server.begin(); iterConf != server.end(); iterConf ++)
	{
		m_ltServer.push_back(iterConf->second);
		ret = m_ctClients.AdClientCreateSession(iterConf->second);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("AdClient Create Session  error\n");
		} 
	}

	return AD_SUCCESS;
}

//update task
int AdCharge::Run(AdCache * pCache, AdSession * pSession, AdTask & ctTask)
{
	while(ctTask.m_bTaskRun)
	{
		Time::wait(10);
		ctTask.m_uiLoop ++;
	}
	ctTask.m_bTaskRun = false;
	return AD_SUCCESS;
}

void AdChargePreSend(AdSession * pSession)
{
}

