#ifndef _PHYIIC_H_
#define _PHYIIC_H_

#define I2C_Speed                 400000
#define I2C_OWN_ADDRESS7          0X0A   /* STM32 自身的 I2C 地址，这个地址只要与 STM32 外挂的 I2C 器件地址不一样即可 */
#define PCA9685_I2C1              I2C1
#define I2CT_FLAG_TIMEOUT         ((unsigned int)0x1000)
#define I2CT_LONG_TIMEOUT         ((unsigned int)(10 * I2CT_FLAG_TIMEOUT))

void I2C_Init(void);
void I2C_GPIO_Config(void);
void I2C_Mode_Config(void);
void I2C_ByteWrite(unsigned char uc_data, unsigned char uc_writeAddr);
unsigned char I2C_ByteRead(unsigned char uc_readAddr);
#endif

