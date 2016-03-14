/********************************************************/
/*	Geo Polymerization Technology Co, Ltd		*/
/*	Project:	AdAdpLog-3.0.0		*/
/*	Author:		Yuanji			*/
/*	Date:		2014_09_08			*/
/*	File:				*/
/********************************************************/

#include "AdAvro.h"


void AdAvro::DatUmInit()
{
	stDatum =  avro_datum_from_schema(stSchema);
}

void AdAvro::DatUmFree()
{
	avro_datum_decref(stDatum);
}

int AdAvro::loadJsonFile(char *JsonFile)
{
    FILE *fp = fopen(JsonFile, "r");
	try{
		if ( NULL !=  fp){
            int file_size;
            fseek( fp, 0, SEEK_END  );
            file_size = ftell(fp);
            fseek( fp , 0 , SEEK_SET );
            char  *uszGet = (char *)malloc( file_size * sizeof(char));
            fread( uszGet , file_size , sizeof(char) , fp );
			avro_schema_from_json(uszGet, strlen(uszGet), &stSchema, NULL);
			free(uszGet);
            fclose(fp);
		}
        else{
			std::cout << "loadJsonFile Error: " << JsonFile << std::endl;
        }
	}
	catch (exception& exErr){
        std::cout << "AdAvroJson:" << exErr.what() << std::endl;
		return AD_FAILURE;
	}

	return AD_SUCCESS;
}

int AdAvro::AdAvroWrite(char* pszPacket, int iSize)
{
	int iWrite = 0;
	avro_writer_t stWriter;

	stWriter = avro_writer_memory(pszPacket, iSize);
	if (AD_SUCCESS == avro_write_data(stWriter, NULL, stDatum)) {
		iWrite = avro_size_data(stWriter, NULL, stDatum);
	}
	/* validated
	if (Ad_SUCCESS == avro_write_data(stWriter, stSchema, *pstDatum)) {
		iWrite = avro_size_data(stWriter, stSchema, *pstDatum);
	}*/
	avro_writer_free(stWriter);

	return iWrite;
}


inline AV_DATUM AdAvro::AdAvroDatumGet(void* pData, UCHAR ucType)
{
	AV_DATUM stReturn;

	switch (ucType) {
		case AD_AV_INT:
			stReturn = avro_int32(*(int*)pData);
			break;
		case AD_AV_LNG:
			stReturn = avro_int64(*(long*)pData);
			break;
		case AD_AV_FLT:
			stReturn = avro_float(*(float*)pData);
			break;
		case AD_AV_STR:
			stReturn = avro_string((char*)pData);
			break;
		default:
			break;
	}

	return stReturn;
}

inline void AdAvro::AdAvroDatumSet(AV_DATUM* pstDatum, void* pData, UCHAR ucType)
{
	switch (ucType) {
		case AD_AV_INT:
			avro_int32_set(*pstDatum, *(int*)pData);
			break;
		case AD_AV_LNG:
			avro_int64_set(*pstDatum, *(long*)pData);
			break;
		case AD_AV_FLT:
			avro_float_set(*pstDatum, *(float*)pData);
			break;
		case AD_AV_STR:
			avro_string_set(*pstDatum, (char*)pData);
			break;
		default:
			break;
	}

	return;
}

void  AdAvro::AvSetOptional(char* pszName, void* pData, UCHAR ucType )
{
	AV_DATUM stUnion;
	AV_DATUM stValue;
	AdAvroRecordGet(&stDatum, pszName, &stUnion);

	if ( ucType == AD_AV_STR && memcmp(pData, "\0\0\0\0", sizeof(int)) == 0){
		avro_union_set_discriminant(stUnion, 1, &stValue);
	}
	else{
		avro_union_set_discriminant(stUnion, 0, &stValue);
	}
	AdAvroDatumSet(&stValue, pData, ucType);

	return;
}

void AdAvro::AvSetRequired(char* pszName, void* pData, UCHAR ucType )
{
	AV_DATUM stValue ;
	stValue = AdAvroDatumGet(pData, ucType);
	AdAvroRecordSet(&stDatum, pszName, &stValue);	
	avro_datum_decref(stValue);
	return;
}

void AdAvro::AdAvroDecrefSchema()
{
	avro_schema_decref(stSchema);
	return;
}



