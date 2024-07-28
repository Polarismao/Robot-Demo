#ifndef _OBJ232_H_
#define _OBJ232_H_

void Obj232_ModuleStatus(BOOL b_actStatus);
void Obj232_ParaInit(void);
void Obj232_MainDeal(void);
void Obj232_rece_deal();
void Obj232_send_deal();
unsigned short Obj232_CrcCal(unsigned char *data_value,unsigned char uc_length);

#endif

