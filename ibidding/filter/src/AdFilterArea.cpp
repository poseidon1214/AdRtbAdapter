/***************************************
*	Description: Ad filter for Area, include transfer with ip and area
*	Author: wilson
*	Date: 2015/07/27
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "AdFilterArea.h"
#include "AdPlanningData.h"
#include "AdRtbBase.h"
#include "AdIpArea.h"

AdFilterArea::AdFilterArea()
{
	m_sName = "Area";
}

AdFilterArea::~AdFilterArea()
{
}
	
int AdFilterArea::match(void * data, AD_RESULT & mResult)
{
	AD_ITER  adIter;
	list<void *>::iterator listIter;
	AdRtbRequest * pRequest = static_cast<AdRtbRequest *>(data);
	string sArea = AD_AREA_ALL;

	adIter = find(sArea);
	if(end() != adIter)
	{
		list<void *> & list = adIter->second;
		for(listIter = list.begin(); listIter != list.end(); listIter ++)
		{
			AdRtbFilterResult & sResult = mResult[*listIter];
			sResult.m_uiMatch |= AD_FILTER_AREA;
		}
	}


	adIter = find(pRequest->m_sArea);
	if(end() != adIter)
	{
		list<void *> & list = adIter->second;
		for(listIter = list.begin(); listIter != list.end(); listIter ++)
		{
			AdRtbFilterResult & sResult = mResult[*listIter];
			sResult.m_uiMatch |= AD_FILTER_AREA;
		}
	}
	
	return AD_SUCCESS;
}


int AdFilterArea::AddData(void * data)
{
	AdPlanningUnit * pUnit = static_cast<AdPlanningUnit *>(data);
	list<string>::iterator sIter;

	if ( true  != pUnit->m_stTime.bPush)
	{
		return AD_SUCCESS;
	}

	for(sIter = pUnit->m_stTargetAudience.m_ltArea.begin();sIter!=pUnit->m_stTargetAudience.m_ltArea.end();sIter ++)
	{
		update(*sIter, static_cast<void *>(pUnit));
	}
	return AD_SUCCESS;
}
