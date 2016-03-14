/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#ifndef  __AD_PROTOCOL_H__
#define __AD_PROTOCOL_H__

#include <string>
#include <iostream>
#include "AdProtocolBase.h"

using namespace std;

class AdProtocolAiQiYi:public AdProtocolBase
{
public:
	AdProtocolAiQiYi();
	virtual ~AdProtocolAiQiYi(){};

	virtual int HttpToInternal(AdAdapterPara * pPara);
	virtual int InternalToHttp(AdAdapterPara * pPara);
	virtual int EmptyToHttp(AdAdapterPara * pPara);
	
private:
	int AdSize(string& ad_zone_id, int flow, string& size);
	int GenerateVastVedio(AdAdapterPara * pPara, char * sBuf, string &cmurl, string &price_macro, string &click_macro,int iBuf);	
	map<string, string> m_PC_Size;
	map<string, string> m_MB_Size;
};
extern "C" {
extern int  SharedLibInit(AdProtocolBase *  & ctpPro);
};


#endif

