#ifndef _PHYPUBLIC_H_
#define _PHYPUBLIC_H_
#include "stm32f10x.h"

extern __asm void PhySys_JumpToBoot(unsigned long SPaddr,unsigned long PCaddr);
extern void physical_can_send(unsigned long can_Id,unsigned char uc_exId, unsigned char can_data_lenth,unsigned char *can_data);

#endif


