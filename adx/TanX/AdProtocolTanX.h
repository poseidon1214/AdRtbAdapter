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

class AdProtocolTanX:public AdProtocolBase
{
public:
	AdProtocolTanX(){};
	virtual ~AdProtocolTanX(){};

	virtual int HttpToInternal(AdAdapterPara * pPara);
	virtual int InternalToHttp(AdAdapterPara * pPara);
	virtual int EmptyToHttp(AdAdapterPara * pPara);
	
private:
};
extern "C" {
extern int  SharedLibInit(AdProtocolBase *  & ctpPro);
};


#endif

