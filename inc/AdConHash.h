/***************************************
*	Description: Ad consistant hash file.
*	Author: wilson
*	Date: 2015/09/24
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef	_AD_CONHASH_H
#define	_AD_CONHASH_H

#include <conhash.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "AdGlobal.h"
#include "AdUtil.h"

using namespace std;

#define HASH_REPLICA 200

class AdConHashs
{
	public:
		AdConHashs();
		virtual ~AdConHashs();

		void Switch();
		void Clear();
		bool  Add(string &sKey, void* pData);
		void* LookUp(char * pKey);
	private:
		struct conhash_s*            m_pHandleA;
		struct conhash_s*            m_pHandleB;
		vector<struct node_s *> m_vNodesA;
		vector<struct node_s *> m_vNodesB;
		bool m_bFirst;
};

class AdConHash
{
	public:
		AdConHash();
		virtual ~AdConHash();

		int  Add(void* pData);
		int  Del(void* pData);
		void Clear();
		bool Check(void* pData);
		void* LookUp(char * pKey);
	private:
		struct conhash_s*            m_pHandle;
		map<void *, struct node_s *> m_mNodes;
		pthread_rwlock_t m_pLock;
};

#endif
