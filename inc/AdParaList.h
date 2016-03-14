/***************************************
*	Description: Parameter list
*	Author: wilson
*	Date: 2015/09/07
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#ifndef   __AD_PARALIST_H__
#define __AD_PARALIST_H__
#include <list>
#include <vector>
#include "AdTask.h"
#include "AdAdapter.h"
#include "AdBuff.h"


class AdParaList
{
public:
	AdParaList()
	{
	};
	~AdParaList()
	{
	};

	int clear(AdAdapter *pAdapter, AdTask & ctTask)
	{
		list<AdAdapterPara *>::iterator ltIter;
		AdAdapterPara * pPara;
		AdSession * pAdxSession;
		AdSession * pBidSession;
		
		m_ctLock.lock();
		for(ltIter =m_ltPara.begin(); ltIter != m_ltPara.end(); ltIter ++)
		{
			pPara = *ltIter;
			pBidSession = static_cast<AdSession * >(pPara->m_pDstSession);
			if(pPara->m_bTimeout == false)
			{		 
				pPara->m_bTimeout = true;		
				pAdxSession = static_cast<AdSession * >(pPara->m_pSrcSession);
				if(pAdapter != NULL)
					pAdapter->SessionTimeout(*pPara, ctTask, pAdxSession);
			}
			pAdapter->FreePara(pPara);
			pBidSession->AdSessionIdle();	
		}
		m_ltPara.clear();
		m_ctLock.unlock();
		return AD_SUCCESS;
	};

	
	int check(AdAdapter *pAdapter, AdTask & ctTask)
	{
		list<AdAdapterPara *>::iterator ltIter;
		AdAdapterPara * pPara;
		AdSession * pAdxSession;
		int iCount = 0;
		m_ctLock.lock();
		for(ltIter =m_ltPara.begin(); ltIter != m_ltPara.end(); ltIter ++)
		{
			 pPara = (*ltIter);
			 if(pPara->m_bTimeout)
			 {
			 	iCount ++;
				continue;
			 }
			 if(pPara->m_ctRequest.checkLife())
			{
				iCount ++;
				pPara->m_bTimeout = true;
				pAdxSession = static_cast<AdSession *>(pPara->m_pSrcSession);
				if(pAdapter != NULL)
					pAdapter->SessionTimeout(*pPara, ctTask, pAdxSession);
			}
			else
				break;
		}
		m_ctLock.unlock();
		return iCount;
	};

	int push(AdAdapterPara * pPara)
	{
		m_ctLock.lock();
		m_ltPara.push_back(pPara);
		m_ctLock.unlock();
		return AD_SUCCESS;
	};

	int del(AdAdapterPara * pPara)
	{
		list<AdAdapterPara *>::iterator ltIter;
		int iDel = AD_FAILURE;
		m_ctLock.lock();
		for(ltIter =m_ltPara.begin(); ltIter != m_ltPara.end(); ltIter ++)
		{
			if(pPara == *ltIter)
			{
				m_ltPara.erase(ltIter);
				iDel = AD_SUCCESS;
				break;
			}
		}
		m_ctLock.unlock();
		return iDel;
	};

	AdAdapterPara *  pop()
	{
		AdAdapterPara * pPara = NULL;
		list<AdAdapterPara *>::iterator ltIter;
		m_ctLock.lock();
		ltIter = m_ltPara.begin();
		if(ltIter != m_ltPara.end())
		{
			m_ltPara.pop_front();
			pPara = *ltIter;
		}
		m_ctLock.unlock();
		return pPara;
	};
	int size()
	{
		int cnt;
		m_ctLock.lock();
		cnt = m_ltPara.size();
		m_ctLock.unlock();
		return cnt;
	}

private:
	list<AdAdapterPara *> m_ltPara;
	LockMutex m_ctLock;
};

#endif

