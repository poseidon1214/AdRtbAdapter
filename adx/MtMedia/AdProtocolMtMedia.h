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

class AdPrivateInfo
{
public:
	AdPrivateInfo()
	{
		clear();
	};
	~AdPrivateInfo(){};
	void clear()
	{
		// request
		m_sMd5.clear();
		m_sSHA.clear();
		m_sImpId.clear();
		m_sDealID.clear();
		m_sIMEI.clear();
	};
	
	// request
	string m_sMd5;
	string m_sSHA;
	string m_sImpId;
	string m_sDealID;
    string m_sIMEI;
};


class AdProtocolMtMedia:public AdProtocolBase
{
public:
	AdProtocolMtMedia(){};
	virtual ~AdProtocolMtMedia(){};

	virtual int HttpToInternal(AdAdapterPara * pPara);
	virtual int InternalToHttp(AdAdapterPara * pPara);
	virtual int EmptyToHttp(AdAdapterPara * pPara);
};

extern "C" {
extern AdProtocolBase * AdProtocolInit();
};

#endif

