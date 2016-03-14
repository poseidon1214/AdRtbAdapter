/***************************************
*	Description: process msg module, transfer msg.
*	Author: wilson
*	Date: 2015/09/25
*
*	CopyRight: adirect Technology Co, Ltd
*
****************************************/
#include "AdProcessMsg.h"
#include <stdlib.h>

AdProcessMsg::AdProcessMsg()
{
	m_uiWrite = 0;
	m_uiRead = 0;
}

AdProcessMsg::~AdProcessMsg()
{
	vector<AdIpc *>::iterator iter;
	for(iter = m_vIpclist.begin(); iter != m_vIpclist.end(); iter ++)
	{
		delete *iter;
	}
	m_vIpclist.clear();
}

int AdProcessMsg::AdProcessMsgInit(string & sPath, char  cKey, int iMsg)
{
	AdIpc * pIpc;
	int i;
	int ret;
	if(!m_vIpclist.empty())
	{
		return AD_SUCCESS;
	}
	AdProcessMsgSystemInit(0xffff, 0x100000, 0x1400);
	
	for(i = 0; i < iMsg; i ++)
	{
		pIpc = new AdIpc;
		if(pIpc == NULL)
		{
			AD_ERROR("Mem not enough\n");
			return AD_FAILURE;
		}
		ret = pIpc->Create(sPath.c_str(), cKey + i);
		if(ret != AD_SUCCESS)
		{
			AD_ERROR("Ipc Create Fail\n");
			return ret;
		}
		m_vIpclist.push_back(pIpc);
	}
	return AD_SUCCESS;
}
int AdProcessMsg::AdProcessMsgPut(void* pMsg, UINT uiSize, bool bBlock, int id)
{
	if(id == AD_FAILURE)
	{
		id = __sync_add_and_fetch(&m_uiWrite, 1) % m_vIpclist.size();
	}
	
	return m_vIpclist[id]->Put(pMsg, uiSize, bBlock);
}
int AdProcessMsg::AdProcessMsgGet(void* pMsg, UINT uiSize, bool bBlock, int id)
{
	if(id == AD_FAILURE)
	{
		id = __sync_add_and_fetch(&m_uiRead, 1) % m_vIpclist.size();
	}
	
	return m_vIpclist[id]->Get(pMsg, uiSize, bBlock);
}


int AdProcessMsg:: AdProcessMsgSystemInit(ULONG ulMax, ULONG ulMnb, ULONG ulMni)
{
	char szCmd[AD_BYTE128];

	sprintf(szCmd, "sysctl -w kernel.msgmax=%ld", ulMax);
	system(szCmd);

	sprintf(szCmd, "sysctl -w kernel.msgmnb=%ld", ulMnb);
	system(szCmd);

	sprintf(szCmd, "sysctl -w kernel.msgmni=%ld", ulMni);
	system(szCmd);
	return AD_SUCCESS;
}


