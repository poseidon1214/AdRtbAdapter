/***************************************
*	Description: process msg module, transfer msg.
*	Author: wilson
*	Date: 2015/09/25
*
*	CopyRight: adirect Technology Co, Ltd
*
****************************************/
#ifndef  __AD_PROCESS_MSG_H__
#define __AD_PROCESS_MSG_H__
#include <string>
#include <iostream>
#include <vector>

#include "AdGlobal.h"
#include "AdIpc.h"

using namespace std;

class AdProcessMsg
{
public:
	AdProcessMsg();
	virtual ~AdProcessMsg();

	int AdProcessMsgInit(string & sPath, char ckey, int iMsg);
	int AdProcessMsgPut(void* pMsg, UINT uiSize, bool bBlock = false,int id = AD_FAILURE);
	int AdProcessMsgGet(void* pMsg, UINT uiSize, bool bBlock = false,int id = AD_FAILURE);
private:
	
	int AdProcessMsgSystemInit(ULONG ulMax, ULONG ulMnb, ULONG ulMni);
	vector<AdIpc *> m_vIpclist;
	UINT  m_uiWrite;
	UINT  m_uiRead;
};

#endif

