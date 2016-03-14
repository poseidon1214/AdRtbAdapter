/***************************************
*	Description: ssh madule, transfer protocol  send for adx
*	Author: wilson
*	Date: 2016/03/03
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef  __AD_SSP_H__
#define __AD_SSP_H__
#include <map>
#include <list>
#include <iostream>
#include "AdRtbConfig.h"
#include "AdUtil.h"

#include "AdSession.h"
#include "AdBuff.h"
#include "AdOpenRtb.h"
#include "AdData.h"


using namespace std;


class AdSsp
{
public:
	AdSsp();
	virtual ~AdSsp();
	//init conf, class
	int Init(AdRtbConfig & ctConfig);
	//update config
	int Update(void);

	//packet  transfer to internel struct
	int ToInternal(AdRtbPara & ctPara, string & sIp);
	
	//struct to http packet
	int ToHttp(AdRtbPara & ctPara);
	
	//no content
	int EmptyToHttp(AdRtbPara & ctPara);
	
private:
	AdData m_cPidData;
};

#endif

