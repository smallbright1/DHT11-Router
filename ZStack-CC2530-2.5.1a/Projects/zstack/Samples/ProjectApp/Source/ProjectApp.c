#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "ProjectApp.h"
#include "ProjectAppCB.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"

#include "user_uart0.h"

const cId_t ProjectApp_ClusterList[PROJECTAPP_MAX_CLUSTERS] =
{
  PROJECTAPP_TRANSPARENT_CLUSTERID,
  PROJECTAPP_P2P_CLUSTERID
};

const SimpleDescriptionFormat_t ProjectApp_SimpleDesc =
{
  PROJECTAPP_ENDPOINT,              //  int Endpoint;
  PROJECTAPP_PROFID,                //  uint16 AppProfId[2];
  PROJECTAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  PROJECTAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  PROJECTAPP_FLAGS,                 //  int   AppFlags:4;
  PROJECTAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)ProjectApp_ClusterList,  //  byte *pAppInClusterList;
  PROJECTAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)ProjectApp_ClusterList   //  byte *pAppInClusterList;
};

endPointDesc_t ProjectApp_epDesc;
byte ProjectApp_TaskID;
byte ProjectApp_TransID;  // This is the unique message ID (counter)
afAddrType_t ProjectApp_DstAddr;

void ProjectApp_Init( uint8 task_id )
{
  ProjectApp_TaskID = task_id;
  ProjectApp_TransID = 0;

  ProjectApp_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  ProjectApp_DstAddr.endPoint = 0;
  ProjectApp_DstAddr.addr.shortAddr = 0;

  // Fill out the endpoint description.
  ProjectApp_epDesc.endPoint = PROJECTAPP_ENDPOINT;
  ProjectApp_epDesc.task_id = &ProjectApp_TaskID;
  ProjectApp_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&ProjectApp_SimpleDesc;
  ProjectApp_epDesc.latencyReq = noLatencyReqs;
  afRegister( &ProjectApp_epDesc ); // Register the endpoint description with the AF

  // Register for all key events - This app will handle all key events
  RegisterForKeys( ProjectApp_TaskID );

  // Configure and Start Uart0

  USER_Uart0_Init(HAL_UART_BR_115200);
  // Update the display
#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "ProjectApp", HAL_LCD_LINE_1 );
#endif

  ZDO_RegisterForZDOMsg( ProjectApp_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( ProjectApp_TaskID, Match_Desc_rsp );
}

uint16 ProjectApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( ProjectApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          ProjectApp_ProcessZDOMsgsCB( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          ProjectApp_HandleKeysCB( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case AF_DATA_CONFIRM_CMD:
          ProjectApp_DataConfirmCB( (afDataConfirm_t *)MSGpkt );
          break;

        case AF_INCOMING_MSG_CMD:
          ProjectApp_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          ProjectApp_StateChangeCB( (devStates_t)(MSGpkt->hdr.status) );
          break;

        default:
          break;
      }
      osal_msg_deallocate( (uint8 *)MSGpkt ); // Release the memory
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( ProjectApp_TaskID ); // Next
    }

    return (events ^ SYS_EVENT_MSG); // return unprocessed events
  }

  return 0; // Discard unknown events
}
