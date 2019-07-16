#ifndef PROJECTAPPCB_H
#define PROJECTAPPCB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ZComDef.h"

#include "ZDApp.h"
#include "ZDProfile.h"

extern void ProjectApp_ProcessZDOMsgsCB( zdoIncomingMsg_t *inMsg );
extern void ProjectApp_HandleKeysCB( byte shift, byte keys );
extern void ProjectApp_DataConfirmCB( afDataConfirm_t *pkt );
extern void ProjectApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
extern void ProjectApp_StateChangeCB( devStates_t state );

#ifdef __cplusplus
}
#endif

#endif /* PROJECTAPPCB_H */
