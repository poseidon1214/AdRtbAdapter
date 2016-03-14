/***************************************
*	Description: Ad consistant hash file.
*	Author: wilson
*	Date: 2015/09/24
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#include "AdConHash.h"

AdConHash::AdConHash()
{
	m_pHandle = NULL;
	int ret = pthread_rwlock_init(&m_pLock, NULL);
	if(ret != AD_SUCCESS)
	{
		AD_ERROR("rwLock init  error\n");
	}
}

AdConHash::~AdConHash()
{
	Clear();
	pthread_rwlock_destroy(&m_pLock);
}


void* AdConHash::LookUp(char * pKey)
{
	struct node_s* pNode;
	if (m_pHandle == NULL) 
	{
		return NULL;
	}
	pthread_rwlock_rdlock(&m_pLock);
	pNode = const_cast<struct node_s *>(conhash_lookup(m_pHandle, pKey));
	pthread_rwlock_unlock(&m_pLock);

	if(pNode == NULL)
		return NULL;
	return pNode->value;
}

bool  AdConHash::Check(void* pData)
{
	pthread_rwlock_rdlock(&m_pLock);
	if(m_mNodes.find(pData) != m_mNodes.end())
	{
		pthread_rwlock_unlock(&m_pLock);
		return true;
	}
	pthread_rwlock_unlock(&m_pLock);
	return false;
}



int AdConHash::Add(void* pData)
{
	struct node_s* pNode;
	char buff[AD_BYTE32];

	snprintf(buff, AD_BYTE32, "bidding_%02d", (int)m_mNodes.size());

	pthread_rwlock_wrlock(&m_pLock);
	if (m_pHandle == NULL) 
	{
		m_pHandle = conhash_init(NULL);
		if (m_pHandle == NULL) 
		{
			pthread_rwlock_unlock(&m_pLock);
			return AD_SUCCESS;
		}
	}
	pNode = (struct node_s *) malloc (sizeof(struct node_s));
	if (pNode != NULL)
	{
		conhash_set_node(pNode, buff, HASH_REPLICA, pData);
		conhash_add_node(m_pHandle, pNode);
		m_mNodes[pData] = pNode;
	} 
	pthread_rwlock_unlock(&m_pLock);
	return AD_SUCCESS;
}

int AdConHash::Del(void* pData)
{
	map<void *, struct node_s *>::iterator iter;

	pthread_rwlock_wrlock(&m_pLock);
	if (m_pHandle == NULL) 
	{
		pthread_rwlock_unlock(&m_pLock);
		return AD_FAILURE;
	}
	iter = m_mNodes.find(pData);
	if(iter != m_mNodes.end())
	{
		conhash_del_node(m_pHandle, iter->second);
		m_mNodes.erase(iter);
	}
	pthread_rwlock_unlock(&m_pLock);
	return AD_SUCCESS;
}

void AdConHash::Clear()
{
	map<void *, struct node_s *>::iterator iter;

	pthread_rwlock_wrlock(&m_pLock);
	if (m_pHandle != NULL) 
	{
		conhash_fini(m_pHandle);
		m_pHandle = NULL;
	}
	
	for (iter = m_mNodes.begin(); iter != m_mNodes.end(); iter ++)
	{
		free(iter->second);
	}
	m_mNodes.clear();
	pthread_rwlock_unlock(&m_pLock);
}

AdConHashs::AdConHashs()
{
	m_pHandleA = NULL;
	m_pHandleB = NULL;
	m_bFirst = false;
}

AdConHashs::~AdConHashs()
{
	Clear();
	Switch();
	Clear();
}


void* AdConHashs::LookUp(char * pKey)
{
	struct node_s* pNode;
	if(m_bFirst)
	{
		if (m_pHandleA == NULL) 
		{
			return NULL;
		}
		pNode = const_cast<struct node_s *>(conhash_lookup(m_pHandleA, pKey));
	}
	else
	{
		if (m_pHandleB == NULL) 
		{
			return NULL;
		}
		pNode = const_cast<struct node_s *>(conhash_lookup(m_pHandleB, pKey));
	}

	if(pNode == NULL)
		return NULL;
	return pNode->value;
}

void AdConHashs::Switch()
{
	if(m_bFirst == false)
	{
		m_bFirst = true;
	}
	else
	{
		m_bFirst = false;
	}
}


bool AdConHashs::Add(string & sKey, void* pData)
{
	struct node_s* pNode;
	if(m_bFirst == false)
	{
		if (m_pHandleA == NULL) 
		{
			m_pHandleA = conhash_init(NULL);
			if (m_pHandleA == NULL) 
				return false;
		}
		pNode = (struct node_s *) malloc (sizeof(struct node_s));
		if (pNode != NULL)
		{
			conhash_set_node(pNode, sKey.c_str(), HASH_REPLICA, pData);
			conhash_add_node(m_pHandleA, pNode);
			m_vNodesA.push_back(pNode);
		} 
		else 
		{
			return false;
		}
	}
	else
	{
		if (m_pHandleB == NULL) 
		{
			m_pHandleB = conhash_init(NULL);
			if (m_pHandleB == NULL) 
				return false;
		}
		pNode = (struct node_s *) malloc (sizeof(struct node_s));
		if (pNode != NULL)
		{
			conhash_set_node(pNode, sKey.c_str(), HASH_REPLICA, pData);
			conhash_add_node(m_pHandleB, pNode);
			m_vNodesB.push_back(pNode);
		} 
		else 
		{
			return false;
		}
	}
	
	return true;
}


void AdConHashs::Clear()
{
	vector<struct node_s *>::iterator iter;
	
	if(m_bFirst == false)
	{
		if (m_pHandleA != NULL) 
		{
			conhash_fini(m_pHandleA);
			m_pHandleA = NULL;
		}
	
		for (iter = m_vNodesA.begin(); iter != m_vNodesA.end(); iter ++)
		{
			free(*iter);
		}
		m_vNodesA.clear();
	}
	else
	{
		if (m_pHandleB != NULL) 
		{
			conhash_fini(m_pHandleB);
			m_pHandleB = NULL;
		}

		for (iter = m_vNodesB.begin(); iter != m_vNodesB.end(); iter ++)
		{
			free(*iter);
		}
		
		m_vNodesB.clear();
	}
}


