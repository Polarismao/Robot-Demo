#include "ObjPublic.h"
#include "ObjCan.h"

/**
 * @brief CAN模块初始化
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void ObjCan_ParaInit(void)
{


}

/**
 * @brief CAN模块处理主函数
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void ObjCan_MainDeal(void)
{
	unsigned short i=0, m=0;
    
	for(m=0;m<4;m++)
	{
		if(uc_phyCan_rxFlag[m])
		{
			st_cf_rcecFrame.COB_ID = ul_phyCan_rxId[m];
			st_cf_rcecFrame.uc_exId = uc_phyCan_exId[m];
			st_cf_rcecFrame.DLC = uc_phyCan_rxLen[m];
			for(i=0; i<st_cf_rcecFrame.DLC; i++)
			{
				st_cf_rcecFrame.Data[i] = uc_phyCan_rxMessage[m][i];
			}
			uc_phyCan_rxFlag[m] = 0;
		}
	}
	us_cf_canSendTimer += CF_2MS;
    
	if(us_cf_canSendTimer >= CF_10MS)
	{
		us_cf_canSendTimer=0;
		Cf_CanTxDeal();
		if(us_cf_needSend)
		{
			us_cf_needSend = 0;
			physical_can_send(st_cf_sendFrame.COB_ID, st_cf_sendFrame.uc_exId, st_cf_sendFrame.DLC, st_cf_sendFrame.Data);
		}
	}

	if(0 == st_cf_rcecFrame.uc_exId)
		Cf_CanStdRxDeal();
	else
		Cf_CanExtRxDeal();

	Cf_CanResetDeal();
}

/**
 * @brief 标准帧接收处理函数
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void Cf_CanStdRxDeal(void)
{
	if(st_cf_rcecFrame.COB_ID == 0x100)
	{}
}


/**
 * @brief 扩展帧接收处理函数
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void Cf_CanExtRxDeal(void)
{
	if(st_cf_rcecFrame.COB_ID == 0x37FF)
	{}
}


/**
 * @brief CAN发送处理
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void Cf_CanTxDeal(void)
{
	Cf_CanEvtChk();            //事件帧判断
	us_cf_stepToggle ^= 0x01;
	if(us_cf_stepToggle&0x01)
		Cf_TxIdCycle();
	
	Cf_EvtWaitTimeInc();      //事件帧紧急情况判断
	Cf_TxIdJudge();			  //发送CANID选择
	Cf_CanTxData();			  //发送帧内容填充
}


/**
 * @brief 查询是否有事件帧
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void Cf_CanEvtChk(void)
{
	Cf_InsertReqList(FRM100IDE60_FDOOR, 0x99, 0xF8, 0, 1, CF_200MS);
}


/**
 * @brief CAN周期帧处理
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void Cf_TxIdCycle(void)
{
	if(st_cf_cycleTxId.ul_funId)
		return;

	switch(us_cf_cycleStep)
	{
		case 0:
		{
			break;	
		}
		default:
			break;
	}
	us_cf_cycleStep++;
	if(us_cf_cycleStep>4)
		us_cf_cycleStep=0;
}

/**
 * @brief 判断所有事件的紧急程度
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void Cf_EvtWaitTimeInc(void)
{
	unsigned short i=0;
	unsigned short us_waitPercentTmp = 0;

	us_cf_insertWaitLongest = 0xffff;
	
	for(i=0; i<CF_INSERT_MAX1; i++)
	{
		if(st_cf_insertReq[i].ul_funId)
		{
			st_cf_insertReq[i].us_timeCur += CF_10MS;
			if(st_cf_insertReq[i].us_timeCur > st_cf_insertReq[i].us_timeLimit)
			{
				st_cf_insertReq[i].us_timeCur = st_cf_insertReq[i].us_timeLimit; 
			}
			if(st_cf_insertReq[i].us_timeLimit==0)
			{
				st_cf_insertReq[i].us_waitPercent = 100;
			}
			else
			{
				st_cf_insertReq[i].us_waitPercent = (unsigned long)(st_cf_insertReq[i].us_timeCur*100)/st_cf_insertReq[i].us_timeLimit;
			}
			if(st_cf_insertReq[i].us_waitPercent > us_waitPercentTmp)
			{
				us_waitPercentTmp = st_cf_insertReq[i].us_waitPercent;
				us_cf_insertWaitLongest = i;
			}
		}
	}
}

/**
 * @brief 发送CANID选择（事件帧/周期帧）
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void Cf_TxIdJudge(void)
{
	st_cf_sendFrame.COB_ID = 0;
	st_cf_sendFrame.ul_funId = 0;
	st_cf_sendFrame.us_subId = 0;
  	st_cf_sendFrame.uc_exId = 0;

	us_cf_judgeToggle ^= 0x01;
	if(us_cf_judgeToggle&0x01)
	{
		if(us_cf_insertWaitLongest != 0xffff)//有紧急的事件
		{
			st_cf_sendFrame.ul_funId = st_cf_insertReq[us_cf_insertWaitLongest].ul_funId;
			st_cf_sendFrame.us_subId = st_cf_insertReq[us_cf_insertWaitLongest].us_subId;
			st_cf_sendFrame.uc_exId = st_cf_insertReq[us_cf_insertWaitLongest].uc_exId;
			st_cf_insertReq[us_cf_insertWaitLongest].us_tickLeft--;
			st_cf_insertReq[us_cf_insertWaitLongest].us_timeCur = 0;
			if(st_cf_insertReq[us_cf_insertWaitLongest].us_tickLeft==0)
			{
				st_cf_insertReq[us_cf_insertWaitLongest].ul_funId = 0;
				st_cf_insertReq[us_cf_insertWaitLongest].us_subId = 0;
				st_cf_insertReq[us_cf_insertWaitLongest].uc_exId = 0;
				st_cf_insertReq[us_cf_insertWaitLongest].us_timeLimit = 0;
				st_cf_insertReq[us_cf_insertWaitLongest].us_waitPercent = 0;
			}
		}
		else if(st_cf_cycleTxId.ul_funId)
		{
			st_cf_sendFrame.ul_funId = st_cf_cycleTxId.ul_funId;
			st_cf_sendFrame.us_subId = st_cf_cycleTxId.us_subId;
			if((st_cf_sendFrame.ul_funId==0x1fff)||(st_cf_sendFrame.ul_funId==0x5fff))
				st_cf_sendFrame.uc_exId = 1;
			else
				st_cf_sendFrame.uc_exId = 0;
			st_cf_cycleTxId.ul_funId =0;
			st_cf_cycleTxId.us_subId =0;
		}
	}
	else
	{
		if(st_cf_cycleTxId.ul_funId)
		{
			st_cf_sendFrame.ul_funId = st_cf_cycleTxId.ul_funId;
			st_cf_sendFrame.us_subId = st_cf_cycleTxId.us_subId;
			st_cf_sendFrame.uc_exId = 0;
			st_cf_cycleTxId.ul_funId =0;
			st_cf_cycleTxId.us_subId =0;
		}
	}
}

/**
 * @brief 发送CAN帧内容
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void Cf_CanTxData(void)
{
	unsigned short i=0;
	
	st_cf_sendFrame.COB_ID = st_cf_sendFrame.ul_funId;

	for(i=0; i<8; i++)
		st_cf_sendFrame.Data[i] = 0;

	if(st_cf_sendFrame.COB_ID==0x100)
	{
		switch(st_cf_sendFrame.us_subId)
		{
			case 0x60:
			{
				st_cf_sendFrame.COB_ID = 0;
				st_cf_sendFrame.DLC = 8;
				break;			
			}
			default:
			{
				st_cf_sendFrame.COB_ID = 0;
				break;
			}
		}
	}      

	if(st_cf_sendFrame.COB_ID)
	{
		us_cf_needSend = 1;
	}
}

/**
 * @brief 事件帧队列插入处理
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void Cf_InsertReqList(unsigned short us_index,unsigned long ul_funId,unsigned short us_subId,unsigned char uc_exId,unsigned short us_tickTotal,unsigned short us_timeLimit)
{
	if((st_cf_insertReq[us_index].ul_funId == ul_funId) && (st_cf_insertReq[us_index].us_subId == us_subId))
	{
		st_cf_insertReq[us_index].us_tickLeft = us_tickTotal;
	}
	else
	{
		st_cf_insertReq[us_index].ul_funId = ul_funId;
		st_cf_insertReq[us_index].us_subId = us_subId;
    	st_cf_insertReq[us_index].uc_exId = uc_exId;
		st_cf_insertReq[us_index].us_tickLeft = us_tickTotal;
		st_cf_insertReq[us_index].us_timeCur = 0;
		st_cf_insertReq[us_index].us_timeLimit = us_timeLimit;
		st_cf_insertReq[us_index].us_waitPercent = 0;
	}
}

/**
 * @brief CAN模块的超时复位处理
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-28
 * @copyright Copyright (c) 2024
 */
void Cf_CanResetDeal(void)
{







}

