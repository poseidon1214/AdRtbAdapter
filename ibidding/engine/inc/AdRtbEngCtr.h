/********************************************************/
/*	Geo Polymerization Technology Co, Ltd		*/
/*	Project:	GtRtbEng-6.6.0			*/
/*	Author:		zp			*/
/*	Date:		2015_06_09			*/
/*	File:		GtRtbEngCtr.h		*/
/********************************************************/

#ifndef _AD_RTB_ENG_CTR_H
#define _AD_RTB_ENG_CTR_H
#include "AdGlobal.h"
#include "AdRtbBase.h"
#include "AdPlanningData.h"
#include "AdClassifier.h"


class AdRtbEngCtrModel
{
public:
    AdRtbEngCtrModel();
    ~AdRtbEngCtrModel();
    int AdRtbEngInit(string & sDir);
    int AdRtbEngDeinit();
    int AdRtbEngCtrGet(AdRtbRequest * pRequest, AdPlanningUnit *AdUnit, std::string *m_stMark,UINT *uiPrice);

protected:

private:
    int GtRtbEngGetHost(char *url, char *host);
    int AdRtbEngPrice(AdRtbRequest * pRequest, AdPlanningUnit *AdUnit, UINT *uiPrice, double ctr);

    gfs::Classifier *pClassifier;

};

#endif










