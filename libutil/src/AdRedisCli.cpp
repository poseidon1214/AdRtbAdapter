/***************************************
*	Description: Redis client, transfer data from redis server.
*	Author: wilson
*	Date: 2015/07/04
*
*	CopyRight: Adirect Technology Co, Ltd
*
****************************************/

#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "AdRedisCli.h"
#include "AdGlobal.h"


AdRedisCli::AdRedisCli()
{
	m_pGtCli = NULL;
	m_sIp.clear();
	m_iPort = 0;
}


AdRedisCli::AdRedisCli(string & sIp, int iPort)
{
	m_pGtCli = NULL;
	m_sIp = sIp;
	m_iPort = iPort;
}

AdRedisCli::~AdRedisCli()
{
	if(!m_sIp.empty())
	{
		m_sIp.clear();
	}
	if(m_pGtCli != NULL)
	{
		AdRedisClose();
	}
}
int AdRedisCli::AdRedisConnect()
{
	m_pGtCli = redisConnect(m_sIp.c_str(), m_iPort);
	if(m_pGtCli == NULL || m_pGtCli->err)
	{
		AdRedisClose();
		return AD_FAILURE;
	}
	return AD_SUCCESS;
}

void AdRedisCli::AdRedisClose()
{
	if(m_pGtCli)
	{
		redisFree(m_pGtCli);
		m_pGtCli = NULL;
	}
}

int AdRedisCli::AdRedisConnect(string& sIp, int iPort)
{
	m_sIp = sIp;
	m_iPort = iPort;
	m_pGtCli = redisConnect(m_sIp.c_str(), m_iPort);
	if(m_pGtCli == NULL || m_pGtCli->err)
	{
		AdRedisClose();
		return AD_FAILURE;
	}
	return AD_SUCCESS;
}

int AdRedisCli::AdRedisHkeys(string& key, list<string>& res)
{
	unsigned int i = 0;
	redisReply *reply = NULL;

	res.clear();

	if (key.empty() || NULL == (reply = (redisReply *)redisCommand(m_pGtCli,"HKEYS %s", key.c_str())) )
	{
		return AD_FAILURE;
	}

	if(reply->type == REDIS_REPLY_ARRAY)
	{
		for(i = 0; i < reply->elements; i++)
		{
			res.push_back(reply->element[i]->str);
		}
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		freeReplyObject(reply);
		return AD_SUCCESS;
	}

	freeReplyObject(reply);
	return AD_FAILURE;
}

int AdRedisCli::AdRedisHkeys(string& key, vector<string>& res)
{
	unsigned int i = 0;
	redisReply *reply = NULL;

	res.clear();

	if (key.empty() || NULL == (reply = (redisReply *)redisCommand(m_pGtCli,"HKEYS %s", key.c_str())) )
	{
		return AD_FAILURE;
	}

	if(reply->type == REDIS_REPLY_ARRAY)
	{
		for(i = 0; i < reply->elements; i++)
		{
			res.push_back(reply->element[i]->str);
		}
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	
	freeReplyObject(reply);
	return AD_FAILURE;
}

int AdRedisCli::AdRedisHGetAll( char *key,  map<int, int> & mValue)
{
	unsigned int	i	= 0;
	redisReply	*reply	= NULL;

	if ( key == NULL || NULL == (reply = (redisReply *) redisCommand( m_pGtCli, "HGETALL %s", key ) ) )
	{
		return(AD_FAILURE);
	}

	if ( reply->type == REDIS_REPLY_ARRAY )
	{
		for ( i = 0; i < reply->elements; i+=2 )
		{
			mValue[atoi(reply->element[i]->str)] = atoi(reply->element[i + 1]->str) ;
		}
		freeReplyObject( reply );
		return(AD_SUCCESS);
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		freeReplyObject(reply);
		return AD_SUCCESS;
	}

	freeReplyObject(reply);
	return(AD_FAILURE);
}

int AdRedisCli::AdRedisArrayHGetAll( vector<string>& vStr,  vector<map<int, int> > & vValue)
{
	unsigned int	i	= 0;
	unsigned int  j  = 0;
	redisReply	*reply	= NULL;
	vector<string>::iterator iter;

	//append cmd
	for(iter = vStr.begin(); iter != vStr.end(); iter ++)
	{
		if (REDIS_OK != redisAppendCommand( m_pGtCli, "HGETALL %s", (*iter).c_str() ))
       			return AD_FAILURE;
	}
	
	for(j = 0; j < vStr.size(); j ++)
	{
		 if (REDIS_OK != redisGetReply(m_pGtCli,  (void**)&reply)) 
		 {
			return AD_FAILURE;
		}
		if ( reply->type == REDIS_REPLY_ARRAY )
		{
			for ( i = 0; i < reply->elements; i+=2 )
			{
				vValue[j][atoi(reply->element[i]->str)] = atoi(reply->element[i + 1]->str) ;
			}
		}
		freeReplyObject(reply);
	}
	
	return  AD_SUCCESS;
}

int AdRedisCli::AdRedisHkeys( char *key, vector<string> & res )
{
	unsigned int	i	= 0;
	redisReply	*reply	= NULL;

	res.clear();

	if ( key == NULL || NULL == (reply = (redisReply *) redisCommand( m_pGtCli, "HKEYS %s", key ) ) )
	{
		return(AD_FAILURE);
	}

	if ( reply->type == REDIS_REPLY_ARRAY )
	{
		for ( i = 0; i < reply->elements; i++ )
		{
			res.push_back( reply->element[i]->str );
		}
		freeReplyObject( reply );
		return(AD_SUCCESS);
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	
	freeReplyObject(reply);
	return(AD_FAILURE);
}


int AdRedisCli::AdRedisGet(string& key, int *value)
{
	redisReply *reply = NULL;

	if( NULL == (reply = (redisReply *)redisCommand(m_pGtCli, "GET %s", key.c_str())) )
	{
		return AD_FAILURE;
	}	

	if(reply->type == REDIS_REPLY_INTEGER)
	{
		*value = reply->integer;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_STRING)
	{
		*value = atoi(reply->str);
		freeReplyObject(reply);
		return AD_SUCCESS;
	}	
	else if(reply->type == REDIS_REPLY_NIL)
	{
		*value = 0;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}	
	
	*value = 0;
	freeReplyObject(reply);
	return AD_FAILURE;
}

int AdRedisCli::AdRedisGet(char * key, int *value)
{
	redisReply *reply = NULL;

	if(key == NULL || NULL == (reply = (redisReply *)redisCommand(m_pGtCli, "GET %s",key)) )
	{
		return AD_FAILURE;
	}	

	if(reply->type == REDIS_REPLY_INTEGER)
	{
		*value = reply->integer;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_STRING)
	{
		*value = atol(reply->str);
		freeReplyObject(reply);
		return AD_SUCCESS;
	}	
	else if(reply->type == REDIS_REPLY_NIL)
	{
		*value = 0;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}	

	*value = 0;
	freeReplyObject(reply);
	return AD_FAILURE;	
}

int AdRedisCli::AdRedisHGet(string & key, string & field, long long * value)
{
	redisReply *reply = NULL;

	reply = (redisReply *)redisCommand(m_pGtCli, "HGET %s %s", key.c_str(), field.c_str());
	if( NULL == reply)
	{
		return AD_FAILURE;
	}	

	if(reply->type == REDIS_REPLY_INTEGER)
	{
		*value = reply->integer;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_STRING)
	{
		*value = atol(reply->str);
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		*value = 0;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}	

	*value = 0;
	freeReplyObject(reply);
	return AD_FAILURE;
}

int AdRedisCli::AdRedisHGet(string & key, string & field, string & res)
{
	redisReply *reply = NULL;
	res.clear();

	reply = (redisReply *)redisCommand(m_pGtCli, "HGET %s %s", key.c_str(), field.c_str());
	if( NULL == reply)
	{
		return AD_FAILURE;
	}	

	if(reply->type == REDIS_REPLY_STRING)
	{
		res = reply->str;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		freeReplyObject(reply);
		return AD_SUCCESS;
	}

	freeReplyObject(reply);
	return AD_FAILURE;
}

int AdRedisCli::AdRedisHGet(char * key, char * field, string & res)
{
	redisReply *reply = NULL;
	res.clear();

	reply = (redisReply *)redisCommand(m_pGtCli, "HGET %s %s", key, field);
	if( NULL == reply)
	{
		return AD_FAILURE;
	}	

	if(reply->type == REDIS_REPLY_STRING)
	{
		res = reply->str;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		freeReplyObject(reply);
		return AD_SUCCESS;
	}

	freeReplyObject(reply);
	return AD_FAILURE;
}


int AdRedisCli::AdRedisHGet(char * key, char * field, unsigned int *value)
{
	redisReply *reply = NULL;

	reply = (redisReply *)redisCommand(m_pGtCli, "HGET %s %s", key, field);
	if( NULL == reply)
	{
		return AD_FAILURE;
	}	

	if(reply->type == REDIS_REPLY_INTEGER)
	{
		*value = reply->integer;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_STRING)
	{
		*value = atoi(reply->str);
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		*value = 0;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}

	*value = 0;
	freeReplyObject(reply);
	return AD_FAILURE;
}

int AdRedisCli::AdRedisHGet(string & key, string & field, unsigned int *value)
{
	redisReply *reply = NULL;

	reply = (redisReply *)redisCommand(m_pGtCli, "HGET %s %s", key.c_str(), field.c_str());
	if( NULL == reply)
	{
		return AD_FAILURE;
	}	

	if(reply->type == REDIS_REPLY_INTEGER)
	{
		*value = reply->integer;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_STRING)
	{
		*value = atoi(reply->str);
		freeReplyObject(reply);
		return AD_SUCCESS;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		*value = 0;
		freeReplyObject(reply);
		return AD_SUCCESS;
	}

	*value = 0;
	freeReplyObject(reply);
	return AD_FAILURE;
}


int AdRedisCli::AdRedisHIncRby(string & key, string & field, int value)
{
	redisReply *reply = NULL;

	if( key.empty() || NULL == (reply = (redisReply *)redisCommand(m_pGtCli, "HINCRBY %s %s %d",key.c_str(), field.c_str(), value)) )
	{
		return AD_FAILURE;
	}
	freeReplyObject(reply);
	return AD_SUCCESS;
}

int AdRedisCli::AdRedisHIncRby(string & key, string & field, double value)
{
	redisReply *reply = NULL;

	if( key.empty() || NULL == (reply = (redisReply *)redisCommand(m_pGtCli, "HINCRBY %s %s %lf",key.c_str(), field.c_str(), value)) )
	{
		return AD_FAILURE;
	}
	freeReplyObject(reply);
	return AD_SUCCESS;
}


int AdRedisCli::AdRedisSadd(string &key, string &value)
{
	redisReply *reply = NULL;

	if( key.empty() || NULL == (reply = (redisReply *)redisCommand(m_pGtCli, "SADD %s %s",key.c_str(), value.c_str())) ){
		return AD_FAILURE;
	}
	freeReplyObject(reply);
	return AD_SUCCESS;
}

int AdRedisCli::AdRedisHSet(string& key, string & field, int value)
{
	redisReply *reply = NULL;

	if( key.empty() || NULL == (reply = (redisReply *)redisCommand(m_pGtCli, "HSET %s %s %d", key.c_str(), field.c_str(), value)) )
	{
		return AD_FAILURE;
	}
	freeReplyObject(reply);
	return AD_SUCCESS;
}

int AdRedisCli::AdRedisHSet(char * key, char * field, int value)
{
	redisReply *reply = NULL;

	if(NULL == (reply = (redisReply *)redisCommand(m_pGtCli, "HSET %s %s %d", key, field, value)) )
	{
		return AD_FAILURE;
	}
	freeReplyObject(reply);
	return AD_SUCCESS;
}

int AdRedisCli::AdRedisHSet(char * key, char * field, char * value)
{
	redisReply *reply = NULL;

	if(key == NULL || NULL == (reply = (redisReply *)redisCommand(m_pGtCli, "HSET %s %s %s", key, field, value)) )
	{
		return AD_FAILURE;
	}
	freeReplyObject(reply);
	return AD_SUCCESS;
}


int AdRedisCli::AdRedisPublish(string &topic, string &message)
{
	redisReply *reply = NULL;

	if( topic.empty() || NULL == (reply = (redisReply *)redisCommand(m_pGtCli, "PUBLISH %s %s", topic.c_str(), message.c_str())) )
	{
		return AD_FAILURE;
	}
	freeReplyObject(reply);
	return AD_SUCCESS;
}
