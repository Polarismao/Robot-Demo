#include "ObjPublic.h"
#include "ObjCan.h"

/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-11-09
*@note 【备注】
*************************************************************/
void Cf_Can_ParaInit(void)
{


}

/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-11-09
*@note 【备注】
*************************************************************/
void Cf_CanMainDeal(void)
{
	unsigned short i,m,us_sendCycle;
    
	for(m=0;m<4;m++)
	{
		if(uc_phyCan_rxFlag[m])
		{
			st_cf_rcecFrame.COB_ID = ul_phyCan_rxId[m];
			st_cf_rcecFrame.uc_exId = uc_phyCan_exId[m];
			st_cf_rcecFrame.DLC = uc_phyCan_rxLen[m];
			for(i=0;i<st_cf_rcecFrame.DLC;i++)
			{
				st_cf_rcecFrame.Data[i] = uc_phyCan_rxMessage[m][i];
			}
			uc_phyCan_rxFlag[m] = 0;
		}
	}
	us_cf_canSendTimer += CF_2MS;
    us_sendCycle = CF_10MS;
    
	if(us_cf_canSendTimer>=us_sendCycle)
	{
		us_cf_canSendTimer=0;
		Cf_CanEvtChk();
		Cf_CanTxDeal();
		if(us_cf_needSend)
		{
			us_cf_needSend = 0;
			physical_can_deal(st_cf_sendFrame.COB_ID,st_cf_sendFrame.uc_exId,st_cf_sendFrame.DLC,st_cf_sendFrame.Data);
		}
	}
	Cf_CanResetDeal();
}

/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-11-09
*@note 【备注】
*************************************************************/
void Cf_CanResetDeal(void)
{
	//Cf_CopOnlineJudge();
}

/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-11-09
*@note 【备注】
*************************************************************/
void Cf_CanStdRxDeal(void)
{
	//Cf_FdMainCopRxDeal();		
}

/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-11-09
*@note 【备注】
*************************************************************/
void Cf_CanEvtChk(void)
{
   //FunMpc1_EvtChkMpc1Gen1Eid();
}

/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-11-12
*@note 【备注】
*************************************************************/
void Cf_CanTxDeal(void)
{
	us_cf_stepToggle ^= 0x01;
	if(us_cf_stepToggle&0x01)
		Cf_TxIdCycle();
	
	Cf_EvtWaitTimeInc();
	Cf_TxIdJudge();
	Cf_CanTxData();
}

/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-11-10
*@note 【备注】
*************************************************************/
void Cf_TxIdCycle(void)	 //20ms进每个case的执行为200ms周期
{
	if(st_cf_cycleTxId.ul_funId)
		return;

	switch(us_cf_cycleStep)
	{
		case 0:
		{
			break;	
		}
		case 1:
		{
			break;				
		}
		case 2:
		{
			break;		
		}
		case 3:
		{
			break;		
		}
		case 4:	
		{	
			break;
		}
		case 5:
		{
			break;
		}
		case 6:
		{		
			break;
		}
		case 7:
		{
			break;
		}
		case 8:
		{
			break;
		}
		case 9:
		{	
			break;		
		}
		default:
			break;
	}
	us_cf_cycleStep++;
	if(us_cf_cycleStep>9)
		us_cf_cycleStep=0;
}

/*************************************************************
*@brief【判断所有事件的紧急程度】
*@author mdq
*@date   2023-11-12
*@note 【备注】
*************************************************************/
void Cf_EvtWaitTimeInc(void)
{
	unsigned short i;
	unsigned short us_waitPercentTmp = 0;

	us_cf_insertWaitLongest = 0xffff;
	
	for(i=0; i<CF_INSERT_MAX1; i++ )
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

/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-11-12
*@note 【备注】
*************************************************************/
void Cf_TxIdJudge(void)
{
	st_cf_sendFrame.COB_ID = 0;
	st_cf_sendFrame.ul_funId = 0;
	st_cf_sendFrame.us_subId = 0;
  	st_cf_sendFrame.uc_exId = 0;    //V20.12 初始化默认标准帧

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
			if((st_cf_sendFrame.ul_funId==0x1fff)||(st_cf_sendFrame.ul_funId==0x5fff))   //A02.3-CR028
				st_cf_sendFrame.uc_exId = 1;
			else
				st_cf_sendFrame.uc_exId = 0;    //其余均为标准帧
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

/*************************************************************
*@brief【发送CAN帧内容】
*@author mdq
*@date   2023-11-09
*@note 【备注】
*************************************************************/
void Cf_CanTxData(void)
{
	unsigned short i;
	
	st_cf_sendFrame.COB_ID = st_cf_sendFrame.ul_funId;
	for(i=0; i<8; i++)
		st_cf_sendFrame.Data[i] = 0;

	if(st_cf_sendFrame.COB_ID==0x100)
	{
		switch(st_cf_sendFrame.us_subId)
		{
			case 0x60:
			{
				st_cf_sendFrame.COB_ID=0;
				st_cf_sendFrame.DLC = 8;
				break;			
			}
			default:
			{
				st_cf_sendFrame.COB_ID=0;
				break;
			}
		}
	}      
	else if(st_cf_sendFrame.COB_ID==0x417)
	{
		if(st_cf_sendFrame.us_subId==0xE4)
		{
			//Cf_PackFrm417IdeE4(st_cf_sendFrame.Data);
			st_cf_sendFrame.DLC = 8;
		}
	}
	if(st_cf_sendFrame.COB_ID)
	{
		us_cf_needSend = 1;
	}
}

/* void Cf_PackFrm100Ide60Fdoor(unsigned char * puc_data)
{
	static unsigned char uc_dataBak = 0;
	
	puc_data[0] = ((LIFT_NUM_CAN&0xff)<<4)+0x05;
	puc_data[1] = 0;
	puc_data[2] = 0x60; 
	puc_data[3] = 0xFF;
	puc_data[4] = cfDoor[0].cop_out_lamp[0];
	puc_data[5] = 0x01;
	puc_data[6] = 0;
	puc_data[7] = 0;
	
	Cf_TxMaskDeal(&uc_dataBak,&puc_data[3],&puc_data[4]);
} */


/*************************************************************
*@brief【描述】
*@param  us_index    【参数注释】
*@param  ul_funId    【参数注释】
*@param  us_subId    【参数注释】
*@param  uc_exId     【参数注释】
*@param  us_tickTotal【参数注释】
*@param  us_timeLimit【参数注释】
*@param  us_doorType 【参数注释】
*@author mdq
*@date   2023-11-12
*@note 【备注】
*************************************************************/
void Cf_InsertReqList(unsigned short us_index,unsigned long ul_funId,unsigned short us_subId,unsigned char uc_exId,unsigned short us_tickTotal,unsigned short us_timeLimit,unsigned short us_doorType)
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

