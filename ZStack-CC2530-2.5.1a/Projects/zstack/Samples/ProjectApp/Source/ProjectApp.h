#ifndef PROJECTAPP_H
#define PROJECTAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ZComDef.h"

#define PROJECTAPP_ENDPOINT              10

#define PROJECTAPP_PROFID                0x0F04
#define PROJECTAPP_DEVICEID              0x0001
#define PROJECTAPP_DEVICE_VERSION        0
#define PROJECTAPP_FLAGS                 0

#define PROJECTAPP_MAX_CLUSTERS          2
#define PROJECTAPP_P2P_CLUSTERID         3

#define PROJECTAPP_TRANSPARENT_CLUSTERID 0x0001

extern const cId_t ProjectApp_ClusterList[PROJECTAPP_MAX_CLUSTERS];

extern endPointDesc_t ProjectApp_epDesc;
extern byte ProjectApp_TaskID;
extern byte ProjectApp_TransID;
extern afAddrType_t ProjectApp_DstAddr;

extern void ProjectApp_Init( byte task_id );

extern UINT16 ProjectApp_ProcessEvent( byte task_id, UINT16 events );

#ifdef __cplusplus
}
#endif

#endif /* PROJECTAPP_H */
