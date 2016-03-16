/***************************************
*	Description: Adapter transfer Method
*	Author: wilson
*	Date: 2016/01/06
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdAdapterMethod.h"
#include "AdUtil.h"

AdAdapterMethod::AdAdapterMethod()
{
}


AdAdapterMethod::~AdAdapterMethod()
{
}

int AdAdapterMethod::Get(AdAdapterPara & ctPara)
{
	char * pBody;
	char * pData = ctPara.m_pCacheIn->Get();
	int iBody;
	int ret;
	AdProtocolBase * pTrans = NULL;

	string str;
	ret = AdHttp::GetPostUri(*ctPara.m_pCacheIn,   str);
	if (AD_SUCCESS != ret) 
	{
		ret = AdHttp::GetGetUri(*ctPara.m_pCacheIn,   str);
		if(AD_SUCCESS != ret )
		{
			return AD_FAILURE;
		}
	}
	
	//find adx
	map<string, int>::iterator iter;
	iter = m_mUriAdx.find(str);
	if(iter == m_mUriAdx.end())
	{
		ret = AdHttp::GetHost(*ctPara.m_pCacheIn,   str);
		if (AD_SUCCESS != ret) 
		{
			return AD_FAILURE;
		}
	
		iter = m_mHostAdx.find(str);
		if(iter == m_mHostAdx.end())
		{
			return AD_FAILURE;
		}
	}

	ctPara.m_ctRequest.m_uiAdx = iter->second;

	ret = m_ctProMethod.Get(iter->second, pTrans);
	if (AD_SUCCESS != ret) 
	{
		return AD_FAILURE;
	}

	ctPara.m_pAdxMethod= static_cast<void *>(pTrans);

	iBody = AdHttp::GetLen(*ctPara.m_pCacheIn);
	if (AD_SUCCESS > iBody) 
	{
		return AD_FAILURE;
	}

	pBody = AdHttp::GetBody(*ctPara.m_pCacheIn);
	if (NULL == pBody) 
	{
		return AD_FAILURE;
	}
	
	if((pBody - pData + iBody)  > ctPara.m_pCacheIn->Size())
	{
		AD_ERROR("Data len Error, All: %d,  Head: %ld body: %d\n", 
			ctPara.m_pCacheIn->Size(), pBody- pData, iBody);
		return AD_FAILURE;
	}
	ctPara.m_pBody = pBody;
	ctPara.m_iBody = iBody;
	return  AD_SUCCESS;
}

int AdAdapterMethod::Init(AdRtbConfig & ctConf)
{
	//config section
	string sSec = "[METHODSO]";
	ADCONFIG & so = ctConf.Get(sSec);
	ADCONFIG_ITER iter;
	
	vector<string> vStr;

	int ret;

	//close shared lib
	m_ctProMethod.Close(true);
	m_mUriAdx.clear();
	m_mHostAdx.clear();

	for(iter = so.begin(); iter != so.end(); iter ++)
	{
		int id = 0;
		if((iter->first)[0] == '/')
		{
			AdString::StrSplit(iter->first,  vStr, ":");
			if(vStr.size() == 2)
			{
				id = atoi(vStr[1].c_str());
				m_mUriAdx[vStr[0]] = id;
			}
		}
	 	else
	 	{
	 		AdString::StrSplit(iter->first,  vStr, ":");
			if(vStr.size() == 2)
			{
				id = atoi(vStr[1].c_str());
				m_mHostAdx[vStr[0]] = id;
			}
	 	}
		vStr.clear();
		if(id == 0)
			return AD_FAILURE;
		ret = m_ctProMethod.Open(id,  iter->second);
		if(ret != AD_SUCCESS)
			return ret;
	}

	return AD_SUCCESS;
}

