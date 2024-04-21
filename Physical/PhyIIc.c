#include "stm32f10x.h"
//#include "PhyPublic.h"
#include "PhyIIc.h"
#include "ObjPCA9685.h"

/*************************************************************
*@brief【IIC外设初始化】
*@author mdq
*@date   2023-09-10
*@note 【备注】
*************************************************************/
void I2C_Init(void)
{
    I2C_GPIO_Config();
    I2C_Mode_Config();
}

/*************************************************************
*@brief【IIC外设的GPIO引脚配置】
*@author mdq
*@date   2023-09-10
*@note 【备注】
*************************************************************/
void I2C_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 配置 SCL、SDA 引脚 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;      //开漏输出    
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;      //开漏输出    
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*************************************************************
*@brief【IIC外设的模式配置】
*@author mdq
*@date   2023-09-10
*@note 【备注】
*************************************************************/
void I2C_Mode_Config(void)
{
    I2C_InitTypeDef I2C_InitStructure;
    /*I2C 模式 */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    /* 占空比 */
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    /*I2C 自身地址 */
    I2C_InitStructure.I2C_OwnAddress1 = I2C_OWN_ADDRESS7;
    /* 使能响应 */
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    /* I2C 的寻址模式 */
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    /* 通信速率 */
    I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
    /* 写入配置 */
    I2C_Init(PCA9685_I2C1, &I2C_InitStructure);
    /* 使能 I2C */
    I2C_Cmd(PCA9685_I2C1, ENABLE);
}

/*************************************************************
*@brief【通过IIC写参数】
*@param  uc_data     【被写的参数】
*@param  uc_writeAddr【写入的地址】
*@author mdq
*@date   2023-09-10
*@note 【备注】
*************************************************************/
void I2C_ByteWrite(unsigned char uc_data, unsigned char uc_writeAddr)
{
    static unsigned int I2CTimeout = I2CT_LONG_TIMEOUT;
    
    /* 产生 I2C 起始信号 */
    I2C_GenerateSTART(PCA9685_I2C1, ENABLE);

    /* 设置超时等待时间 */
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV5 事件并清除标志 */
    while (!I2C_CheckEvent(PCA9685_I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)   
        {
            return;
        }
    }

    /* 发送 PCA9685 设备地址 */
    I2C_Send7bitAddress(PCA9685_I2C1, PCA9685_ADDR, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV6 事件并清除标志 */
    while (!I2C_CheckEvent(PCA9685_I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
        {
            return;
        }
    }

    /* 发送要写入的 PCA9685 内部地址 (即 PCA9685 内部存储器的地址) */
    I2C_SendData(PCA9685_I2C1, uc_writeAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(PCA9685_I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
        {
            return;
        }
    }
    /* 发送一字节要写入的数据 */
    I2C_SendData(PCA9685_I2C1, uc_data);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(PCA9685_I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
        {
            return;
        }
    }

    /* 发送停止信号 */
    I2C_GenerateSTOP(PCA9685_I2C1, ENABLE);
	return;
}

/*************************************************************
*@brief【通过IIC读取字节】
*@param[in/out] uc_readAddr【参数注释】
*@return unsigned char【返回值注释】
*@author mdq
*@date   2023-09-09
*@note 【备注】
*************************************************************/
unsigned char I2C_ByteRead(unsigned char uc_readAddr)
{
	static unsigned int I2CTimeout = I2CT_LONG_TIMEOUT;
	unsigned char uc_data = 0;

    while (I2C_GetFlagStatus(PCA9685_I2C1, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
        {
            return 0;
        }
    }

    /* 产生 I2C 起始信号 */
    I2C_GenerateSTART(PCA9685_I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* 检测 EV5 事件并清除标志 */
    while (!I2C_CheckEvent(PCA9685_I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
        {
            return 0;
        }
    }

    /* 发送 PCA9685 设备地址 */
    I2C_Send7bitAddress(PCA9685_I2C1, PCA9685_ADDR, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* 检测 EV6 事件并清除标志 */
    while (!I2C_CheckEvent(PCA9685_I2C1 ,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
        {
            return 0;
        }
    }
    /* 通过重新设置 PE 位清除 EV6 事件 */
    I2C_Cmd(PCA9685_I2C1, ENABLE);

    /* 发送要读取的 PCA9685 内部地址 (即 PCA9685 内部存储器的地址) */
    I2C_SendData(PCA9685_I2C1, uc_readAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(PCA9685_I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
        {
            return 0;
        }
    }
    /* 产生第二次 I2C 起始信号 */
    I2C_GenerateSTART(PCA9685_I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* 检测 EV5 事件并清除标志 */
    while (!I2C_CheckEvent(PCA9685_I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
        {
            return 0;
        }
    }
    /* 发送 PCA9685 设备地址 */
    I2C_Send7bitAddress(PCA9685_I2C1, PCA9685_ADDR,I2C_Direction_Receiver);
    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* 检测 EV6 事件并清除标志 */

    while (!I2C_CheckEvent(PCA9685_I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
        {
            return 0;
        }
    }

    /* 发送非应答信号 */
    I2C_AcknowledgeConfig(PCA9685_I2C1, DISABLE);
    /* 发送停止信号 */
    I2C_GenerateSTOP(PCA9685_I2C1, ENABLE);

    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (I2C_CheckEvent(PCA9685_I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)
    {
        if ((I2CTimeout--) == 0)
        {
            return 0;
        }
    }

    /* 通过 I2C，从设备中读取一个字节的数据 */
    uc_data = I2C_ReceiveData(PCA9685_I2C1);

    /* 使能应答，方便下一次 I2C 传输 */
    I2C_AcknowledgeConfig(PCA9685_I2C1, ENABLE);
    return uc_data;
}

