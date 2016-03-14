/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/10/23
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


class AdProtocolJuXiao:public AdProtocolBase
{
public:
	AdProtocolJuXiao();
	virtual ~AdProtocolJuXiao(){};

	virtual int HttpToInternal(AdAdapterPara * pPara);
	virtual int InternalToHttp(AdAdapterPara * pPara);
	virtual int EmptyToHttp(AdAdapterPara * pPara);
	
private:
	string m_sExtStr;
	string m_sImgUrl;
};
extern "C" {
extern int  SharedLibInit(AdProtocolBase *  & ctpPro);
};


#endif

