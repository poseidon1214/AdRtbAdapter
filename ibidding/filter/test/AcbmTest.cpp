#include <map>
#include <list>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AdAcbm.h"
#include "AdGlobal.h"

int AdRtbOutput(char * buff, int len)
{
	return AD_FAILURE;
}


int main(int argc, char ** argv)
{
	AdAcbm ac;
	int ret;
	vector<string> vStr;
	char buff[8];
	map<void *, list<string> > mResult;
	map<void *, list<string> >::iterator iter;

	for(int i = 0; i < 8; i ++)
	{
		buff[i] = -1 * (i + 1);
	}
	buff[8] = 0;
	vStr.push_back(buff);
	vStr.push_back("adlkajflkasjdf");
	vStr.push_back("xkjzixdclkukjf");
	vStr.push_back("jzixdckjkjmlkukjf");
	vStr.push_back("xkjzixdclkukjf");
	vStr.push_back("jxmkcjimm./zsfiowejf");
	vStr.push_back("qikquerklqwjel,,oamdf");
	vStr.push_back(",mij18934ualksdu913f");
	vStr.push_back("alksdfjimn");
	vStr.push_back("198374198374");
	vStr.push_back("minijnx");
	for(int i = 0; i < vStr.size(); i ++)
	{
		ret = ac.AdAcbmTreeAdd(vStr[i], (void *)i);
		if(ret != AD_SUCCESS)
		{
			printf("error\n");
			return ret;
		}
	}
//	ac.AdAcbmTreeBuild();
	string test = "minijnxakdadks";
	ac.AdAcbmSearch(test, mResult);
	test = "198374198374adkfja";
	ac.AdAcbmSearch(test, mResult);
	test = buff;
	ac.AdAcbmSearch(test, mResult);
	for(iter = mResult.begin(); iter != mResult.end(); iter ++)
	{
		printf("%d\n", iter->first);
	}
	return 0;
}
