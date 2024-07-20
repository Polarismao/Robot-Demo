#ifndef _OBJ232_H_
#define _OBJ232_H_

void Obj232_ModuleStatus(BOOL b_ActStatus);
void Obj232_ParaInit(void);
void Obj232_MainDeal(void);
void Obj232_rece_deal(unsigned char* pc_recvData ,unsigned short us_232RecvLen,
						  unsigned char* pc_tkeData,  unsigned short* ps_tkeDataLen,
					      unsigned char* pc_mnkData,  unsigned short* ps_mnkDataLen,
						  unsigned short* ps232_race_flag,
						  unsigned long* pl_enable_flag);
						  
void Obj232_send_deal(unsigned char* uc_mnkReceTab, unsigned short* ps232_race_flag, unsigned long* pl_enable_flag, unsigned char* pc_232SendData, unsigned short* ps_232SendLengh);
unsigned short Obj232_CrcCal(unsigned char *data_value,unsigned char length);

#endif

