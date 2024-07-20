
static BOOL b_obj232_moduleActive = true;

/**
 * @brief 232模块初始化
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-20
 * @copyright Copyright (c) 2024
 */
void Obj232_ParaInit(void)
{


}

/**
 * @brief 
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-20
 * @copyright Copyright (c) 2024
 */
void Obj232_ModuleStatus(BOOL b_ActStatus)
{
    b_obj232_moduleActive = b_ActStatus;
}

/**
 * @brief 232模块主逻辑
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-20
 * @copyright Copyright (c) 2024
 */
void Obj232_MainDeal(void)
{



}

/**
 * @brief 
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-07-20
 * @copyright Copyright (c) 2024
 */
unsigned short Obj232_CrcCal(unsigned char *data_value,unsigned char length)
{
	unsigned short crc_value=0xFFFF;
	unsigned short i=0,ia=0;
	if(function.f_code.password_func_select&0x4000)
	{
		crc_value=0xf5f5;
		ia=scpu_hbi_code*0x0f1f;
		crc_value^=ia;		
		while(length--)
		{
			crc_value ^= *data_value++;
		}
	}
	else
	{
		while(length--)
		{
			crc_value^=*data_value++;
			for(i=0;i<8;i++)
			{
				if(crc_value&0x0001)
				{
					crc_value=(crc_value>>1)^0xa001;
				}
				else
				{
					crc_value=crc_value>>1;
				}
			}
		}
	}
	return crc_value;
}



