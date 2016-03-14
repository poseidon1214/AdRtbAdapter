/***************************************
*	Description: internal net protocol,  with tlv
*	Author: wilson
*	Date: 2015/07/22
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_RTB_NET_PROTOCOL_H
#define	_AD_RTB_NET_PROTOCOL_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "AdGlobal.h"
#include "AdUtil.h"

#include "AdRtbBase.h"

using namespace std;

typedef struct AdProcotolHead
{
	UCHAR ucFlag;
	UCHAR ucVersion;
	UCHAR ucProtocol;
	UCHAR ucWide;
	UCHAR ucResponse;
	UCHAR ucExpand;


	UCHAR ucType;
	UCHAR ucVerb;
	UCHAR ucAttr;
	UCHAR ucResult;
	USHORT usTotal;
	int iLength;
}ADPROHEAD_S;


#define AD_PROTOCOL_HEAD_LEN       12
#define AD_PROTOCOL_HEAD_EXPAND_LEN       4

#define AD_MODE_BID_PRICE         		           1
#define AD_MODE_FIXED_PRICE            	   2


struct parser
{
	UINT  type;
	void *  pointer;
};

class AdRtbProtocol
{
public:
	AdRtbProtocol();
	virtual ~AdRtbProtocol(){};

	//for adapter
	 int InternalToTlv(AdRtbRequest & ctReq, char * pdata,int iLen, bool bPrint=false);
	  int TlvToInternal(AdRtbResponse & ctRes, char * pData,int iLen, bool bPrint=false);
	  //for ibid
	   int InternalToTlv(AdRtbResponse & ctRes, char * pdata,int iLen, bool bPrint=false);
	  int TlvToInternal(AdRtbRequest & ctReq, char * pData,int iLen, bool bPrint=false);
	  int TlvPacketLen(char * pData, int iLen);

private:
	void AdRtbProtocolInitParser(AdRtbResponse& ctRes);
	void AdRtbProtocolInitParser(AdRtbRequest & ctReq);
	ADPROHEAD_S  m_stHead;
	string m_sExHead;

	map<UCHAR, struct parser>  m_mParse;
};


#endif 

