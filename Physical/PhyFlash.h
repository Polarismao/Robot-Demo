
#define FLASH_KEY1          0x45670123
#define FLASH_KEY2          0xCDEF89AB
#define FLASH_TIMER20S	    20000

#define FLASH_ERASE_OK		0
#define FLASH_ERASE_DOING	1
#define FLASH_ERASE_ERROR	2

#define FLASH_WRITE_OK	    0
#define FLASH_WRITE_DOING	1
#define FLASH_WRITE_ERROR	2

#define FLASH_READ_OK		0
#define FLASH_READ_DOING    1
#define FLASH_READ_ERROR	2

#define FLASH_RDPROTECT_OK		    0
#define FLASH_RDPROTECT_DOING		1
#define FLASH_RDPROTECT_ERROR		2

#define FLASH_PAGE_USEMAX           (FLASH_DATA_NUM*2)
#define FLASH_DATA_NUM              1024
#define FLASH_PAGE_MAX	            256
#define FLASH_NULL		            0

const FLASHATTRIBUTE FLASH_LAYOUT_TAB[]=
{ //page start  end   size
	{0,	255,		2048}
};

typedef struct 
{
	unsigned short us_pageAddr;
	unsigned short us_dataAddr;
	unsigned short us_status;
}FLASH_WRITE_STATUS; 
FLASH_WRITE_STATUS st_phyFlash_writeStatus,st_phyFlash_readStatus;

typedef struct 
{
	unsigned short us_pageAddr;
	unsigned short us_dataAddr;
	unsigned short us_num;
	unsigned short us_data[FLASH_DATA_NUM];
}FLASH_RW_CODE;
FLASH_RW_CODE st_phyFlash_funcWrite, st_phyFlash_funcRead;	
FLASH_RW_CODE st_phyFlash_funcWriteP, st_phyFlash_funcReadP;


unsigned short us_phyFlash_eraseStep;
unsigned short us_phyFlash_erasePageNum;
unsigned short us_phyFlash_erasePageLegth;
unsigned short us_phyFlash_writePeriphStep;
unsigned short us_phyFlash_writeDataStep;
unsigned long  ul_phyFlash_writeAddr;
unsigned short us_phyFlash_writeData;
unsigned short us_phyFlash_writeNum;
unsigned short us_phyFlash_doingTimer;
unsigned short us_phyFlash_protectStep;
unsigned short us_phyFlash_protectTimer;
unsigned short us_phyFlash_readNum;
unsigned short us_phyFlash_readStep;
unsigned long  ul_phyFlash_readAddr;
unsigned short us_phyFlash_saveDataStep;
unsigned long  ul_phyFlash_saveAddr;
unsigned short us_phyFlash_saveData;
unsigned short us_phyFlash_saveNum;


void PhyFlash_ParaInit(void);
void PhyFlash_UnLock(void);
void PhyFlash_Lock(void);
void PhyFlash_WriteToPeriph(FLASH_RW_CODE *st_flashCode);
void PhyFlash_ReadFromPeriph(FLASH_RW_CODE *st_flashCode);
void PhyFlash_ClearSRFlag(unsigned short flag);
void PhyFlash_TimerOverDeal(void);
unsigned short PhyFlash_GetFlashStatus(void);
unsigned short PhyFlash_ErasePage(unsigned short us_pageAddr,unsigned short us_pageLength);
unsigned short PhyFlash_WriteData(FLASH_RW_CODE *st_flashCode);
unsigned short PhyFlash_ReadData(FLASH_RW_CODE *st_flashCode);



void PhyFlash_ReadProtect(unsigned short flag);
unsigned short PhyFlash_SaveData(FLASH_RW_CODE *st_flashCode);
