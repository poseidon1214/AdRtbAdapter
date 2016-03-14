/***************************************
*	Description: Adapter transfer Method
*	Author: wilson
*	Date: 2016/01/06
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef   __AD_ADAPTER_METHOD_H__
#define __AD_ADAPTER_METHOD_H__
#include <vector>
#include <string>
#include <list>

#include "AdGlobal.h"
#include "AdRtbConfig.h"
#include "AdProtocolBase.h"
#include "AdSharedLib.h"

using namespace std;

class AdAdapterMethod
{
public:
	AdAdapterMethod();
	virtual ~AdAdapterMethod();

	int Init(AdRtbConfig & ctConf);
	int  Get(AdAdapterPara & ctPara);
private:
	//URI to Transfer method(shared so)
	map<string, int>  m_mUriAdx;
	//host to transfer method
	map<string, int>  m_mHostAdx;
	//shared lib
	AdSharedLib<UINT, AdProtocolBase * > m_ctProMethod;
};
#endif
