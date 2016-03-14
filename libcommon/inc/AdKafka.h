/********************************************************/
/*	Geo Polymerization Technology Co, Ltd		*/
/*	Project:	AdAdpLog-3.0.0		*/
/*	Author:		Yuanji			*/
/*	Date:		2014_09_08			*/
/*	File:				*/
/********************************************************/


#ifndef _ADKAFKA_H
#define	_ADKAFKA_H


#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <librdkafka/rdkafka.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "AdGlobal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define	AD_KK_PART			0x24

#define	AD_KK_PROD			RD_KAFKA_PRODUCER
#define	AD_KK_CONS			RD_KAFKA_CONSUMER

#define MESSAGE_NUM			(char*)"10"
#define MESSAGE_CONF		(char*)"batch.num.messages"

#define	AD_KK_GZIP			(char*)"gzip"
#define	AD_KK_SNAP			(char*)"snappy"
#define	AD_KK_COMP			(char*)"compression.codec"

#define RD_KF_T 				rd_kafka_t
#define RD_KF_CF_T			rd_kafka_conf_t
#define RD_KF_TP_T			rd_kafka_topic_t
#define RD_KF_TP_CF_T 	rd_kafka_topic_conf_t

class AdKafka
{
public:
	AdKafka(){}
	~AdKafka(){}

	/* init */
	int initProducer(char *pszBroker);
	int ProduceMessage(const char *topic_name, char *mesg, int iLength);


protected:
	int AdKfkRand();
	void AdKfkPoll(RD_KF_T* pstKafka);
	
private:	
	RD_KF_T* m_pstKfk;
	RD_KF_CF_T* m_pstKConf;

};



#ifdef __cplusplus
}
#endif

#endif /* _AdKAFKA_H */

