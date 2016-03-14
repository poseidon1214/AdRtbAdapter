


#include <iostream>
#include "AdAvro.h"

using namespace std;

int main()
{
	
	AdAvro ob_avro;
	char file[128] = "./request.avsc";
	char pszPacket[1024]  = {0};
	ob_avro.loadJsonFile( file );
	
	ULONG time = 12345678910;
	char ip[16] = "192.168.167.190";

	while(1){
		ob_avro.DatUmInit();
		ob_avro.AvSetOptional((char *)"ip", (void *)ip, AD_AV_STR);
		ob_avro.AvSetRequired((char *)"req_time", (void *)&time, AD_AV_LNG);
		ob_avro.AdAvroWrite(pszPacket,1024);
		ob_avro.DatUmFree();
	}
	cout << pszPacket << endl;
	return 0;
}


