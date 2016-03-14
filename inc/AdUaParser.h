#include <map>
#include <list>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AdAcbm.h"
#include "AdGlobal.h"


#define AD_UA_DEF_IE	1
#define AD_UA_DEF_EDGE	2
#define AD_UA_DEF_360	3
#define AD_UA_DEF_CHROME	4
#define AD_UA_DEF_FIREFOX	5
#define AD_UA_DEF_SAFARI	6
#define AD_UA_DEF_MAXTHON	7
#define AD_UA_DEF_QQ	8
#define AD_UA_DEF_LB	9
#define AD_UA_DEF_OTHER	20


typedef map<void *, list<string> > UA_MAP_RESULT;

class AdUserAgentParser
{
public:
    AdUserAgentParser()
    {
        map<string, int> m_Global; 
        map<string, int>::iterator iter;
        m_Global["MSIE"] = AD_UA_DEF_IE;
        m_Global["Edge"] = AD_UA_DEF_EDGE;
        m_Global["360SE"] = AD_UA_DEF_360;
        m_Global["Chrome"] = AD_UA_DEF_CHROME;
        m_Global["Firefox"] = AD_UA_DEF_FIREFOX;
        m_Global["Safari"] = AD_UA_DEF_SAFARI;
        m_Global["Maxthon"] = AD_UA_DEF_MAXTHON;
        m_Global["QQBrowser"] = AD_UA_DEF_QQ;
        m_Global["LBBROWSER"] = AD_UA_DEF_LB;
        for(iter = m_Global.begin(); iter != m_Global.end(); iter++)
        {
            ac.AdAcbmTreeAdd( (string&)iter->first, (void*)iter->second);
        }
    };
    ~AdUserAgentParser(){};
    
    int parse(string &ua, UA_MAP_RESULT &mResult)
    {
	    int ret = ac.AdAcbmSearch(ua, mResult);
		if(  0 == ret )
		{
			list<string> tmp;	
			mResult[(void*)AD_UA_DEF_OTHER] = tmp;
		}
		return ret;
    };
    
private:
    AdAcbm ac;
    vector<string> vStr;
};

