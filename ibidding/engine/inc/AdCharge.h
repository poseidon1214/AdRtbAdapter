/***************************************
*	Description: Ad Charge System, check bid Uinit money.
*	Author: wilson
*	Date: 2016/01/28
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_CHARGE_H_
#define _AD_CHARGE_H_
#include <iostream>
#include <map>
#include <list>
#include "AdGlobal.h"
#include "AdLock.h"
#include "AdUtil.h"
#include "AdRtbConfig.h"
#include "AdClient.h"
#include "AdControlTask.h"
#include "AdTime.h"

using namespace std;

class AdConsumer
{
	public:
		AdConsumer()
		{
		};
		virtual ~AdConsumer()
		{
			m_mTimeConsumer.clear();
			m_mConsumerPrice.clear();
			m_mConsumerTime.clear();
		};

		void Add(string & sName, UINT uiPrice)
		{
			Time cTime;
			m_mTimeConsumer[cTime.getSecond()] = sName;
			m_mConsumerPrice[sName] = uiPrice;
			m_mConsumerTime[sName] = cTime.getSecond();
		};

		int DelBefore(Time_t stTime )
		{
			map<string, UINT>::iterator iter;
			UINT uiPrice = 0;
			if(stTime == 0)
				return AD_FAILURE;

			multimap<Time_t, string>::iterator mIter;
			mIter = m_mTimeConsumer.begin();
			while( mIter != m_mTimeConsumer.end())
			{
				if(mIter->first < stTime)
				{
					iter = m_mConsumerPrice.find(mIter->second);
					if(iter != m_mConsumerPrice.end())
					{
						uiPrice += iter->second;
						m_mConsumerPrice.erase(mIter->second);
						m_mConsumerTime.erase(mIter->second);
					}
					m_mTimeConsumer.erase(mIter ++);
					continue;
				}
				break;
			}

			return uiPrice;
		};
		

		int DelBefore(string & sName, Time_t stTime = 0)
		{
			map<string, UINT>::iterator iter;
			UINT uiPrice = 0;
			if(!sName.empty())
			{
				iter = m_mConsumerTime.find(sName);
				if(iter != m_mConsumerTime.end())
				{
					stTime = mIter->second;
				}
			}
			if(stTime == 0)
				return AD_FAILURE;

			multimap<Time_t, string>::iterator mIter;
			mIter = m_mTimeConsumer.begin();
			while( mIter != m_mTimeConsumer.end())
			{
				if(mIter->first < stTime)
				{
					iter = m_mConsumerPrice.find(mIter->second);
					if(iter != m_mConsumerPrice.end())
					{
						uiPrice += iter->second;
						m_mConsumerPrice.erase(mIter->second);
						m_mConsumerTime.erase(mIter->second);
					}
					m_mTimeConsumer.erase(mIter ++);
					continue;
				}
				break;
			}

			return uiPrice;
		};
	private:
		multimap<Time_t, string>  m_mTimeConsumer;
		map<string, Time_t> m_mConsumerTime;
		map<string, UINT> m_mConsumerPrice;
		
};

//adx money counter
class  AdMoney
{
	public:
		AdMoney(UINT uiMoney = 0)
		{
			m_uiAllMoney = uiMoney;
			m_uiMoneySpend = 0;
		};
		virtual ~AdMoney()
		{
		};

		void Set(UINT uiMoney)
		{
			m_uiAllMoney = uiMoney;
		};

		bool Check()
		{
			//check timeout, 1second loop
			if(m_ctTime.toNow() > 1000000)
			{
				map<UINT, AdConsumer>::iterator iter;
				UINT uiPrice = 0;
				//update time
				m_ctTime.update();
				m_ctLock.lock();
				for(iter = m_mConsumer.begin(); iter != m_mConsumer.end(); iter ++)
				{
					//check timeout request
					uiPrice += iter->second.DelBefore(m_ctTime.getSecond() - 10 * 60);
				}
				//restore spend
				m_uiMoneySpend -= uiPrice;
				m_ctLock.unlock();
			}
			return  (m_uiMoneySpend < m_uiAllMoney);
		};

		void AddConsumer(UINT adx, string & sName, UINT uiPrice)
		{
			m_ctLock.lock();
			m_uiMoneySpend += uiPrice;
			m_mConsumer[adx].Add(sName, uiPrice);
			m_ctLock.unlock();
		};

		void DelConsumer(UINT adx, string & sName)
		{
			m_ctLock.lock();
			UINT uiPrice = m_mConsumer[adx].DelBefore(sName);
			m_uiMoneySpend -= uiPrice;
			m_ctLock.unlock();
		};
	private:
		UINT  m_uiAllMoney;
		UINT  m_uiMoneySpend;
		map<UINT, AdConsumer> m_mConsumer;
		LockMutex m_ctLock;
		Time m_ctTime;
};

class AdCharge:public AdNetTaskInterface
{
	public:
		AdCharge();
		virtual ~AdCharge();
		int ChargeInit(const AdRtbConfig & ctConfig, map<UINT, AdTask * > & TaskList);

		inline bool Check(UINT uiEid)
		{
			return m_mEidMoney[uiEid].Check();
		};

		inline void Sub(UINT adx, UINT uiEid, string & sName, UINT uiPrice)
		{
			m_mEidMoney[uiEid].AddConsumer(adx, sName,uiPrice);
		};

		virtual  int Run(AdCache * pCache, AdSession * pSession, AdTask & ctTask);
		
	private:
		map<UINT, AdMoney> m_mEidMoney;
		//clients: connect charge server.
		AdClient m_ctClients;
		//server info
		list<string> m_ltServer;
};
#endif
