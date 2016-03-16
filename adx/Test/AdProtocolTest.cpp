/***************************************
*	Description: adx protocol transfer module. adx->internal,  internal->adx.
*	Author: wilson
*	Date: 2015/09/29
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdProtocolTest.h"
#include <string.h>
#include <stdlib.h>


int AdProtocolTest::HttpToInternal(AdAdapterPara * pPara)
{
	return AD_FAILURE;
}

int AdProtocolTest::InternalToHttp(AdAdapterPara * pPara)
{
	return AD_FAILURE;
}

int AdProtocolTest::EmptyToHttp(AdAdapterPara * pPara)
{

	std::string STanx;
	char sHead[AD_KBYTES1] = { 0 };


	/* head */
	std::string temp = "HTTP/1.0 200 OK\r\nContent-Length: 2\r\nContent-Type: text/html; charset=UTF-8\r\nServer: TornadoServer/2.2\r\n\r\nOK";
	strncpy(sHead, temp.c_str(), temp.size());
	strncpy(sHead, temp.c_str(), temp.size());
	int ret;
	ret = pPara->m_ctCacheOut.Write(sHead ,strlen(sHead));
	if(ret != AD_SUCCESS)
		return AD_FAILURE;

	ret = pPara->m_ctCacheOut.Append(STanx.c_str(), 0);
	if(ret != AD_SUCCESS)
		return AD_FAILURE;
	
	return AD_SUCCESS;
}

AdProtocolBase * AdProtocolInit()
{
	AdProtocolTest *  pPro = new AdProtocolTest;
	if(pPro == NULL)
	{
		AD_ERROR("Mem not enough\n");
	}
	return static_cast<AdProtocolBase *>(pPro);
}

