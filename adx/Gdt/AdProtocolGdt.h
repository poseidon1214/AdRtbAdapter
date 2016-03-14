/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: zp
*	Date: 2015/12/11
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

#define AD_GG_CLICK_MACRO	"%%CLICK_URL_UNESC%%"
#define AD_GG_PRICE_MACRO  "%%WINNING_PRICE_ESC%%"

#define AD_COOKIEMAPPING_GOOGLE		""

class AdPrivateInfoGdt
{
public:
	AdPrivateInfoGdt()
	{
		clear();
	};
	~AdPrivateInfoGdt(){};
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


class AdProtocolGdt:public AdProtocolBase
{
public:
	AdProtocolGdt();
	virtual ~AdProtocolGdt(){};

	virtual int HttpToInternal(AdAdapterPara * pPara);
	virtual int InternalToHttp(AdAdapterPara * pPara);
	virtual int EmptyToHttp(AdAdapterPara * pPara);
	
private:
	void StrSplit(const string& str, vector<string>& ret_, string  sep);
	map<int, string> mtSize;
    map<int, int> mtPltf;
};
extern "C" {
extern int  SharedLibInit(AdProtocolBase *  & ctpPro);
};


#endif


