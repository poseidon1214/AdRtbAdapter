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
		m_iVersion = AD_FAILURE;
		m_iAdzApi = 0;
		m_bMobile = false;
		m_sSlotID.clear();
		m_sRawIdfa.clear();
		m_sEncrypIdfa.clear();
		m_sHashIdfa.clear();
		m_sImme.clear();
		m_sEncrypImme.clear();			
	};

	int m_iVersion;
	int   m_iAdzApi;
	bool  m_bMobile;
	/* mobile device id*/
	string m_sSlotID;
	string m_sRawIdfa;
	string m_sEncrypIdfa;
	string m_sHashIdfa;
	string m_sImme;
	string m_sEncrypImme;
	
};


class AdProtocolGoogle:public AdProtocolBase
{
public:
	AdProtocolGoogle(){};
	virtual ~AdProtocolGoogle(){};

	virtual int HttpToInternal(AdAdapterPara * pPara);
	virtual int InternalToHttp(AdAdapterPara * pPara);
	virtual int EmptyToHttp(AdAdapterPara * pPara);
	
private:
};
extern "C" {
extern int  SharedLibInit(AdProtocolBase *  & ctpPro);
};


#endif

