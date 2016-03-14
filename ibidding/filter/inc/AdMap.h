/***************************************
*	Description: class Admap.
*	Author: wilson
*	Date: 2015/07/20
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#ifndef _AD_MAP_H
#define	_AD_MAP_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "AdGlobal.h"
#include "AdFilterBase.h"


using namespace std;

typedef std::map<string, list<void *> >	AD_MAP;
typedef std::map<string, list<void *> >::iterator	AD_ITER;

#define MODULE_NAME   "AdMapBase"

class AdMap
{
public:
	AdMap() 
	{
		m_sName = MODULE_NAME;
	};
	virtual ~AdMap() 
	{
		clear();
	};

	virtual int match(void * data, AD_RESULT & mResult);
	virtual int AddData(void * data);
	virtual string &  name();
	
protected:
	bool empty();
	ULONG size();
	AD_ITER end();
	AD_ITER begin();
	void clear();
	void display();
	void del(string& Key);
	bool insert(string& Key, list<void *> & Value);
	void update(string& Key, list<void *> & Value);
	void update(string& Key, void * id);
	void update(char *Key, void * id);
	AD_ITER find(string & Key);
	AD_ITER find(char * Key);

private:
	AD_MAP m_mMap;
	string m_sName;
};

#endif 
