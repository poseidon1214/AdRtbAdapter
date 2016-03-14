


#include "AdKafka.h"

int main()
{
	AdKafka ob_client;
	char * pszBroker = "10.111.32.106:6667";
	char msg[16] = "aaaaaaaa";
	ob_client.initProducer(pszBroker);
	ob_client.ProduceMessage("test", msg, strlen(msg));
	
	return 0;
}

