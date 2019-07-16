#ifndef USER_API_H
#define USER_API_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ZDApp.h"
#include "AF.h"

extern void user_show_info(void);

/*********************************************************************
 * @param  srcEP      - Source End Point.
 * @param  addrMode   - Send Mode.
 * @param  shortAddr  - Short Address of Destination.
 * @param  dstEP      - Destination End Point.
 * @param   clusterId - A valid cluster ID as specified by the Profile.
 * @param  *data_buf  - A pointer to the data bytes to send.
 * @param  *transID   - A pointer to a byte which can be modified and which will
 *                      be used as the transaction sequence number of the msg.
 *                      Incremented by one if the return value is success.
 */
extern afStatus_t user_send_data( uint8 srcEP,
                                  afAddrMode_t addrMode,
                                  uint16 shortAddr,
                                  uint8 dstEP,
                                  uint16 clusterId,
                                  char *data_buf,
                                  uint8 *transID );

extern afStatus_t user_report_join(void);

extern void user_state_change( devStates_t state );

#ifdef __cplusplus
}
#endif

#endif /* USER_API_H */
