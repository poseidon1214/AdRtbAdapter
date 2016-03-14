/***************************************
*	Description: Ad Ip for Area, include transfer with ip and area
*	Author: wilson
*	Date: 20160309
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_IP_AREA_H
#define _AD_IP_AREA_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"

using namespace std;


class TreeNode
{
public:
	TreeNode()
	{
		clear();
	};
	~TreeNode()
	{
		clear();
	};
	void clear()
	{
		m_pLeft = NULL;
		m_pRight = NULL;
		m_sCountry.clear();
		m_sProvince.clear();
		m_sCity.clear();
	};
	TreeNode& operator=(const TreeNode &a) 
	{
		this->m_sCountry = a.m_sCountry;
		this->m_sProvince = a.m_sProvince;
		this->m_sCity = a.m_sCity;
		return *this;
	};
	
	TreeNode *  m_pLeft;
	TreeNode *  m_pRight;
	string m_sCountry;
	string m_sProvince;
	string m_sCity;
};

#define AD_AREA_ALL    "0086-ffff-ffff"


class AdIpArea
{
public:
	AdIpArea();
	virtual ~AdIpArea();
	
	int Init(string & sFile);
	void Clear();
	int Get(string & sAddr, string & sArea);
	
private:
	string m_sName;
	TreeNode m_ctTree;
	list<TreeNode *> m_ltTreeList;
	int AddTree(string & sCoun, string & sPro, string &sCity, string &sAddr, UINT uiMastlen);
};

#endif

