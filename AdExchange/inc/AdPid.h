/***************************************
*	Description:  Pid Data, store Pid info
*	Author: wilson
*	Date: 2016/03/02
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef __AD_PID_H__
#define __AD_PID_H__
#include <string>
#include <map>
#include <vector>
#include <list>
#include <iostream>

class AdPid
{
public:
	AdPid()
	{
		clear();
	};

	virtual ~AdPid()
	{
		clear();
	};

	void clear()
	{
		m_sPid.clear();
		m_sSize.clear();
		m_iHeight = AD_FAILURE;
		m_iWidth = AD_FAILURE;
		m_fBidFloor = 0.0;
		m_iPos = AD_FAILURE;
		m_sClass.clear();
		m_sAdType.clear();
		m_sPage.clear();
		m_sKeywords.clear();
		m_vCat.clear();
		m_vBanCat.clear();
		m_iMid = AD_FAILURE;
		
	};
	INT32 m_iMid;
	std::string m_sPid;
	std::string m_sSize;
	INT32 m_iWidth;
	INT32 m_iHeight;
	FLOAT m_fBidFloor;
	INT32 m_iPos;
	INT32  m_iPidType;                        //0 banner,  1 video, app
	std::string m_sClass;                      //pc app...
	std::string m_sAdType;                  //¹Ì¶¨£¬ÐüÍ£
	
	std::string m_sPage;
	std::string m_sKeywords;
	std::vector<std::string> m_vCat;
	std::vector<std::string> m_vBanCat;
};
#endif
