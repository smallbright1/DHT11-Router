#include "ProjectAppCB.h"
#include "ProjectApp.h"

#include "ZDObject.h"

#include "hal_led.h"
#include "hal_key.h"

#include "user_api.h"
#include "user_printf.h"

void ProjectApp_ProcessZDOMsgsCB( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
#if defined( BLINK_LEDS )
      else
      {
        // Flash LED to show failure
        HalLedSet ( HAL_LED_4, HAL_LED_MODE_FLASH );
      }
#endif
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            ProjectApp_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            ProjectApp_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            ProjectApp_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}

void ProjectApp_HandleKeysCB( uint8 shift, uint8 keys )
{
  zAddrType_t dstAddr;

  // Shift is used to make each button/switch dual purpose.
  if ( shift )
  {
    if ( keys & HAL_KEY_SW_1 )
    {
    }
    if ( keys & HAL_KEY_SW_2 )
    {
    }
    if ( keys & HAL_KEY_SW_3 )
    {
    }
    if ( keys & HAL_KEY_SW_4 )
    {
    }
  }
  else
  {
    if ( keys & HAL_KEY_SW_1 )
    {
      // Since SW1 isn't used for anything else in this application...
#if defined( SWITCH1_BIND )
      // we can use SW1 to simulate SW2 for devices that only have one switch,
      keys |= HAL_KEY_SW_2;
#elif defined( SWITCH1_MATCH )
      // or use SW1 to simulate SW4 for devices that only have one switch
      keys |= HAL_KEY_SW_4;
#endif
    }

    if ( keys & HAL_KEY_SW_2 )
    {
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );

      // Initiate an End Device Bind Request for the mandatory endpoint
      dstAddr.addrMode = Addr16Bit;
      dstAddr.addr.shortAddr = 0x0000; // Coordinator
      ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(),
                            ProjectApp_epDesc.endPoint,
                            PROJECTAPP_PROFID,
                            PROJECTAPP_MAX_CLUSTERS, (cId_t *)ProjectApp_ClusterList,
                            PROJECTAPP_MAX_CLUSTERS, (cId_t *)ProjectApp_ClusterList,
                            FALSE );
    }

    if ( keys & HAL_KEY_SW_3 )
    {
    }

    if ( keys & HAL_KEY_SW_4 )
    {
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
      // Initiate a Match Description Request (Service Discovery)
      dstAddr.addrMode = AddrBroadcast;
      dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
      ZDP_MatchDescReq( &dstAddr, NWK_BROADCAST_SHORTADDR,
                        PROJECTAPP_PROFID,
                        PROJECTAPP_MAX_CLUSTERS, (cId_t *)ProjectApp_ClusterList,
                        PROJECTAPP_MAX_CLUSTERS, (cId_t *)ProjectApp_ClusterList,
                        FALSE );
    }
  }
}

void ProjectApp_DataConfirmCB( afDataConfirm_t *pkt )
{
  byte sentEP = pkt->endpoint;
  byte sentTransID = pkt->transID; // This should match the value sent
  ZStatus_t sentStatus = pkt->hdr.status;

  (void)sentEP;
  (void)sentTransID;

  // Action taken when confirmation is received.
  if ( sentStatus != ZSuccess )
  {
    // The data wasn't delivered -- Do something
  }
}

void ProjectApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->endPoint ) // Destination EndPoint
  {
    case PROJECTAPP_ENDPOINT:
      switch ( pkt->clusterId ) // Message's ClusterID
      {
        case PROJECTAPP_TRANSPARENT_CLUSTERID:
          printf("%s", pkt->cmd.Data);
          printf("\r\n");
          break;

        default:
          break;
      }
      break;

    default:
      break;
  }
}

void ProjectApp_StateChangeCB( devStates_t state )
{
  user_state_change(state);
}
