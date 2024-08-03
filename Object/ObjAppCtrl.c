#include "ObjAppCtrl.h"

/*******************************************************
**函数名称:NovaCtrlApp_ParaInit
**函数功能:APP调检模块参数初始化
**输入参数:void
**返回值:void
**作者:MAO
**备注:
*******************************************************/
void NovaCtrlApp_ParaInit(void)
{
    unsigned short i=0;

    for(i=0;i<4;i++)
        un_novaApp_deBugFlag.all[i]= 0;
    NovaCtrlApp_ClearDebugRgstr();
    memset(&st_novaApp_flag,0,sizeof(struct NOVA_APP_DEBUG_FLAG));
}

/*******************************************************
**函数名称：NovaCtrlApp_DebugMainDeal
**函数功能：
**输入参数：void
**返回值：void
**日期：2024-5-8
**作者：MAO
**备注：
*******************************************************/
void NovaCtrlApp_DebugMainDeal(void)
{
    NovaCtrlApp_DebugInfoGet();     //测试过程基本信息获取
    NovaCtrlApp_ConectAppDeal();    //APP连接处理
    NovaCtrlApp_Enter();            //调检状态进入处理
    NovaCtrlApp_DebugItemDeal();    //调检项目处理
    NovaCtrlApp_Quit();             //调检状态退出处理
}

/*******************************************************
**函数名称：NovaCtrlApp_ConectAppDeal
**函数功能：手机APP连接处理
**输入参数：
**返回值：
**日期：2024-5-8
**作者：MAO
**备注：
*******************************************************/
void NovaCtrlApp_ConectAppDeal(void)
{
    static BOOL b_RandomCodeGet=false;
    //进入连接状态
    if(un_novaApp_deBugFlag.bit.b_ conectReqt && (APP_CONNECT_FAIL==un_novaApp_deBugFlag.bit.b_conectStatus)
    {
        us_novaApp_timeHeart1 = 0;
        if(b_RandomCodeGet==false)
        {
            us_novaApp_RandomCode = NovaCtrlApp_ConectRandomCodeGet();
            b_RandomCodeGet = true;
        }
        if(us_novaApp_conectPswrd==0)
        {
            un_novaApp_deBugFlag. bit. b_cnPswrdStatus = CONECT_CODE_NO_RECEIVE;
        }

        if(true == NovaCtrlApp_RandomCodeDisplay())
        {
            if(us_novaApp_conectPswrd=us_novaApp_RandomCode)
            //if(1)
            {
                un_novaApp_deBugFlag.bit.b_conectStatus = APP_CONNECT_OK;
                un_novaApp_deBugFlag.bit.b_cnPswrdStatus =CONECT_CODE_CORRECT;
                un_novaApp_sucDisplayTimer =20;
            }
            else
            {
                if(us_novaApp_conectPswrd != 0)
                    un_novaApp_deBugFlag.bit.b_cnPswrdStatus = CONECT_CODE_WRONG;            
            }
        }
        else
        {
            un_novaApp_deBugFlag.bit.b_cnPswrdStatus = CONECT_CODE_OVER_TIME;
            un_novaApp_deBugFlag.bit.b_conectReqt =APP_CONNECT_REQT_CLEAR;
        }
    }
    else
    {
        b_RandomCodeGet = false;
        us_novaApp_timeRandomcode = 0;
        un_novaApp_deBugFlag.bit.b_conectReqt = APP_CONNECT_REQT_CLEAR;
    }

    NovaCtrlApp_SucDisplay();

    //连接超时处理
    if(un_novaApp_deBugFlag.bit.b_conectStatus == APP_CONNECT_OK)
    {
        us_novaApp_timeHeart1 += DELAY_TIMER_10Ms;
        if(us_novaApp_heartBeatFrm == DEGUB_HEART_FRAME)
        {
            if(IN_DEBUG_STATUS != un_novaApp_deBugFlag.bit.b_debugStatus)
            {
                us_novaApp_heartBeatFrm= 0;
            }
            us_novaApp_timeHeart1 = 0;
        }

        if(us_novaApp_timeHeart1 >=DELAY_TIMER_10s)
        {
            us_novaApp_timeHeart1 = DELAY_TIMER_10s;
            un_novaApp_deBugFlag.bit.b_conectstatus = APP_CONNECT_FAIL;
            un_novaApp_deBugFlag.bit.b_cnPswrdStatus = CONECT_CODE_NO_RECEIVE;us_novaApp_conectPswrd = 0;
            un_novaApp_conectPswrd = 0;
        }
    }
}



