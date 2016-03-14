/********************************************************/
/*	Geo Polymerization Technology Co, Ltd		*/
/*	Project:	AdAdpLog-3.0.0		*/
/*	Author:		Yuanji			*/
/*	Date:		2014_09_08			*/
/*	File:				*/
/********************************************************/

#ifndef _ADAVRO_H
#define	_ADAVRO_H

#include <avro.h>
#include <fstream>
#include <string>
#include "AdGlobal.h"

using std::exception;

#define	AD_AV_INT			0x01
#define	AD_AV_LNG			0x02
#define	AD_AV_FLT			0x04
#define	AD_AV_STR			0x08


#define AV_DATUM	avro_datum_t
#define AV_SCHEM	avro_schema_t


class AdAvro
{
public:
	AdAvro(){}
	~AdAvro(){
		AdAvroDecrefSchema();
	};

	/* json to schema */
	int loadJsonFile(char *JsonFile);

    /* data prepare */
    void DatUmInit();
	void DatUmFree();

	/* Key Value Type Datum */
	void AvSetOptional(char* pszName, void* pData, UCHAR ucType );
	void AvSetRequired(char* pszName, void* pData, UCHAR ucType );
	
	/* avro pack */
	int AdAvroWrite(char* pszPacket, int iSize);

private:
	/* data op */
	AV_DATUM AdAvroSchema();
	AV_DATUM AdAvroDatumGet(void* pData, UCHAR ucType);
	
	int AdAvroRecordSet(AV_DATUM* pstDatum, char* pszName, AV_DATUM* pstValue);
	void AdAvroDatumSet(AV_DATUM* pstDatum, void* pData, UCHAR ucType);
	void AdAvroRecordGet(AV_DATUM* pstDatum, char* pszName, AV_DATUM* pstValue);
	/* del */
	void AdAvroDecrefSchema();

	AV_SCHEM stSchema;
    AV_DATUM stDatum;
};


inline int AdAvro::AdAvroRecordSet(AV_DATUM* pstDatum, char* pszName, AV_DATUM* pstValue)
{
	return avro_record_set(*pstDatum, pszName, *pstValue);
}

inline void AdAvro::AdAvroRecordGet(AV_DATUM* pstDatum, char* pszName, AV_DATUM* pstValue)
{
	avro_record_get(*pstDatum, pszName, pstValue);

	return;
}




#endif /* _AdAVRO_H */

