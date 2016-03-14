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
#include "AdTime.h"

using namespace std;


class AdProtocolWeiBo:public AdProtocolBase
{
public:
	AdProtocolWeiBo(){};
	virtual ~AdProtocolWeiBo(){};

	virtual int HttpToInternal(AdAdapterPara * pPara);
	virtual int InternalToHttp(AdAdapterPara * pPara);
	virtual int EmptyToHttp(AdAdapterPara * pPara);
private:
	int UserAgeGroup(int born_year, string &sAge);
};

extern "C" {
extern int  SharedLibInit(AdProtocolBase *  & ctpPro);
};


#endif

