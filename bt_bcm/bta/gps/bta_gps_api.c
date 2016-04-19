#include "bta_api.h"
#include "bta_gps_api.h"
#include "bta_gps_act.h"
#include "bta_sys.h"
#include "gki.h"
#include <string.h>


static const tBTA_SYS_REG bta_gps_reg =
{
    bta_gps_sm_execute,
    NULL
};


/*******************************************************************************
**
** Function         BTA_GpsEnable
**
** Description      
**                  
**
** Returns          void
**
*******************************************************************************/
tBTA_STATUS BTA_GpsEnable(tBTA_GPS_CBACK *p_cback)
{
    int result = 1;
    tBTA_GPS_API_ENABLE *p_msg;
    /* register with BTA system manager */
    bta_sys_register(37, &bta_gps_reg);
		
	if ((p_msg = (tBTA_GPS_API_ENABLE  *) GKI_getbuf(sizeof(tBTA_GPS_API_ENABLE ))) != NULL)
    {
        p_msg->hdr.event = 9472;
        p_msg->p_cback = p_cback;
        bta_sys_sendmsg(p_msg);
        
        result = 0;
    }
	
	return result;
}

/*******************************************************************************
**
** Function         BTA_GpsDisable
**
** Description      
**                  
**
** Returns          void
**
*******************************************************************************/
void BTA_GpsDisable()
{      
    BT_HDR  *p_buf;
    if ((p_buf = (BT_HDR *) GKI_getbuf(sizeof(BT_HDR))) != NULL)
    {
        p_buf->event = 9473;
        bta_sys_sendmsg(p_buf);
    }
}
