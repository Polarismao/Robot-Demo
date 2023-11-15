#ifndef _PHYCAN_H_
#define _PHYCAN_H_

void CAN_Configuartion(unsigned char can_baut,unsigned short Acr, unsigned short Amr);
void physical_can_send(unsigned long can_Id,unsigned char uc_exId, unsigned char can_data_lenth,unsigned char *can_data);
void USB_LP_CAN1_RX0_IRQHandler(void);

struct CF_NEED_SEND
{
	unsigned long ul_funId;
	unsigned short us_subId;
};
struct CF_NEED_SEND st_cf_cycleTxId;

unsigned long ul_phyCan_rxId[4];
unsigned char uc_phyCan_exId[4];
unsigned char uc_phyCan_rxLen[4];
unsigned char uc_phyCan_rxMessage[4][8];	
unsigned char uc_phyCan_rxFlag[4];


#endif

