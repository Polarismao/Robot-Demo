#include "Physical_Flash.h"


/*************************************************
 * @brief Flash模块初始化
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
void PhyFlash_ParaInit(void)
{
	us_phyFlash_readNum = 0;
	st_phyFlash_writeStatus.us_status = FLASH_WRITE_OK;
	st_phyFlash_readStatus.us_status = FLASH_READ_OK;
}


/*************************************************
 * @brief Flash解除锁定
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
void PhyFlash_UnLock(void)
{
	if(FLASH->CR & FLASH_CR_LOCK)
	{  
	    FLASH->KEYR = FLASH_KEY1;  
	    FLASH->KEYR = FLASH_KEY2;  
	}  	
}


/*************************************************
 * @brief Flash获取锁定
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
void PhyFlash_Lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK; 
}


/*************************************************
 * @brief 获取Flash的状态
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
unsigned short PhyFlash_GetFlashStatus(void)
{
	unsigned short us_flashTemp;
	
	if(FLASH->SR & FLASH_SR_BSY)
		us_flashTemp = FLASH_SR_BSY;
	else if(FLASH->SR & FLASH_SR_PGERR)
		us_flashTemp = FLASH_SR_PGERR;
	else if(FLASH->SR & FLASH_SR_WRPRTERR)
		us_flashTemp = FLASH_SR_WRPRTERR;
	else 
		us_flashTemp = FLASH_NULL;

	return us_flashTemp;
}


/*************************************************
 * @brief 计算某个地址所在页的起始地址
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
unsigned long PhyFlash_PageAddrCalcul(unsigned short us_pageAddr)
{
	unsigned short i;
	unsigned long ul_addrTemp;
	unsigned short us_pageNumTemp;
	
	i = 0;
	ul_addrTemp = 0x08000000;
	us_pageNumTemp = FLASH_LAYOUT_TAB[i].us_startPage;
	
	while(us_pageNumTemp != us_pageAddr)
	{
		ul_addrTemp = ul_addrTemp + FLASH_LAYOUT_TAB[i].us_pageSize;
		us_pageNumTemp++;
		if(us_pageNumTemp > FLASH_LAYOUT_TAB[i].us_endPage)
			i++;
	}
	return ul_addrTemp;	
}


/*************************************************
 * @brief 擦除Flash某一页
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
unsigned short PhyFlash_ErasePage(unsigned short us_pageAddr,unsigned short us_pageLength) 
{
	unsigned long ul_pageAddrTemp;

	if(us_pageLength==0)
		return FLASH_ERASE_ERROR;

	if((us_pageAddr>=FLASH_PAGE_MAX)&&(us_pageAddr!=0xffff))
		return FLASH_ERASE_ERROR;
	
	PhyFlash_UnLock();
		
	if((us_pageAddr==0xffff)&&(us_pageLength==0xffff)) //erase all
	{
		switch(us_phyFlash_eraseStep)
		{
			case 0:
			{
				if(PhyFlash_GetFlashStatus() == FLASH_NULL)
				{
					FLASH->CR |= FLASH_CR_MER;             
					FLASH->CR |= FLASH_CR_STRT;                    
					us_phyFlash_eraseStep = 1;
				}
				break;
			}
			case 1:
			{
				if((PhyFlash_GetFlashStatus()==FLASH_NULL) && (FLASH->SR&FLASH_SR_EOP))
				{
					PhyFlash_ClearSRFlag(FLASH_SR_EOP);
					us_phyFlash_eraseStep = 2;
				}
				break;
			}
			case 2:
			{
				FLASH->CR &= ~FLASH_CR_MER;
				FLASH->CR &= ~FLASH_CR_STRT;
				us_phyFlash_eraseStep = 0;
				PhyFlash_Lock();
				return FLASH_ERASE_OK;
			}
			default:
				us_phyFlash_eraseStep = 0;
				break;
		}	
	}
	else
	{
		switch(us_phyFlash_eraseStep)
		{
			case 0:
			{
				us_phyFlash_erasePageNum = us_pageAddr;
				us_phyFlash_eraseStep = 1;
				break;
			}
			case 1:
			{
				if(PhyFlash_GetFlashStatus() == FLASH_NULL)
				{
					FLASH->CR |= FLASH_CR_PER;
					ul_pageAddrTemp = PhyFlash_PageAddrCalcul(us_phyFlash_erasePageNum);
					FLASH->AR = ul_pageAddrTemp;
					FLASH->CR |= FLASH_CR_STRT;
					us_phyFlash_eraseStep = 2;
				}
				break;
			}
			case 2:
			{
				if((PhyFlash_GetFlashStatus()==FLASH_NULL)&&(FLASH->SR&FLASH_SR_EOP))
				{
					PhyFlash_ClearSRFlag(FLASH_SR_EOP);
					us_phyFlash_erasePageNum++;
					if(us_phyFlash_erasePageNum>=FLASH_PAGE_MAX)
						us_phyFlash_eraseStep = 3;
					else if(us_phyFlash_erasePageNum>=(us_pageAddr+us_pageLength))
						us_phyFlash_eraseStep = 3;
					else
						us_phyFlash_eraseStep = 1;
				}
				break;
			}
			case 3:
			{
				FLASH->CR &= ~FLASH_CR_PER;
				FLASH->CR &= ~FLASH_CR_STRT;
				us_phyFlash_eraseStep = 0;
				us_phyFlash_erasePageNum = 0;
				PhyFlash_Lock();
				return FLASH_ERASE_OK;
			}
			default:
				us_phyFlash_eraseStep = 0;
				break;
		}
	}
	return FLASH_ERASE_DOING;
}


/*************************************************
 * @brief Flash写入数据
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
unsigned short PhyFlash_WriteData(FLASH_RW_CODE *st_flashCode)
{
	unsigned long ul_addrTemp=0;
	unsigned short us_dataTemp=0;

	if(st_flashCode->us_num == 0)
		return FLASH_WRITE_ERROR;
	if((st_flashCode->us_num*2) >FLASH_PAGE_USEMAX)
		return FLASH_WRITE_ERROR;
	
	ul_addrTemp = st_flashCode->us_dataAddr + (st_flashCode->us_num*2);

	if(ul_addrTemp>FLASH_PAGE_USEMAX)
		return FLASH_WRITE_ERROR;
	if((st_flashCode->us_dataAddr%2)!=0)
		return FLASH_WRITE_ERROR;

	PhyFlash_UnLock();

	switch(us_phyFlash_writeDataStep)
	{
		case 0:
		{
			ul_phyFlash_writeAddr = PhyFlash_PageAddrCalcul(st_flashCode->us_pageAddr);
			ul_phyFlash_writeAddr += st_flashCode->us_dataAddr;
			us_phyFlash_writeNum = 0;
			us_phyFlash_writeData = st_flashCode->us_data[us_phyFlash_writeNum];
			PhyFlash_ClearSRFlag(FLASH_SR_EOP);
			us_phyFlash_writeDataStep  = 1;
			break;
		}
		case 1:
		{
			if(PhyFlash_GetFlashStatus()==FLASH_NULL)
			{
				FLASH->CR |= FLASH_CR_PG; 
				us_phyFlash_writeData = st_flashCode->us_data[us_phyFlash_writeNum];
				*(unsigned short*)ul_phyFlash_writeAddr = us_phyFlash_writeData;
				us_phyFlash_writeDataStep = 2;
			} 
			else 
				PhyFlash_ClearSRFlag(FLASH_SR_PGERR|FLASH_SR_WRPRTERR|FLASH_SR_EOP);   
			break;
		}
		case 2:
		{
			if((PhyFlash_GetFlashStatus()==FLASH_NULL)&&(FLASH->SR&FLASH_SR_EOP))
			{
				PhyFlash_ClearSRFlag(FLASH_SR_EOP);
				
				us_dataTemp = *(unsigned short*)ul_phyFlash_writeAddr;
				if(us_dataTemp==us_phyFlash_writeData)
				{
					ul_phyFlash_writeAddr=ul_phyFlash_writeAddr+2;
					us_phyFlash_writeNum++;
					if(us_phyFlash_writeNum<st_flashCode->us_num)
					{
						us_phyFlash_writeDataStep = 1;	
					}
					else
					{
						us_phyFlash_writeDataStep = 3;
					}
				}
				else
					us_phyFlash_writeDataStep = 1;	
			}
			break;
		}
		case 3:
		{
			FLASH->CR &= ~FLASH_CR_PG;
			us_phyFlash_writeNum = 0;
			ul_phyFlash_writeAddr = 0;
			us_phyFlash_writeDataStep = 0;
			PhyFlash_Lock();
			return FLASH_WRITE_OK;
		}
		default:
			us_phyFlash_writeDataStep = 0;
			break;
	}
	return FLASH_WRITE_DOING;
}


/*************************************************
 * @brief Flash读取数据
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
unsigned short PhyFlash_ReadData(FLASH_RW_CODE *st_flashCode)
{
	unsigned short i=0;
	
	if(st_flashCode->us_num == 0)
		return FLASH_READ_ERROR;
	if((st_flashCode->us_num*2) >FLASH_PAGE_USEMAX)
		return FLASH_READ_ERROR;
	if((st_flashCode->us_dataAddr%2)!=0)
		return FLASH_READ_ERROR;

	switch(us_phyFlash_readStep)
	{
		case 0: 
		{
			us_phyFlash_readNum = 0;
			ul_phyFlash_readAddr = PhyFlash_PageAddrCalcul(st_flashCode->us_pageAddr);
			ul_phyFlash_readAddr += st_flashCode->us_dataAddr;
			us_phyFlash_readStep = 1;
			break;	
		}
		case 1:
		{	
			for(i=us_phyFlash_readNum; i<(us_phyFlash_readNum+32); i++) 
			{
				if(i>=st_flashCode->us_num)
				{
					us_phyFlash_readNum = 0;
					us_phyFlash_readStep = 0;
					return FLASH_READ_OK;
				}
				else
				{
					st_flashCode->us_data[i] = *(unsigned short*)ul_phyFlash_readAddr;
					ul_phyFlash_readAddr += 2;
				}
			}
			us_phyFlash_readNum += 32;
			break;
		}
		default:
			us_phyFlash_readStep = 0;
			break;
	}			
	return FLASH_READ_DOING;
}	


/*************************************************
 * @brief Flash写入数据
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void PhyFlash_WriteToPeriph(FLASH_RW_CODE *st_flashCode)
{
	unsigned short i;
	unsigned short us_pageAdrTemp;
	unsigned short us_statusTemp;
	
	if(st_phyFlash_writeStatus.us_status != FLASH_WRITE_DOING)
	{
		st_phyFlash_funcWriteP.us_pageAddr = st_flashCode->us_pageAddr;
		st_phyFlash_funcWriteP.us_dataAddr = st_flashCode->us_dataAddr;
		st_phyFlash_funcWriteP.us_num = st_flashCode->us_num;
		for(i=0;i<st_flashCode->us_num;i++)
		{
			st_phyFlash_funcWriteP.us_data[i] = st_flashCode->us_data[i];	
		}
	
		st_phyFlash_writeStatus.us_status = FLASH_WRITE_DOING;
		st_phyFlash_writeStatus.us_pageAddr = st_phyFlash_funcWriteP.us_pageAddr;
		st_phyFlash_writeStatus.us_dataAddr = st_phyFlash_funcWriteP.us_dataAddr;
		us_phyFlash_writePeriphStep = 0;
	}
	else if(st_phyFlash_writeStatus.us_status == FLASH_WRITE_DOING)
	{	
		if((st_flashCode->us_pageAddr != st_phyFlash_writeStatus.us_pageAddr)&&\
		   (st_flashCode->us_dataAddr != st_phyFlash_writeStatus.us_dataAddr))
			return;

		switch(us_phyFlash_writePeriphStep)
		{
			case 0:
			{
				st_phyFlash_funcReadP.us_pageAddr = st_phyFlash_funcWriteP.us_pageAddr;
				st_phyFlash_funcReadP.us_dataAddr = 0;
				st_phyFlash_funcReadP.us_num = FLASH_PAGE_USEMAX/2;
				
				PhyFlash_ReadFromPeriph(&st_phyFlash_funcReadP);
				
				if((st_phyFlash_readStatus.us_status==FLASH_READ_OK)&&(st_phyFlash_readStatus.us_dataAddr == st_phyFlash_funcReadP.us_dataAddr)&&(st_phyFlash_readStatus.us_pageAddr ==st_phyFlash_funcReadP.us_pageAddr))
				{
					us_phyFlash_writePeriphStep = 1;
				}
				break;
			}
			case 1:
			{
				us_pageAdrTemp = st_phyFlash_funcWriteP.us_pageAddr;
				
				us_statusTemp = PhyFlash_ErasePage(us_pageAdrTemp,1);
				if(us_statusTemp == FLASH_ERASE_OK)
				{
					us_phyFlash_writePeriphStep = 2;
				}
				break;
			}
			case 2:
			{
				st_phyFlash_funcWrite = st_phyFlash_funcReadP;
				us_pageAdrTemp = st_phyFlash_funcWriteP.us_dataAddr;
				us_pageAdrTemp = us_pageAdrTemp/2;
				
				for(i=0;i<st_phyFlash_funcWriteP.us_num;i++)
				{
					st_phyFlash_funcWrite.us_data[i+us_pageAdrTemp] = st_phyFlash_funcWriteP.us_data[i];
				}
				st_phyFlash_funcWrite.us_dataAddr =0;
				st_phyFlash_funcWrite.us_num = FLASH_PAGE_USEMAX/2;
				us_phyFlash_writePeriphStep = 3;
				break;
			}
			case 3:
			{
				us_statusTemp = PhyFlash_WriteData(&st_phyFlash_funcWrite);
				
				if(us_statusTemp ==FLASH_WRITE_OK)
				{	
					us_phyFlash_writePeriphStep = 0;
					st_phyFlash_writeStatus.us_status = FLASH_WRITE_OK;
					return;
				}
				break;
			}
			default:
			{
				us_phyFlash_writePeriphStep = 0;
				st_phyFlash_writeStatus.us_status = FLASH_WRITE_OK;
				break;
			}
		}
	}
	return;
}


/*************************************************
 * @brief Flash读取数据
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void PhyFlash_ReadFromPeriph(FLASH_RW_CODE *st_flashCode)
{
	if(st_phyFlash_readStatus.us_status != FLASH_READ_DOING)
	{
		st_phyFlash_readStatus.us_status = FLASH_READ_DOING;
		st_phyFlash_readStatus.us_pageAddr =  st_flashCode->us_pageAddr;
		st_phyFlash_readStatus.us_dataAddr =  st_flashCode->us_dataAddr;
	}
	else if(st_phyFlash_readStatus.us_status == FLASH_READ_DOING)
	{
		if((st_flashCode->us_pageAddr !=st_phyFlash_readStatus.us_pageAddr)&&\
		   (st_flashCode->us_dataAddr !=st_phyFlash_readStatus.us_dataAddr))
			return;
				
		if(PhyFlash_ReadData(st_flashCode)==FLASH_READ_OK)
		{
			st_phyFlash_readStatus.us_status = FLASH_READ_OK;
		}	
	}
}


/*************************************************
 * @brief Flash-SR寄存器清除
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void PhyFlash_ClearSRFlag(unsigned short us_flag)
{
	if(us_flag & FLASH_SR_PGERR)
		FLASH->SR |= FLASH_SR_PGERR;
	if(us_flag & FLASH_SR_WRPRTERR)
		FLASH->SR |= FLASH_SR_WRPRTERR;
	if(us_flag & FLASH_SR_EOP)
		FLASH->SR |= FLASH_SR_EOP;
}

/*************************************************
 * @brief 写入读取超时处理
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void PhyFlash_TimerOverDeal(void)
{
	 if((st_phyFlash_readStatus.us_status==FLASH_READ_DOING)||(st_phyFlash_writeStatus.us_status==FLASH_WRITE_DOING))
	 {
	 	us_phyFlash_doingTimer += 2;
	 	if(us_phyFlash_doingTimer>FLASH_TIMER20S) 
	 	{
	 		us_phyFlash_doingTimer = 0;
	 		error_code1_in(25,103);
	 		PhyFlash_ClearSRFlag(FLASH_SR_PGERR|FLASH_SR_WRPRTERR|FLASH_SR_EOP);
	 		st_phyFlash_writeStatus.us_status = FLASH_WRITE_OK;
	 		st_phyFlash_readStatus.us_status = FLASH_READ_OK;
	 		PhyFlash_Lock();
	 	}
	}
	else
		us_phyFlash_doingTimer = 0;
}


/*************************************************
 * @brief 配置Flash读保护
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void PhyFlash_ReadProtect(unsigned short flag)
{
	if(flag == READ_DISABLE)
	{
		if(FLASH->OBR & FLASH_OBR_RDPRT)
			return;
	}
	else if(flag == READ_ENABLE)
	{
		if((FLASH->OBR & FLASH_OBR_RDPRT)==0)
			return;
	}
		
	PhyFlash_UnLock();

	switch(us_phyFlash_protectStep)
	{
		case 0:
		{
			FLASH->OPTKEYR = FLASH_KEY1;
			FLASH->OPTKEYR = FLASH_KEY2;
			FLASH->CR |= FLASH_CR_OPTER;
			FLASH->CR |= FLASH_CR_STRT;
			if(PhyFlash_GetFlashStatus()==FLASH_NULL)
			{
				us_phyFlash_protectStep = 1;
			}
			break;	
		}
		case 1:
		{
			FLASH->CR &= ~FLASH_CR_OPTER;
			FLASH->CR |= FLASH_CR_OPTPG;
			if(flag == READ_ENABLE)
				OB->RDP = 0xA5;
			else if(flag == READ_DISABLE)
			 	OB->RDP = 0x00;
			if(PhyFlash_GetFlashStatus()==FLASH_NULL)
				us_phyFlash_protectStep = 2;
			break;
		}
		case 2:
		{
			us_phyFlash_protectStep = 0;
			FLASH->CR &= ~FLASH_CR_OPTPG;
			FLASH->CR &= ~FLASH_CR_STRT;
			PhyFlash_Lock();
			break;
		}
		default:
			us_phyFlash_protectStep = 0;
			break;
	}
}


unsigned short PhyFlash_SaveData(FLASH_RW_CODE *st_flashCode)
{
	unsigned short us_dataTemp;
	
	if(st_flashCode->us_num == 0)
	{
		return FLASH_WRITE_ERROR;
	}
	if((st_flashCode->us_dataAddr%2)!=0)
	{
		return FLASH_WRITE_ERROR;		
	}

	
	PhyFlash_UnLock();
	switch(us_phyFlash_saveDataStep)
	{
		case 0:
		{
			ul_phyFlash_saveAddr = PhyFlash_PageAddrCalcul(st_flashCode->us_pageAddr);
			ul_phyFlash_saveAddr += st_flashCode->us_dataAddr;
			us_phyFlash_saveNum = 0;
			us_phyFlash_saveData = st_flashCode->us_data[us_phyFlash_saveNum];
			PhyFlash_ClearSRFlag(FLASH_SR_EOP);
			us_phyFlash_saveDataStep  = 1;
			break;
		}
		case 1:
		{
			if(PhyFlash_GetFlashStatus()==FLASH_NULL)
			{
				FLASH->CR|=FLASH_CR_PG; 
				us_phyFlash_saveData = st_flashCode->us_data[us_phyFlash_saveNum];
				*(unsigned short*)ul_phyFlash_saveAddr = us_phyFlash_saveData;
				us_phyFlash_saveDataStep = 2;
			} 
			else 
				PhyFlash_ClearSRFlag(FLASH_SR_PGERR|FLASH_SR_WRPRTERR|FLASH_SR_EOP);   
			break;
		}
		case 2:
		{
			if((PhyFlash_GetFlashStatus()==FLASH_NULL)&&(FLASH->SR&FLASH_SR_EOP))
			{
				PhyFlash_ClearSRFlag(FLASH_SR_EOP);
				
				us_dataTemp = *(unsigned short*)ul_phyFlash_saveAddr;
				if(us_dataTemp==us_phyFlash_saveData)
				{
					ul_phyFlash_saveAddr=ul_phyFlash_saveAddr+2;
					us_phyFlash_saveNum++;
					if(us_phyFlash_saveNum<st_flashCode->us_num)
					{
						us_phyFlash_saveDataStep = 1;	
					}
					else
					{
						us_phyFlash_saveDataStep = 3;
					}
				}
				else
					us_phyFlash_saveDataStep = 1;	
			}
			break;
		}
		case 3:
		{
			FLASH->CR &= ~FLASH_CR_PG;
			us_phyFlash_saveNum = 0;
			ul_phyFlash_saveAddr = 0;
			us_phyFlash_saveDataStep = 0;
			PhyFlash_Lock();
			return FLASH_WRITE_OK;
		}
		default:
			us_phyFlash_saveDataStep = 0;
			break;
	}
	return FLASH_WRITE_DOING;
}


