
static BOOL b_obj232_moduleActive = true;

void Obj232_ModuleStatus(BOOL b_ActStatus)
{
    b_obj232_moduleActive = b_ActStatus;
}

void Obj232_ParaInit(void)
{



}

void obj232_deal(void)
{
	static unsigned char  uc_mnkSendTab[DIAGONSTIC_DATA_BUFF_LEN]= {0};    //
	static unsigned char  uc_mnkReceTab[DIAGONSTIC_DATA_BUFF_LEN]= {0};    // mnk协议数据帧,static因为除了解析还存在保存AES的关系
	static unsigned short us_mnkProtelLen = 0; 							   // mnk协议的长度
		   unsigned char  uc_232RecvData[DIAGONSTIC_DATA_BUFF_LEN] = {0};  // 接收数据缓冲区，将二维数组转移到一位数组进行传递
		   unsigned char  uc_tkeSendTab[DIAGONSTIC_DATA_BUFF_LEN]= {0};	   //蒂森协议接收数据
		   unsigned char  uc_tkeReceTab[DIAGONSTIC_DATA_BUFF_LEN]= {0};
		   unsigned short us_tkeProtelLen = 0; // tke协议的长度
    	   unsigned short us_Addr = 0;
		   unsigned short i = 0;
		   unsigned short us_crcTmp = 0;
		   unsigned short us_ASIITmp = 0;
		   unsigned short us_index = 0;
	
	// 主板程序首先要确保2_1秘钥更新了发送配置信息才会有用，吴晓斌的蓝牙测试程序逻辑是这样，后续进行确认
	// 可能会进行修改
    #ifndef TDC_FUNCTION
        #ifdef SHDS_FB_BUILD
        if(b_obj232_moduleActive == false)
        {
            return;
        }
        #endif  
    #endif    
	sci_space_time+=10;
	if(sci_space_time>1000)
	{
		sci_space_time=1000;
	}
	sci_delay_time++;
	if(sci_delay_time>60000)
	{
		sci_delay_time=60000;
	}
	if(group_can_mode!=3)   //=3时并联占用uart1
	{
		//if((!function.f_code.commuerr_time)||((sci_delay_time/10)<=function.f_code.commuerr_time))
		{
			if(((UsartStatusFlag[0]==4) && (0 == (enable_flag&0x200000) )) &&(0 == (ul_ObjCarBtm_enableFlag & 0x200000)) ) //接收完成 并且当前不处于更新
			{
				UsartStatusFlag[0]=0;
				
				// 这里进行赋值到临时变量
				for(i = 0; i < uart_recev_ptr[0]; i++ )
				{
					uc_232RecvData[i] = uart_recev_data[0][i];
				}
				
				e_dataType = FROM_MC2;
                
                for( i = 0; i <DIAGONSTIC_DATA_BUFF_LEN; i++)
                {
                    uc_mnkReceTab[i] = 0;
                }
                
                us_mnkProtelLen = 0;
                
				// 由于二维数组的行地址叠加需要写成 a[0] +1 或者 *a +1;容易引起歧义，先进行单数组的变化，再进行赋值。
				// 建议后续可以把232的入口地址整体从二维数组改成四个一维数组或者把二维数组的行列定义变化
				object_232_rece_deal( &uc_232RecvData[0], uart_recev_ptr[0], 
									  &uc_tkeReceTab[0], &us_tkeProtelLen,
									  &uc_mnkReceTab[0], &us_mnkProtelLen,
									  &rs232_race_flag,
								      &enable_flag); //其中增加协议判断
			
				PhyUart_ITConfig(PHY_USART1_NUB,PHY_USART_IT_RXNE, 1);
				
				uart_recev_ptr[0]=0;
				sci_space_time = 0;
				sci_delay_time = 0;
			}
			
			// 进行秘钥更新动作
             #ifndef COMMON_BLUETOOTH
			if(enable_flag&0x200000) // 更新的秘钥
			{
				enable_flag = Obj232_UpdateAes21Data();
			}
            #endif
			
			if(OBJ_CAN_END_FRM_FINISH == us_CfCan_IapStatusFlag)//can总线烧录回复平板报文接收完成
			{
                if(
                    (CF_IAP_FROM_MC2C == un_cfIap_ctrl.bit.b_src) && 
                    (
                      ((uc_CfCan_IapRxBuf[3] >= CF_IAP_RESP_d) && (uc_CfCan_IapRxBuf[3] <= CF_IAP_RESP_e)) || 
                      ((uc_CfCan_IapRxBuf[3] >= CF_IAP_RESP_g) && (uc_CfCan_IapRxBuf[3] <= CF_IAP_RESP_i))
                    )&&
                    (
                      ((uc_CfCan_IapRxBuf[4] >= CF_IAP_VERS) && (uc_CfCan_IapRxBuf[4]<=CF_IAP_JUMP)) || 
                      ((CF_IAP_INFO ==uc_CfCan_IapRxBuf[4]) && (CF_IAP_RESP_NOK == uc_CfCan_IapRxBuf[5]))
                    )			        
                  )
                {
                    un_cfIap_ctrl.bit.b_jumpToIapLoopCmd = 0;     
                    #ifdef TDC_FUNCTION
					// TDC修改
					obj232_setSendData(uc_CfCan_IapRxBuf,uc_CfCan_IapRxLen );	
					#else                     
                    sci_uart_deal(0, 0, uc_CfCan_IapRxBuf, uc_CfCan_IapRxLen);
                    #endif
				    us_CfCan_IapStatusFlag = OBJ_CAN_END_FRM_WAITING;  
                }
			}
			
			// 进行发送相关的动作
			if(enable_flag&0x0010)
			{
				if(sci_space_time>=function.f_code.commu_delay)
				{
					sci_space_time = 0;
					sci_delay_time = 0;
					if(uc_obj232_mc2IapFlag)//主板或DSP在线升级
					{
					    if(uc_obj232_mc2IapFlag==CF_MC2C_IAP_MC2C)
					    {
						    Cf_IapMc2cSendDeal(uc_tkeReceTab, uc_tkeSendTab, &us_tkeProtelLen);
					    }
                        else if(uc_obj232_mc2IapFlag==CF_MC2C_IAP_DSP)
                        {
                            Cf_IapDspSendDeal(uc_tkeReceTab, uc_tkeSendTab, &us_tkeProtelLen);
                        }    
						uc_obj232_mc2IapFlag=0;
                        
                        if(us_tkeProtelLen > 0)
                        {
                            #ifdef TDC_FUNCTION
                            // TDC修改
                            obj232_setSendData(uc_tkeSendTab,(unsigned char)us_tkeProtelLen );	
                            #else  
						    sci_uart_deal(0,0,uc_tkeSendTab,(unsigned char)us_tkeProtelLen);
                            #endif
                        }
						us_tkeProtelLen=0;
						enable_flag=0;
					}
					else if(uc_objMcp2515_receStatus==RECE_END)//接收到末帧则发送
					{
			            if(
                             (CF_IAP_FROM_MC2C == un_cfIap_ctrl.bit.b_src) &&
                             (
                                ((uc_objMcp2515_canToUartData[3] >= CF_IAP_RESP_d) && (uc_objMcp2515_canToUartData[3] <= CF_IAP_RESP_e)) ||
                                ((uc_objMcp2515_canToUartData[3] >= CF_IAP_RESP_g) && (uc_objMcp2515_canToUartData[3] <= CF_IAP_RESP_i))
                             )&&
                             (
                                ((uc_objMcp2515_canToUartData[4] >= CF_IAP_VERS) && (uc_objMcp2515_canToUartData[4]<=CF_IAP_JUMP)) || 
                                ((CF_IAP_INFO == uc_objMcp2515_canToUartData[4]) && (CF_IAP_RESP_NOK == uc_objMcp2515_canToUartData[5]))
                             )
                          )
                        {
                            un_cfIap_ctrl.bit.b_jumpToIapLoopCmd = 0;                            
                            uc_objMcp2515_receStatus = RECE_ING;
                            #ifdef TDC_FUNCTION
                            // TDC修改
                            obj232_setSendData(uc_objMcp2515_canToUartData,uc_objMcp2515_canToUartLen );	
                            #else 
						    sci_uart_deal(0,0,uc_objMcp2515_canToUartData,uc_objMcp2515_canToUartLen);						    
						    #endif
                            enable_flag=0;
                        }
					}
					else
					{
						// 发送升级协议和诊断统一用mnk的变量,根据协议更新AES秘钥回复的是固定值
						object_232_send_deal(uc_mnkReceTab, &rs232_race_flag, &enable_flag, uc_mnkSendTab ,&us_mnkProtelLen);
						
						// AES的秘钥升级协议，senddeal处理完之后直接发送。MNK协议也直接发送。
						if( (OBJ232_PROTOCAL_MNK == uc_obj232_ProtocalFlag) || (OBJ232_PROTOCAL_DS_BT_AES_UPDATE == uc_obj232_ProtocalFlag) )
						{
							if(OBJ232_PROTOCAL_DS_BT_AES_UPDATE == uc_obj232_ProtocalFlag)
							{
								us_index = 1;
								uc_tkeSendTab[0] = 0x02;
								for( i = 1 ; i < us_mnkProtelLen-5;i++ )
								{
									if( uc_mnkSendTab[i] == 0x02)
									{
										uc_tkeSendTab[us_index] = 0x04;
										us_index++;
										uc_tkeSendTab[us_index] = 0x06;
										us_index++;
										
									}
									else if( uc_mnkSendTab[i] == 0x03)
									{				
										uc_tkeSendTab[us_index] = 0x04;
										us_index++;
										uc_tkeSendTab[us_index] = 0x07;
										us_index++;
									}
									else if( uc_mnkSendTab[i] == 0x04)
									{										
										uc_tkeSendTab[us_index] = 0x04;
										us_index++;
										uc_tkeSendTab[us_index] = 0x04;
										us_index++;
									}
									else
									{
										uc_tkeSendTab[us_index] = uc_mnkSendTab[i];
										us_index++;
									}
								}
								
								us_crcTmp = ObjCarBtm_DsUpdateAesCrc16Chk(uc_tkeSendTab+1, (us_index - 1)); // 只计算FF 'Y' '1' '0'
				
								us_ASIITmp = ObjCarBtm_HexToASCII((unsigned char)(us_crcTmp&0xff));    //CRC低位
								uc_tkeSendTab[us_index++] = (unsigned char)((us_ASIITmp>>8)&0xff);
								uc_tkeSendTab[us_index++] = (unsigned char)(us_ASIITmp&0xff);  

								us_ASIITmp = ObjCarBtm_HexToASCII((unsigned char)((us_crcTmp>>8)&0xff));  //CRC高位
								uc_tkeSendTab[us_index++] = (unsigned char)((us_ASIITmp>>8)&0xff);
								uc_tkeSendTab[us_index++] = (unsigned char)(us_ASIITmp&0xff);  
								
								uc_tkeSendTab[us_index] = 0x03;
								us_index++;
								#ifdef TDC_FUNCTION
                                // TDC修改
                                obj232_setSendData(uc_tkeSendTab,(unsigned char)us_index );	
                                #else 
								sci_uart_deal(0,0,uc_tkeSendTab,(unsigned char)us_index); //发送处理
                                #endif
							}
							else
							{
                                #ifdef TDC_FUNCTION
                                // TDC修改
                                obj232_setSendData(uc_mnkSendTab,(unsigned char)us_mnkProtelLen );	
                                #else 
								sci_uart_deal(0,0,uc_mnkSendTab,(unsigned char)us_mnkProtelLen); //发送处理
                                #endif
							}
						}
						else if((OBJ232_PROTOCAL_DS_DIAG_OK==uc_obj232_ProtocalFlag) || \
								(OBJ232_PROTOCAL_DS_DIAG_FAIL==uc_obj232_ProtocalFlag))
						{
							if(enable_flag != 0)
							{
								us_Addr = (uc_mnkReceTab[2]<<8) + uc_mnkReceTab[3];
							
								// 将mnk协议转化成tke发送协议
								us_tkeProtelLen = ObjCarBtm_mnkFrameToDs(us_Addr, uc_mnkSendTab,\
													us_mnkProtelLen, uc_tkeSendTab, OBJ_CARBTM_DS_SEND_BUFSIZE);
						
								// 后续mnk的数据帧无用了，因此把他作为临时变量赋值，在加密中，输入参数和输出参数不能是一个对象
								for( i = 0 ; i < us_tkeProtelLen; i++)
								{
									uc_mnkSendTab[i]	= uc_tkeSendTab[i];
								}		
							
								if(us_tkeProtelLen > 9)
								{
                                    #ifndef COMMON_BLUETOOTH    
									// 对诊断数据进行AES2.1加密
									Cf_BlueToothAesEncode(AES2_1, &uc_mnkSendTab[4], (us_tkeProtelLen-9), &uc_tkeSendTab[4]);
							
									if( ((us_tkeProtelLen -9) % 16) != 0 )
									{
										us_tkeProtelLen = 9 + 16 *( ((us_tkeProtelLen -9) / 16)+1 );
									}
									
									us_index = 1;
									uc_mnkSendTab[0] = 0x02;
									for( i = 1 ; i < us_tkeProtelLen-5;i++ )
									{
										if( uc_tkeSendTab[i] == 0x02)
										{
											uc_mnkSendTab[us_index] = 0x04;
											us_index++;
											uc_mnkSendTab[us_index] = 0x06;
											us_index++;
										
										}
										else if( uc_tkeSendTab[i] == 0x03)
										{				
											uc_mnkSendTab[us_index] = 0x04;
											us_index++;
											uc_mnkSendTab[us_index] = 0x07;
											us_index++;
										}
										else if( uc_tkeSendTab[i] == 0x04)
										{										
											uc_mnkSendTab[us_index] = 0x04;
											us_index++;
											uc_mnkSendTab[us_index] = 0x04;
											us_index++;
										}
										else
										{
											uc_mnkSendTab[us_index] = uc_tkeSendTab[i];
											us_index++;
										}
									}	
									us_tkeProtelLen = us_index;
							
									// 重新计算CRC
									us_crcTmp = ObjCarBtm_DsCrc16Chk(uc_mnkSendTab+1,(us_index-1));
							
									// 设置ASCII码的CRC值
									us_ASIITmp = ObjCarBtm_HexToASCII((unsigned char)(us_crcTmp&0xff));    //CRC低位
									uc_mnkSendTab[us_index++] = (unsigned char)((us_ASIITmp>>8)&0xff);
									uc_mnkSendTab[us_index++] = (unsigned char)(us_ASIITmp&0xff);  

									us_ASIITmp = ObjCarBtm_HexToASCII((unsigned char)((us_crcTmp>>8)&0xff));  //CRC高位
									uc_mnkSendTab[us_index++] = (unsigned char)((us_ASIITmp>>8)&0xff);
									uc_mnkSendTab[us_index++] = (unsigned char)(us_ASIITmp&0xff);  
		
									// 末位赋值
									uc_mnkSendTab[us_index] = 0x03;
									us_index++;	
									us_tkeProtelLen=us_index;							
							
                                    #endif
									if(us_tkeProtelLen > 0)
									{
                                        #ifdef TDC_FUNCTION
                                        // TDC修改
                                        obj232_setSendData(uc_mnkSendTab,(unsigned char)us_tkeProtelLen );	
                                        #else 
										sci_uart_deal(0,0,uc_mnkSendTab,(unsigned char)us_tkeProtelLen); //发送处理
                                        #endif
									}			
								}
							}
						}
						enable_flag=0;
					}
				}
			}
			else
			{
				enable_flag=0;
			}
		}
	}	
}

/***********************************************************************
**函数名称:object_232_rece_deal
**函数功能:232接收数据处理
**输入参数: 
**			输入变量			数据类型			说明
**			pc_recvData		unsigned char*  接收的数据
**			us_232RecvLen	unsigned short	接收的数据长度
**			ps232_race_flag unsigned short*
**输出参数：
**			pc_tkeData		unsigned char*  tke协议的数据
**			ps_tkeDataLen	unsigned short* tke协议的长度
**			pc_mnkData		unsigned char*  mnk协议的数据
**			ps_mnkDataLen	unsigned short* mnk协议的长度
**			ps232_race_flag unsigned short* 密码授权
**			pl_enable_flag  unsigned long*  发送状态标记
**返回值:无
**日期:2022-11-06
**作者:
**说明:232的整体流程步骤如下：
**		1：接收数据的协议转化
**		1.1：判断接收的协议是否是蒂升协议，如果是蒂升的诊断协议，转化成mnk协议
**		1.2：如果是蒂升的iap升级协议，不需要转换，解析后直接回复
**		1.3：如果是蒂升的升级秘钥协议 则需要进行秘钥的flash写入
**		1.4：如果不是蒂升的协议则判断为MNK协议
**		2：对MNK协议进行CRC验证并且
***********************************************************************/
void object_232_rece_deal(unsigned char* pc_recvData ,unsigned short  us_232RecvLen,
						  unsigned char* pc_tkeData,  unsigned short* ps_tkeDataLen,
					      unsigned char* pc_mnkData,  unsigned short* ps_mnkDataLen,
						  unsigned short* ps232_race_flag,
						  unsigned long* pl_enable_flag)
{ 	
	unsigned short crc_ok_flag = 0;
	unsigned short crc_check_data = 0;
	
	if((us_232RecvLen>VALID_RECEVIE_MAX_LENGTH)||(us_232RecvLen<2))
	{
		return;
	}

	(*ps232_race_flag) = 0;
	// 进行协议的转换，将TKE协议转化成MNK协议, 同时把转化后的mnk协议和tke协议进行赋值
	uc_obj232_ProtocalFlag = Obj232_DataDeal( &pc_recvData[0], us_232RecvLen,  &pc_tkeData[0], ps_tkeDataLen, &pc_mnkData[0],ps_mnkDataLen, ps232_race_flag,pl_enable_flag );
	
	// 如果不是MNK也不是TKE则直接返回
    if((OBJ232_PROTOCAL_MNK!=uc_obj232_ProtocalFlag) && \
        (OBJ232_PROTOCAL_DS_DIAG_OK!=uc_obj232_ProtocalFlag)&& \
			(OBJ232_PROTOCAL_DS_BT_AES_UPDATE!=uc_obj232_ProtocalFlag))//只有MNK协议或者蒂森协议转换成功需要进行处理
        return;
	
	crc_check_data = pc_mnkData[(*ps_mnkDataLen)-2]+(pc_mnkData[(*ps_mnkDataLen)-1]<<8); //CRC校验值
	crc_ok_flag = 0;
	
	if(((feibiao_define&0x02)==0) || (function.f_code.factroy_nub != YONGRI))	
	{
		if(((pc_mnkData[0] == OBJ232_ADDR_1)||(pc_mnkData[0]==function.f_code.address_set))&&((*ps_mnkDataLen)>4)) //地址正确
		{
            if(OBJ232_PROTOCAL_MNK == uc_obj232_ProtocalFlag)
            {
                if((*ps232_race_flag) & 0x02)
    			{
    				if(crc_chk_value_bz(pc_mnkData,((*ps_mnkDataLen)-2))==crc_check_data)
    		  		{
    					crc_ok_flag = 1;
    					(*ps232_race_flag) |= 0x04; //标准校验校验通过
    				}
    			}
    			else 
    			{
    				if(crc_chk232_value1(pc_mnkData,((*ps_mnkDataLen)-2))==crc_check_data)
    		  		{
    					crc_ok_flag = 1;
    				}
    			}
				if(crc_ok_flag)
				{
					Obj232_mnkProtelStatusDeal(pc_mnkData, ps232_race_flag,pl_enable_flag );
				}
				else
					(*pl_enable_flag)=0x0014;				//crc check error
			}
			else if( OBJ232_PROTOCAL_DS_DIAG_OK == uc_obj232_ProtocalFlag )
			{
				(*ps232_race_flag) |= 0x04; //标准校验校验通过
				Obj232_mnkProtelStatusDeal(pc_mnkData, ps232_race_flag,pl_enable_flag );
			}
		}
	}
}

/***********************************************************************
**函数名称:object_232_send_deal
**函数功能:发送处理
**输入参数: 
**			输入变量			数据类型			说明
**			uc_mnkReceTab	unsigned char*  入参是MNK协议信息
**			ps232_race_flag unsigned short* 密码授权
**			pl_enable_flag  unsigned long*  发送状态标记
**输出参数
**			pc_232SendData	unsigned char*  发送数据内容
**			ps_232SendLengh unsigned short* 发送数据长度
**返回值:无
**日期:2022-10-24
**作者:10012468
**备注:AES_SAVE_GOING   AES秘钥正在存储中
**	   AES_SAVE_FINISH  AES秘钥存储完毕
**	   AES_SAVE_ERROR   AES秘钥存储错误
***********************************************************************/
void object_232_send_deal(unsigned char* uc_mnkReceTab, 
						  unsigned short* ps232_race_flag,
						  unsigned long* pl_enable_flag, 
						  unsigned char* pc_232SendData, 
						  unsigned short* ps_232SendLengh)
{
	unsigned short i=0,m=0;
	unsigned short data_addr = 0;
	unsigned short data_counter = 0;
	unsigned short obj232_group_number = 0;
	unsigned short addrtemp = 0;
	unsigned short addrtemp1 = 0;
	unsigned short addrtemp2 = 0;
	unsigned short addrtemp3 = 0;
	unsigned short short1 = 0;
	unsigned short short2 = 0;
	unsigned short us_ansFlag=0;
    unsigned short us_funcVal = 0;
	
	pc_232SendData[0]=uc_mnkReceTab[0];
	pc_232SendData[1]=uc_mnkReceTab[1];
	(*ps_232SendLengh) = 6;
	
	if((*pl_enable_flag)&0x0004)		//come back crc check error code 0x0003
	{
		pc_232SendData[2]=0x80;
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0x03;
	}
	else if((*pl_enable_flag)&0x0400)	//come back password error code 0x0001
	{
		pc_232SendData[2]=0x80;
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0x01;
	}
	else if((*pl_enable_flag)&0x0800)	//come back password pass code 0x8888
	{
		pc_232SendData[2]=uc_mnkReceTab[2];   //0806
		pc_232SendData[3]=uc_mnkReceTab[3];
		pc_232SendData[4]=0x88;
		pc_232SendData[5]=0x88;
	}
	else if((*pl_enable_flag)&0x1000)	//come back system is locked code 0x0007
	{
		pc_232SendData[2]=0x80;
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0x07;
	}
	else if((*pl_enable_flag)&0x2000)	//save function busing is locked code 0x0008
	{
		pc_232SendData[2]=0x80;
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0x08;
	}
	else if((*pl_enable_flag)&0x0200)	//come back para don't written code 0x0006
	{
		pc_232SendData[2]=0x80;
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0x06;
	}
	else if((*pl_enable_flag)&0x0040)	//come back para over error code 0x0005
	{
		pc_232SendData[2]=0x80;
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0x05;
	}
	else if((*pl_enable_flag)&0x0080)	//come back address over error code 0x0004
	{
		pc_232SendData[2]=0x80;
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0x04;
	}
	else if((*pl_enable_flag)&0x0100)	//come back command error code 0x0002
	{
		pc_232SendData[2]=0x80;
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0x02;
	}
	else if((*pl_enable_flag)&0x4000)	   //come back read or write para need safety password
	{
		pc_232SendData[2]=0x80;
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0x09;
	}
#ifdef SHDS_FB_BUILD
    else if(((*pl_enable_flag)&0x10000) && (OBJ232_PROTOCAL_DS_DIAG_FAIL==uc_obj232_ProtocalFlag))
    {
		pc_232SendData[2]=0x80;//bit16:1:SHDS Invalid frame
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0xD1;
    }
    else if(((*pl_enable_flag)&0x20000) && (OBJ232_PROTOCAL_DS_DIAG_FAIL==uc_obj232_ProtocalFlag))
    {
		pc_232SendData[2]=0x80;//bit17:1:SHDS Invalid communication address
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0xD2;
    }
    else if(((*pl_enable_flag)&0x40000) && (OBJ232_PROTOCAL_DS_DIAG_FAIL==uc_obj232_ProtocalFlag))
    {
		pc_232SendData[2]=0x80;//bit18:1:SHDS Invalid hex's ASCII
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0xD3;
    }
    else if(((*pl_enable_flag)&0x80000) && (OBJ232_PROTOCAL_DS_DIAG_FAIL==uc_obj232_ProtocalFlag))
    {
		pc_232SendData[2]=0x80;//bit19:1:SHDS The number of bytes written is not equal to 2
		pc_232SendData[3]=0x01;
		pc_232SendData[4]=0x00;
		pc_232SendData[5]=0xD4;
    }
    #ifndef COMMON_BLUETOOTH
	else if(((*pl_enable_flag)&0x100000) && (OBJ232_PROTOCAL_DS_DIAG_FAIL==uc_obj232_ProtocalFlag))
    {
		pc_232SendData[0]=0x02;
		pc_232SendData[1]='F';
		pc_232SendData[2]='F';
		pc_232SendData[3]='Y';
		pc_232SendData[4]='1';
		pc_232SendData[5]='F';
		
		uc_updateTemp[0] = '1';
		uc_updateTemp[1]='F';
		Cf_BlueToothAesEncode(AES3, &uc_updateTemp[0], 2, &pc_232SendData[4] );
		
		us_dsCrc = ObjCarBtm_DsUpdateAesCrc16Chk(pc_232SendData+1, 19); // 只计算FF 'Y' '1' 'F'
		
		us_ASIITmp = ObjCarBtm_HexToASCII((unsigned char)(us_dsCrc&0xff));    //CRC低位
		pc_232SendData[20] = (unsigned char)((us_ASIITmp>>8)&0xff);
		pc_232SendData[21] = (unsigned char)(us_ASIITmp&0xff);  

		us_ASIITmp = ObjCarBtm_HexToASCII((unsigned char)((us_dsCrc>>8)&0xff));  //CRC高位
		pc_232SendData[22] = (unsigned char)((us_ASIITmp>>8)&0xff);
		pc_232SendData[23] = (unsigned char)(us_ASIITmp&0xff);  
		
		pc_232SendData[24] = 0x03;
		uc_obj232_ProtocalFlag = OBJ232_PROTOCAL_DS_BT_AES_UPDATE;
		
		(*ps_232SendLengh) = 25;
    }
    #endif
#endif
	else
	{
		if((*pl_enable_flag)&0x0001)	//read data from the slave
		{
			if( uc_mnkReceTab[1] == OBJ232_CMD_HANDSHAKE)
			{
				Obj232_Frame70SendDeal(uc_mnkReceTab,pc_232SendData,ps_232SendLengh);
			}
			else if( uc_mnkReceTab[1] == OBJ232_CMD_READ_DEFAULT)
			{
				Obj232_Frame73SendDeal(uc_mnkReceTab,ps232_race_flag,pc_232SendData,ps_232SendLengh);
			}
			else if( uc_mnkReceTab[1] == OBJ232_CMD_READIAP)
			{
				Obj232_Frame45SendDeal(uc_mnkReceTab,pc_232SendData,ps_232SendLengh);
			}
			else if( uc_mnkReceTab[1] == OBJ232_CMD_WRITEIAP )
			{
				Obj232_Frame46SendDeal(uc_mnkReceTab,pc_232SendData,ps_232SendLengh);
			}	
			else if(uc_mnkReceTab[1] == OBJ232_CMD_WRITE_DEFAULT)
			{
				 Obj232_Frame76SendDeal(uc_mnkReceTab,pc_232SendData,ps_232SendLengh);
			}
            #ifndef COMMON_BLUETOOTH
			else if(OBJ232_PROTOCAL_DS_BT_AES_UPDATE==uc_obj232_ProtocalFlag)
			{
				pc_232SendData[0]=0x02;
				pc_232SendData[1]='F';
				pc_232SendData[2]='F';
				pc_232SendData[3]='Y';
				pc_232SendData[4]='1';
				pc_232SendData[5]='0';
		
				uc_updateTemp[0] = '1';
				uc_updateTemp[1]='0';
				Cf_BlueToothAesEncode(AES3, &uc_updateTemp[0], 2, &pc_232SendData[4] );
				
				us_dsCrc = ObjCarBtm_DsUpdateAesCrc16Chk(pc_232SendData+1, 19); // 只计算FF 'Y' '1' '0'
				
				us_ASIITmp = ObjCarBtm_HexToASCII((unsigned char)(us_dsCrc&0xff));    //CRC低位
				pc_232SendData[20] = (unsigned char)((us_ASIITmp>>8)&0xff);
				pc_232SendData[21] = (unsigned char)(us_ASIITmp&0xff);  

				us_ASIITmp = ObjCarBtm_HexToASCII((unsigned char)((us_dsCrc>>8)&0xff));  //CRC高位
				pc_232SendData[22] = (unsigned char)((us_ASIITmp>>8)&0xff);
				pc_232SendData[23] = (unsigned char)(us_ASIITmp&0xff);  
		
				pc_232SendData[24] = 0x03;
		
				(*ps_232SendLengh) = 25;		
			}
            #endif
			else
			{
				if((function.f_code.f611_select&0x01) && (((*ps232_race_flag) & 0x04) == 0))
				{
					pc_232SendData[2]=uc_mnkReceTab[5]*2;	//come back data number byte
					(*ps_232SendLengh)=3;
				}
				else
				{
					pc_232SendData[2]=uc_mnkReceTab[4];	//come back data number byte
					pc_232SendData[3]=uc_mnkReceTab[5]*2;	//come back data number byte
					(*ps_232SendLengh)=4;
			    }
				data_addr=(uc_mnkReceTab[2]<<8)+uc_mnkReceTab[3];
				data_counter=(uc_mnkReceTab[4]<<8)+uc_mnkReceTab[5];
				addrtemp=data_addr&0x0f;
				switch(data_addr&0xf000)
				{

					case 0x3000:
						obj232_group_number=(data_addr&0x0f00)>>8;
						if(obj232_group_number==0)
						{
							pc_232SendData[(*ps_232SendLengh)++]=0;
							if(!(run_status_flag&bit0))
								pc_232SendData[(*ps_232SendLengh)]=3;
							else if(run_status_flag&bit1) //下行
								pc_232SendData[(*ps_232SendLengh)]=2;
							else
								pc_232SendData[(*ps_232SendLengh)]=1;
							(*ps_232SendLengh)++;
						}
						break;
					case 0x8000:
#ifdef SHDS_FB_BUILD
                        us_funcVal = CfErrMap_mnkErrToShdsErr(data_addr);
						pc_232SendData[(*ps_232SendLengh)++]=us_funcVal>>8;
						pc_232SendData[(*ps_232SendLengh)++]=us_funcVal&0xff;
#else
						if((data_addr&0xfff)==0)//8000
						{
							pc_232SendData[(*ps_232SendLengh)++]=0;
							pc_232SendData[(*ps_232SendLengh)++]=error_code;
						}
						else if((data_addr&0xfff)==2)//8002
						{
							pc_232SendData[(*ps_232SendLengh)++]=0;
							pc_232SendData[(*ps_232SendLengh)++]=sub_error_code;						
						}
#endif
						break;
					case OBJ232_READ_ADDR_9000:													
#ifdef SHDS_FB_BUILD
						if((data_addr>=0x9D40)&&(data_addr<0x9DA4))
						{// 地址
							short1 = data_addr-0x9D40;
							for(i=0;i<data_counter;i++)
							{
								us_funcVal=un_event.data[short1];
								if((short1%EVENT_MEMBER_SUB)==0)
								{// 转换为蒂森的事件编号
									if(us_funcVal)
									{
										us_funcVal=TKE_EVENT_NUB_BAS+us_funcVal;
									}
								}
								
								pc_232SendData[(*ps_232SendLengh)++] = (us_funcVal>>8); 
								pc_232SendData[(*ps_232SendLengh)++] = (us_funcVal&0xff);
								short1++;
							}                            							
						}
#ifdef BRK_EVENT_RECORD                
                        else if((data_addr>=0x9DFA)&&(data_addr<=0x9DFF))// A01.7-CR004-event
						{// 地址
						
                            if(data_addr==0x9DFA)
                            {// 抱闸力事件状态
                                if(BRK_EVNT_RD_DONE==st_brakeEventRead.readStatus)
                                {// eep读取完成，返回事件编号
                                    pc_232SendData[(*ps_232SendLengh)++] = ((st_brakeEventRead.evntNub)>>8); 
                                    pc_232SendData[(*ps_232SendLengh)++] = ((st_brakeEventRead.evntNub)&0xff);  
                                }
                                else
                                {
        							pc_232SendData[(*ps_232SendLengh)++] = 0; 
        							pc_232SendData[(*ps_232SendLengh)++] = 0;                                      
                                } 
                                data_addr=data_addr+1;
                                data_counter=data_counter-1;                                
                            }
                        
							short1 = data_addr-0x9DFB;
							for(i=0;i<data_counter;i++)
							{
								us_funcVal=st_brakeEventRead.eventData.data[short1];
								if((short1%EVENT_MEMBER_SUB)==0)
								{// 转换为蒂森的事件编号
									if(us_funcVal)
									{
										us_funcVal=BRAKE_EVENT_NUB_BAS+us_funcVal;
									}
								}
								
								pc_232SendData[(*ps_232SendLengh)++] = (us_funcVal>>8); 
								pc_232SendData[(*ps_232SendLengh)++] = (us_funcVal&0xff);
								short1++;
							}      
                            //st_brakeEventRead.readStatus=BRK_EVNT_RD_IDEL;// 不需要了
						}
#endif                        
						else if(((data_addr&0xff00)==0x9B00) || ((data_addr&0xff00)==0x9C00) ||
							((data_addr&0xff00)==0x9D00))
						{
							short1 = data_addr;
							for(i=0;i<data_counter;i++)
							{
								us_funcVal = CfErrMap_mnkErrToShdsErr(short1);
								pc_232SendData[(*ps_232SendLengh)++] = (us_funcVal>>8); 
								pc_232SendData[(*ps_232SendLengh)++] = (us_funcVal&0xff);
								short1++;
							}                            
						}
						else if((data_addr&0xff00)==0x9A00)
						{
							if((data_addr&0xff)==0)//门状态
							{
								pc_232SendData[(*ps_232SendLengh)++] = (us_logic_doorStatus>>8); //门2状态
								pc_232SendData[(*ps_232SendLengh)++] = (us_logic_doorStatus&0xff);//	门1状态							
							}
							else if((data_addr&0xff)==1)
							{
								pc_232SendData[(*ps_232SendLengh)++] = lg_cop_floor_display2; //内召显示中位
								pc_232SendData[(*ps_232SendLengh)++] = lg_cop_floor_display1;//	内召显示个位						
							}
							else if((data_addr&0xff)==2)
							{
								pc_232SendData[(*ps_232SendLengh)++] = 0x00;	//20.01-10
								pc_232SendData[(*ps_232SendLengh)++] = lg_cop_floor_display3;//内召显示高位							
							}
							else if((data_addr&0xff)==3)
							{
								pc_232SendData[(*ps_232SendLengh)++] = lg_hbi_floor_display2;//外召显示中位	
								pc_232SendData[(*ps_232SendLengh)++] = lg_hbi_floor_display1;//外召显示个位							
							}
							else if((data_addr&0xff)==4)
							{
								pc_232SendData[(*ps_232SendLengh)++] = 0x00;	
								pc_232SendData[(*ps_232SendLengh)++] = lg_hbi_floor_display3;//外召显示高位							
							}
							else if(((data_addr&0xff)>=0x06)&&((data_addr&0xff)<=0x16))//0x9A06~0x9A16 EID的所有信息
							{
								addrtemp1=(data_addr&0xff);
								addrtemp1=(addrtemp1-6)*2;
								
								if(us_obj232_mc2eidFlag)//答复主板EID
								{
									us_obj232_mc2eidFlag=0;
									for(i=0;i<data_counter;i++)
									{
										pc_232SendData[(*ps_232SendLengh)++]=*rs232_mc2EidDisplay_para[addrtemp1++];
										pc_232SendData[(*ps_232SendLengh)++]=*rs232_mc2EidDisplay_para[addrtemp1++];

									}
								}
                                else if(us_obj232_dspEidFlag)
                                {
                                    us_obj232_dspEidFlag = 0;
									for(i=0;i<data_counter;i++)
									{
										pc_232SendData[(*ps_232SendLengh)++]=*rs232_dspEidDisplay_para[addrtemp1++];
										pc_232SendData[(*ps_232SendLengh)++]=*rs232_dspEidDisplay_para[addrtemp1++];
									}
                                }
								else//答复其余单板EID
								{
									for(m=0;m<EID_FLAG_LEN;m++)
									{
										if(us_obj232_eidFlagBak[m] )//判断问询的单板在线  & us_objMcp2515_lopInlineFlag[m]
										{
											if(us_obj232_eidFlagBak[m] == us_canRecv_eidFlag[m])//主板CAN接收到的EID标记与请求是否一致
											{
												us_obj232_eidFlagBak[m]=0;//清除上位机请求备份
												us_ansFlag=1;
											}
										}
									}
									if(us_ansFlag)//答复在线的单板EID
									{
										for(i=0;i<data_counter;i++)
										{
											pc_232SendData[(*ps_232SendLengh)++]=*rs232_boardEidDisplay_para[addrtemp1++];
											pc_232SendData[(*ps_232SendLengh)++]=*rs232_boardEidDisplay_para[addrtemp1++];
										}
										us_ansFlag=0;
									}
									else
									{
										for(i=0;i<data_counter;i++)
										{
											pc_232SendData[(*ps_232SendLengh)++]=0x30;
											pc_232SendData[(*ps_232SendLengh)++]=0x30;											
										}
									}
								}
							}
                            else if(((data_addr&0xff)>=0x17)&&((data_addr&0xff)<=0x1d))//查询在线
							{
								addrtemp2=((data_addr&0xff)-0x17);
								for(i=0;i<data_counter;i++)
								{
									pc_232SendData[(*ps_232SendLengh)++] = us_objMcp2515_lopInlineFlag[addrtemp2]>>8;
									pc_232SendData[(*ps_232SendLengh)++] = (us_objMcp2515_lopInlineFlag[addrtemp2]&0xff);
									addrtemp2++;
								}
							}
							else if((data_addr&0xff)==0x1e)//mm对应的脉冲数
							{
								pc_232SendData[(*ps_232SendLengh)++] = (us_s_mmToPulse>>8); //mm对应脉冲数的100倍的高8位
								pc_232SendData[(*ps_232SendLengh)++] = (us_s_mmToPulse&0xff);//mm对应脉冲数的100倍的低8位					
							}								
							else if((data_addr&0xff)==0x1f)//轿顶板输入端子状态
							{
								pc_232SendData[(*ps_232SendLengh)++] = (us_cf_mf3InSatus>>8);
								pc_232SendData[(*ps_232SendLengh)++] = (us_cf_mf3InSatus&0xff);
							}
							else if(((data_addr&0xff)>=0x20)&&((data_addr&0xff)<=0x25))
							{
								addrtemp3=((data_addr&0xff)-0x20);
								for(i=0;i<data_counter;i++)
								{
									pc_232SendData[(*ps_232SendLengh)++] = ((*rs232_pulse_para[addrtemp3])>>8);
									pc_232SendData[(*ps_232SendLengh)++] = ((*rs232_pulse_para[addrtemp3])&0xff);
									addrtemp3++;
								}
							}
                            else if(((data_addr&0xff)>=0x26)&&((data_addr&0xff)<=0x29))//0x9A26~0x9A29 EID的所有信息
                            {
                                addrtemp1=(data_addr&0xff);
								addrtemp1=(addrtemp1-0x26)*2 + 34;
                                
                                for(i=0;i<data_counter;i++)
                                {
                                    pc_232SendData[(*ps_232SendLengh)++]=*rs232_boardEidDisplay_para[addrtemp1++];
                                    pc_232SendData[(*ps_232SendLengh)++]=*rs232_boardEidDisplay_para[addrtemp1++];
                                }
                            }
							else if(((data_addr&0xff)>=0x31)&&((data_addr&0xff)<=0x37))
							{
								addrtemp3=((data_addr&0xff)-0x31 + 0x07);//A01.7-CR001 数组从[7]开始
								for(i=0;i<data_counter;i++)
								{
									pc_232SendData[(*ps_232SendLengh)++] = ((us_objMcp2515_lopInlineFlag[addrtemp3])>>8);//A01.7-CR001
									pc_232SendData[(*ps_232SendLengh)++] = ((us_objMcp2515_lopInlineFlag[addrtemp3])&0xff);//A01.7-CR001
									addrtemp3++;
								}
							}
							else if(((data_addr&0xff)>=0x40)&&((data_addr&0xff)<=0x43))//A01.1-R53
							{
								addrtemp3=((data_addr&0xff)-0x40);
								
                                #ifdef TDC_FUNCTION
                                main_board_sw_version_get(&uc_softwareVer[0]);
                                #endif
                                
								us_obj232_hwAndSwId[0] = ((unsigned short)HARDWARE_VER0<<8)+(HARDWARE_VER1&0xFF);
								us_obj232_hwAndSwId[1] = ((unsigned short)HARDWARE_VER2<<8)+(HARDWARE_VER3&0xFF);
								us_obj232_hwAndSwId[2] = ((unsigned short)SOFTWARE_VER0<<8)+(SOFTWARE_VER1&0xFF);
								us_obj232_hwAndSwId[3] = ((unsigned short)SOFTWARE_VER2<<8)+(SOFTWARE_VER3&0xFF);
								for(i=0;i<data_counter;i++)
								{
									pc_232SendData[(*ps_232SendLengh)++] = ((us_obj232_hwAndSwId[addrtemp3])>>8);
									pc_232SendData[(*ps_232SendLengh)++] = ((us_obj232_hwAndSwId[addrtemp3])&0xff);
									addrtemp3++;
								}
							}
							else if(((data_addr&0xff)>=0x45)&&((data_addr&0xff)<=0x46))//A01.1-R53
							{
								addrtemp3=((data_addr&0xff)-0x45);
							
								for(i=0;i<data_counter;i++)
								{
									pc_232SendData[(*ps_232SendLengh)++] = ((us_cf_SyetemStateFlag[addrtemp3])>>8);
									pc_232SendData[(*ps_232SendLengh)++] = ((us_cf_SyetemStateFlag[addrtemp3])&0xff);
									addrtemp3++;
								}
							}
							else if(((data_addr&0xff)>=0x50)&&((data_addr&0xff)<=0x51))  //A01.8-CR009
							{ 
								addrtemp3=((data_addr&0xff)-0x50);                        //蓝牙软硬件版本
								for(i=0;i<data_counter;i++)
								{
                                    pc_232SendData[(*ps_232SendLengh)++] = (unsigned char)(us_obj232_btVrsn[addrtemp3]>>8);
								    pc_232SendData[(*ps_232SendLengh)++] = (unsigned char)(us_obj232_btVrsn[addrtemp3]&0xff);
                                    addrtemp3++;
								}
							}
							else if(((data_addr&0xff)>=0x53)&&((data_addr&0xff)<=0x56))  //蓝牙mac地址
							{
								addrtemp3=((data_addr&0xff)-0x53);
								for(i=0;i<data_counter;i++)
								{
                                    pc_232SendData[(*ps_232SendLengh)++] = uc_cf_btMacAddr[addrtemp3++];
								    pc_232SendData[(*ps_232SendLengh)++] = uc_cf_btMacAddr[addrtemp3++];
								}
							}  
                            else if((data_addr&0xff)>=0x57)
                            {
                                Obj232_grp9aSendChk(data_addr,data_counter,pc_232SendData,(unsigned char*)ps_232SendLengh);
                            }
						}
						else
#endif						
							Obj232_Read9000AddrSend(data_addr,data_counter,pc_232SendData,ps_232SendLengh);
						break;	
					default:
						break;
				}
			}
		}
		else if((*pl_enable_flag)&0x0002)//write data to the slave
		{
			pc_232SendData[2]=uc_mnkReceTab[2];
			pc_232SendData[3]=uc_mnkReceTab[3];
			pc_232SendData[4]=uc_mnkReceTab[4];
			pc_232SendData[5]=uc_mnkReceTab[5];
			(*ps_232SendLengh)=6;
			if( uc_mnkReceTab[1] == OBJ232_CMD_WRITE_DEFAULT)
			{
				pc_232SendData[6]=uc_mnkReceTab[6];
				(*ps_232SendLengh)=7;
			}
		}
	}
    if(OBJ232_PROTOCAL_MNK == uc_obj232_ProtocalFlag)
    {
        short1 = Obj232_crc(pc_232SendData,(*ps_232SendLengh));
		pc_232SendData[(*ps_232SendLengh)++]=short1&0xff;
		pc_232SendData[(*ps_232SendLengh)++]=short1>>8;  
    }
}


unsigned short Obj232_crc(unsigned char *data_value,unsigned char length)
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
	return(crc_value);
}



