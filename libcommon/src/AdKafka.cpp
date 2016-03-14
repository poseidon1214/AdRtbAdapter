/********************************************************/
/*	Geo Polymerization Technology Co, Ltd		*/
/*	Project:	AdAdpLog-3.0.0		*/
/*	Author:		Yuanji			*/
/*	Date:		2014_09_08			*/
/*	File:				*/
/********************************************************/

#include "AdKafka.h"

int AdKafka::initProducer(char *pszBroker){

	m_pstKConf = rd_kafka_conf_new();
	rd_kafka_conf_set(m_pstKConf, AD_KK_COMP, AD_KK_GZIP, NULL, 0);
	rd_kafka_conf_set(m_pstKConf, MESSAGE_CONF, MESSAGE_NUM, NULL, 0);
	m_pstKfk = rd_kafka_new(AD_KK_PROD, m_pstKConf, NULL, 0);
	if( 0 != rd_kafka_brokers_add(m_pstKfk, pszBroker))
	{	
		return AD_SUCCESS;
	}
	else{
		std::cout << "Kafka Connect Error!" <<std::endl;
		return AD_FAILURE;
	}

}

int AdKafka::ProduceMessage(const char *topic_name, char *mesg, int iLength)
{

	RD_KF_TP_T* m_pstTop = NULL;
	RD_KF_TP_CF_T* m_pstTConf = NULL;

	int partition = AdKfkRand();
	m_pstTop = rd_kafka_topic_new(m_pstKfk, topic_name, m_pstTConf);
	
	if ( NULL != m_pstTop )
	{
		rd_kafka_produce(m_pstTop, partition, RD_KAFKA_MSG_F_COPY, mesg, iLength, NULL, 0, NULL);
		rd_kafka_topic_destroy(m_pstTop);

		return AD_SUCCESS;
	}
	else{
		return AD_FAILURE;
	}
}


void AdKafka::AdKfkPoll(RD_KF_T* pstKafka)
{
	rd_kafka_poll(pstKafka, 0);
	return;
}


int AdKafka::AdKfkRand()
{
	srand((unsigned)time(NULL));
	int r = rand()/AD_KK_PART;
	return r;
}