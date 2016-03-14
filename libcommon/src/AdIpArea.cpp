/***************************************
*	Description: Ad Ip for Area, include transfer with ip and area
*	Author: wilson
*	Date: 20160309
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <string.h>

#include "AdIpArea.h"

AdIpArea::AdIpArea()
{
}

AdIpArea::~AdIpArea()
{
	Clear();
}

void AdIpArea::Clear()
{
	list<TreeNode *>::iterator iter;
	for(iter = m_ltTreeList.begin(); iter != m_ltTreeList.end(); iter ++)
	{
		delete *iter;
	}
	m_ltTreeList.clear();
	m_ctTree.clear();
}

int AdIpArea::AddTree(string & sCoun, string & sPro, string &sCity, string &sAddr, UINT uiMastlen)
{
	UINT uiIP = 0;
	UINT i = 0;
	struct in_addr Addr;
	TreeNode * pNode ;
	UINT uiMask;

	inet_aton(sAddr.c_str(), &Addr);
	uiIP = ntohl(Addr.s_addr);

	pNode = &m_ctTree;

	if (sAddr !=  "0.0.0.0")
	{
		for(i = 0; i < uiMastlen; i ++)
		{	
			uiMask = 0x80000000>> i;
			if (uiMask & uiIP) 
			{
				if (NULL == pNode->m_pRight) 
				{
					pNode->m_pRight= new TreeNode;
					if(pNode->m_pRight == NULL)
					{
						AD_ERROR("Mem not enough,  new fail\n");
						return AD_FAILURE;
					}
					m_ltTreeList.push_back(pNode->m_pRight);
				}
				pNode = pNode->m_pRight;
			}
			else {
				if (NULL == pNode->m_pLeft) 
				{
					pNode->m_pLeft= new TreeNode;
					if(pNode->m_pLeft == NULL)
					{
						AD_ERROR("Mem not enough,  new fail\n");
						return AD_FAILURE;
					}
					m_ltTreeList.push_back(pNode->m_pLeft);
				}
				pNode = pNode->m_pLeft;
			}
		}
		pNode->m_sCountry = sCoun;
		pNode->m_sProvince = sPro;
		pNode->m_sCity = sCity;
	}
	else {
		if (0 == uiMastlen)
		{
			pNode->m_sCountry = sCoun;
			pNode->m_sProvince = sPro;
			pNode->m_sCity = sCity;
		}
	}

	pNode->m_sCountry = sCoun;
	pNode->m_sProvince = sPro;
	pNode->m_sCity = sCity;

	return AD_SUCCESS;
}

int AdIpArea::Init(string & sFile)
{
	UINT uiMask;
	xmlDoc* pDoc = NULL;
	xmlNode* pRoot = NULL;
	xmlNode* pNode = NULL;
	xmlAttr* pAttr = NULL;
	int ret = AD_SUCCESS;

	string sCountry = "0086";
	string sProvince;
	string sCity;
	string sAddr;

	/* file */
	pDoc = xmlParseFile(sFile.c_str());
	if (NULL == pDoc) 
	{
		AD_ERROR("xml Parse file %s fails\n", sFile.c_str());
		return AD_FAILURE;
	}

	/* root */
	pRoot = xmlDocGetRootElement(pDoc);
	if (NULL == pRoot)
	{
		AD_ERROR("xml Get root Element\n");
		ret = AD_FAILURE;
		goto RERURN;
	}

	/* node */	
	for (pNode= pRoot->children; NULL != pNode; pNode= pNode->next) 
	{
		if (XML_ELEMENT_NODE != pNode->type) 
		{
			continue;
		}
		for (pAttr= pNode->properties; NULL != pAttr; pAttr= pAttr->next) 
		{
			if (NULL != pAttr->children)
			{
				if (AD_SUCCESS == strcmp((char*)pAttr->name, "prov")) 
				{
					sProvince= (char *)(pAttr->children->content);
				}
				else if (AD_SUCCESS == strcmp((char*)pAttr->name, "city"))
				{
					sCity= (char *)(pAttr->children->content);
				}
				else if (AD_SUCCESS == strcmp((char*)pAttr->name, "addr"))
				{
					sAddr= (char *)(pAttr->children->content);
				}
				else if (AD_SUCCESS == strcmp((char*)pAttr->name, "mask"))
				{
					uiMask= atoi((char *)(pAttr->children->content));
				}

			}
		}
		ret = AddTree(sCountry,sProvince,sCity,sAddr, uiMask);
		if(ret != AD_SUCCESS)
		{
			break;
		}
	}

RERURN:
	xmlCleanupParser();
	xmlFreeDoc(pDoc);
	return ret;
}

int AdIpArea::Get(string & sAddr, string & sArea)
{
	UINT uiIP = 0;
	UINT i;
	UINT uiMask = 0;
	struct in_addr stAddr;
	TreeNode * pNode = &m_ctTree;

	inet_aton(sAddr.c_str(), &stAddr);
	uiIP = ntohl(stAddr.s_addr);

	sArea.reserve(AD_BYTE128);
	if(uiIP == 0)
	{
		sArea = pNode->m_sCountry + "-" + pNode->m_sProvince + "-" + pNode->m_sCity;
		return AD_SUCCESS;
	}

	for(i = 0; i < 32; i ++)
	{
		uiMask = 0x80000000 >> i;
		if (uiMask & uiIP)
		{
			if (0 != pNode->m_pRight) 
			{
				pNode =  pNode->m_pRight;
			}
			else
			{
				break;
			}
			if(!pNode->m_sCity.empty())
			{
				sArea = pNode->m_sCountry + "-" + pNode->m_sProvince + "-" + pNode->m_sCity;
			}
		}
		else 
		{
			if (0 != pNode->m_pLeft)
			{
				pNode =pNode->m_pLeft;
			}
			else 
			{
				break;
			}
			if(!pNode->m_sCity.empty())
			{
				sArea = pNode->m_sCountry + "-" + pNode->m_sProvince + "-" + pNode->m_sCity;
			}
		}
	}

	return AD_SUCCESS;
}
