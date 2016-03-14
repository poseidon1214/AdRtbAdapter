/***************************************
*	Description: class Admap.
*	Author: wilson
*	Date: 2015/07/20
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/
#include "AdMap.h"

bool AdMap::empty()
{
	return m_mMap.empty();
}

ULONG AdMap::size()
{
	return (ULONG)m_mMap.size();
}

AD_ITER AdMap::end()
{
	return m_mMap.end();
}

AD_ITER AdMap::begin()
{
	return m_mMap.begin();
}

void AdMap::clear()
{
	AD_ITER iteCur = m_mMap.begin();

	for (; iteCur != m_mMap.end(); iteCur ++) {
		iteCur->second.clear();
	}

	m_mMap.clear();

	return;
}

void AdMap::display()
{
	return;
}

void AdMap::del(string& Key)
{
	m_mMap.erase(Key);
	return;
}

bool  AdMap::insert(string& Key, list<void *>& Value)
{
	std::pair<AD_ITER, bool> pairReturn;
	std::pair<string,list<void *> > pairInsert(Key, Value);

	pairReturn = m_mMap.insert(pairInsert);

	return pairReturn.second;
}

void AdMap::update(string& Key, list<void *> & Value)
{
	m_mMap[Key] = Value;
}

void AdMap::update(string& Key, void * id)
{
	m_mMap[Key].push_back(id);
}

void AdMap::update(char * Key, void * id)
{
	m_mMap[Key].push_back(id);
}

AD_ITER AdMap::find(string & Key)
{
	return m_mMap.find(Key);
}

AD_ITER AdMap::find(char * Key)
{
	return m_mMap.find(Key);
}


int AdMap::match(void * data, AD_RESULT & mResult)
{
	return 0;
}

int AdMap::AddData(void * data)
{
	return 0;
}

string & AdMap::name()
{
	return m_sName;
}

