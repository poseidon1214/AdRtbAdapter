/***************************************
*	Description: Redis client, transfer data from redis server.
*	Author: wilson
*	Date: 2015/07/17
*
*	CopyRight: adirect Technology Co, Ltd
*
****************************************/

#ifndef _REDIS_CLI__
#define _REDIS_CLI__

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <list>
#include <map>
#include <hiredis/hiredis.h>

using namespace std;  

typedef struct redVal{
    char stBuf[32];
}REDVAL_S;

#define  AdRedisClient redisContext

class AdRedisCli
{
public:
	AdRedisCli();
	AdRedisCli(string& sIp, int iPort, string password="");
	virtual ~AdRedisCli();

	int AdRedisConnect(string& strIp, int port, string password="");
	int AdRedisConnect(void);
	void AdRedisClose();

	template<class out_type,class in_value>
	out_type AdRedisConvert(const in_value & t)
	{
		stringstream stream;
		out_type result;
		stream<<t;
		stream>>result;
		return result;
	};

	int AdRedisHkeys(string& key, vector<string>& res);
	int AdRedisHkeys(char * key, vector<string>& res);
	int AdRedisHkeys(string& key, list<string>& res);
	int AdRedisGet(string& key, int *value);

	int AdRedisGet(char * key,  int *value);
	int AdRedisHGet(string & key, string & field, long long * value);
	int AdRedisHGet(string & key, string & field, string & res);
	int AdRedisHGet(char * key, char * field, string & res);
	int AdRedisHGet(string & key, string & field, unsigned int *value);
	int AdRedisHGet(char * key, char * field, unsigned int *value);
	int AdRedisHIncRby(string & key, string & field, int value);
	int AdRedisHIncRby(string & key, string & field, double value);
	int AdRedisSadd(string &key, string &value);
	int AdRedisHSet(string& key, string & field, int value);
	int AdRedisHSet(char * key, char * field, int value);
	int AdRedisHSet(char * key, char * field, char * value);
	int AdRedisPublish(string &topic, string &message);
	int AdRedisHGetAll( char *key,  map<int, int> & mValue);
	int AdRedisArrayHGetAll( vector<string>& vStr,  vector<map<int, int> > & vValue);

protected:
	AdRedisClient *m_pGtCli;		
	string m_sIp;
	string m_sPasswd;
	int  m_iPort;
private:

};
#endif
