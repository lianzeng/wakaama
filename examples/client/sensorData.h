//
// Created by impact on 18-7-5.
//

#ifndef LWM2MCLIENT_JSONDATA_H
#define LWM2MCLIENT_JSONDATA_H


#include <memory.h>
#include "liblwm2m.h"
#include <stdlib.h>
#include <stdio.h>

#define  MAX_INSTANCE_PER_OBJ    5
#define  MAX_RESOURCE_PER_INSTANCE  20
#define  MAX_VALUE_LENGTH_PER_RESOURCE   50
#define  elementsOf(array) (sizeof(array)/sizeof(array[0]))
#define  ENSURE_END_WITH_NULL_CHAR(array) {array[elementsOf(array)-1] = '\0';}

#define RES_NUM_VEHICLE       13 //be consistent with object_vehicle.c
#define RES_NUM_TEMPERATURE   12 //total num of above resourceId
#define RES_NUM_PM25          2


#define true   1
#define false  0

/////////////  these struct is used to encode data when report to lwm2m server

typedef struct{
    int  resId;
    char dataType; //'s'=string, 'i'=int, 'f'= float
}Lwm2mResEncodeType;



typedef struct{

    int ojbId;
    int resNum;
    Lwm2mResEncodeType types[MAX_RESOURCE_PER_INSTANCE];

}Lwm2mObjDataType;

extern bool lwm2m_data_encode_common(const int objId,const int resId,const char* value, lwm2m_data_t *dataP);
///////////////////


typedef struct{
	int  objId;
	bool recvReadRequest;
}ReadRequestRecord; //trig by read request, when receive data, immediately report to server without consider the pmin/pmax;

typedef struct {
    int resId;
    char value[MAX_VALUE_LENGTH_PER_RESOURCE];
}ResourceValue;


typedef struct{
    int instId;
    int resNum;
    ResourceValue resValues[MAX_RESOURCE_PER_INSTANCE];
}InstanceData;

typedef struct{
    int objId;
    int instNum;
    InstanceData data[MAX_INSTANCE_PER_OBJ];
}ObjectData;


typedef void (*FunctionUpdateMeasure)(const ObjectData* , lwm2m_context_t* );

typedef struct{
    int objId;
    FunctionUpdateMeasure  pfun;
}ObjIdFuncMap;


static inline void appendResourceIdValue(const char* resId, const char *resV, InstanceData *instData)
{
    if(strlen(resV) >= MAX_VALUE_LENGTH_PER_RESOURCE)
        fprintf(stderr,"warning:resID(%d) value exceed max length\n", atoi(resId));

    int resNum = instData->resNum;
    if(resNum < MAX_RESOURCE_PER_INSTANCE)
    {
        instData->resValues[resNum].resId = atoi(resId);//use strtol() to replace atoi()
        strncpy(instData->resValues[resNum].value, resV, MAX_VALUE_LENGTH_PER_RESOURCE);
        ENSURE_END_WITH_NULL_CHAR(instData->resValues[resNum].value);
        instData->resNum += 1;
    }
}


static inline void markSensorValueChangedToTrigLaterReport(const int objId, lwm2m_context_t* context)
{

    lwm2m_uri_t urip = {LWM2M_URI_FLAG_OBJECT_ID, objId, 0, 0};
    lwm2m_resource_value_changed(context, &urip);
}


#endif //LWM2MCLIENT_JSONDATA_H
