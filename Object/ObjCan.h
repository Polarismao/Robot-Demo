#ifndef _OBJCAN_H_
#define _OBJCAN_H_

void Cf_Can_ParaInit(void);
void Cf_CanMainDeal(void);
void Cf_CanStdRxDeal(void);
void Cf_CanResetDeal(void);
void Cf_CanTxDeal(void);
void Cf_CanEvtChk(void);
void Cf_TxIdJudge(void);
void Cf_TxIdCycle(void);
void Cf_CanTxData(void);
void Cf_EvtWaitTimeInc(void);
void Cf_InsertReqList(unsigned short us_index,unsigned long ul_funId,unsigned short us_subId,unsigned char uc_exId,unsigned short us_tickTotal,unsigned short us_timeLimit,unsigned short us_doorType);

unsigned short us_cf_canSendTimer;
unsigned short us_cf_cycleStep;
unsigned short us_cf_stepToggle;
unsigned short us_cf_judgeToggle;
unsigned short us_cf_needSend;
unsigned short us_cf_insertWaitLongest;

struct CF_INSERT st_cf_insertReq[CF_INSERT_MAX1];
CF_CAN st_cf_rcecFrame;
CF_CAN st_cf_sendFrame;

struct CF_NEED_SEND
{
	unsigned long ul_funId;
	unsigned short us_subId;
};
struct CF_NEED_SEND st_cf_cycleTxId;

#endif

