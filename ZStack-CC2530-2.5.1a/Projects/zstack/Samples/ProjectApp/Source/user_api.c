#include "user_api.h"
#include "ProjectApp.h"
#include "user_printf.h"
#include "string.h"

const uint8* devStates_str[11]=
{
  "DEV_HOLD",               // Initialized - not started automatically
  "DEV_INIT",               // Initialized - not connected to anything
  "DEV_NWK_DISC",           // Discovering PAN's to join
  "DEV_NWK_JOINING",        // Joining a PAN
  "DEV_NWK_REJOIN",         // ReJoining a PAN, only for end devices
  "DEV_END_DEVICE_UNAUTH",  // Joined but not yet authenticated by trust center
  "DEV_END_DEVICE",         // Started as device after authentication
  "DEV_ROUTER",             // Device joined, authenticated and is a router
  "DEV_COORD_STARTING",     // Started as Zigbee Coordinator
  "DEV_ZB_COORD",           // Started as Zigbee Coordinator
  "DEV_NWK_ORPHAN"          // Device has lost information about its parent..
};

void user_show_info(void)
{
  uint8 *MacAddr = 0;

  printf("Channel: %02X\r\n", _NIB.nwkLogicalChannel);
  printf("NwkPANID: %04X\r\n",_NIB.nwkPanId);

  NLME_GetCoordExtAddr(MacAddr);
  printf("Fath_Nwk: %04X  ",NLME_GetCoordShortAddr());
  printf("Fath_Mac: %04X%04X%04X%04X\r\n",
         *((uint16*)(&MacAddr[6])),
         *((uint16*)(&MacAddr[4])),
         *((uint16*)(&MacAddr[2])),
         *((uint16*)(&MacAddr[0])));

  MacAddr = NLME_GetExtAddr();
  printf("Self_Nwk: %04X  ",NLME_GetShortAddr());
  printf("Self_Mac: %04X%04X%04X%04X\r\n",
         *((uint16*)(&MacAddr[6])),
         *((uint16*)(&MacAddr[4])),
         *((uint16*)(&MacAddr[2])),
         *((uint16*)(&MacAddr[0])));
}
afStatus_t user_send_data( uint8 srcEP,
                           afAddrMode_t addrMode,
                           uint16 shortAddr,
                           uint8 dstEP,
                           uint16 clusterId,
                           char *data_buf,
                           uint8 *transID )
{
  endPointDesc_t *epDesc;
  epDesc = afFindEndPointDesc( srcEP );// Find the endpoint description
  if(epDesc == NULL) return ZFailure;

  afAddrType_t dst_addr;
  dst_addr.addrMode       = addrMode;
  dst_addr.addr.shortAddr = shortAddr;
  dst_addr.endPoint       = dstEP;

  afStatus_t status;
  status = AF_DataRequest( &dst_addr,
                           epDesc,
                           clusterId,
                           (byte)osal_strlen( data_buf ) + 1,
                           (byte *)data_buf,
                           transID,
                           AF_DISCV_ROUTE,
                           AF_DEFAULT_RADIUS );

  return status;
}
afStatus_t user_report_join(void)
{
  char send_buf[60] = {0}, temp_buf[30] = {0};

  // the 1st part
  strcat(send_buf, "New Device Join");

  // the 2nd part
  uint16 shortAddr;
  shortAddr = NLME_GetShortAddr();
  sprintf( temp_buf, ": %04X", shortAddr );
  strcat(send_buf, temp_buf);

  // the 3rd part
  uint8 *mac_addr;
  mac_addr = NLME_GetExtAddr();
  sprintf( temp_buf, ", %04X%04X%04X%04X",
           *((uint16*)(&mac_addr[6])),
           *((uint16*)(&mac_addr[4])),
           *((uint16*)(&mac_addr[2])),
           *((uint16*)(&mac_addr[0])));
  strcat(send_buf, temp_buf);

  // send join-network-info to Coordinator
  afStatus_t status;
  status = user_send_data( PROJECTAPP_ENDPOINT,
                           afAddr16Bit,
                           NWK_PAN_COORD_ADDR,
                           PROJECTAPP_ENDPOINT,
                           PROJECTAPP_TRANSPARENT_CLUSTERID,
                           send_buf,
                           &ProjectApp_TransID);

  return status;
}
void user_state_change( devStates_t state )
{
  printf("DEV: %s\r\n",devStates_str[state]);

  if((state == DEV_ZB_COORD)
   ||(state == DEV_ROUTER)
   ||(state == DEV_END_DEVICE))
  {
    printf("\r\n");
    user_show_info();
    printf("\r\n");

    user_report_join();
  }
}
