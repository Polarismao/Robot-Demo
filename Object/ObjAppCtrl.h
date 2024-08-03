#ifndef _APPCTRLTEST_H
#define _APPCTRLTEST_H

#include "ObjAppCtrlPublic.h"
#include "stdlib.h”
#include <string.h>


#define DEGUB_HEART_FRAME          ((un_novaApp_deBugFlag.bit.b_debugStatus!=IN_DEBUG_STATUS) ? 0xAAS5:0xAA66)
#define APP_START_END_CHECKING     (function.f_code.A1[193])
#define APP_TEST_OVERTIME_WRITE    (function.f_code.A1[178])
#define MAX_FLOOR                   64
#define READ_REGISTER_NUM           15
#define CHIP_UID_NUM                24

//调检允许条件
#define DEBUG_RSN_NULL             0   //没有问题

//UCMP测试步序
#define UCMP_TEST_WAITING          0   //测试前准备工作，呼梯到次顶层
#define UCMP_TEST_RECORD_A         1   //记录位置A
#define UCMP_TEST_RECORD_B1        2   //记录位置B1
#define UCMP_TEST_RECORD_B2        3   //记录位置B2
#define UCMP_TEST_RECORD_C         4   //记录位置c
#define UCMP_TEST_CALCULATION      5   //计算制停距离,测试结束

//封星测试步序
#define STARDELTA_TEST_WAITING     0   //测试前准备工作，呼梯到次顶层
#define STARDELTA_TEST_TRIGGER     1   //封星测试触发

//缓冲器测试步序
#define BUFFER_TEST_WAITING        0    //呼梯到端站
#define BUFFER_TEST_RCRD_EDGE      1    //记录平层边沿位器
#define BUFFER_TEST_RCRD_SOFT      2    //记录软限位位置
#define BUFFER_TEST_RCRD_LIMIT     3    //记录极限位置
#define BUFFER_TEST_RCRD_BUFFER    4    //记录缓冲器位置
#define BUFFER_TEST_CALCULATION    5    //计算间距，测试结束

//抱闸制停测试步序
#define BKSTOP_TEST_WAITING        0   //呼梯到端站
#define BKSTOP_TEST_RCRD_START     1   //记录落抱闸时刻的位置
#define BKSTOP_TEST_RCRD_STATIC    2   //记录轿厢完成制停时的位置
#define BKSTOP_TEST_RELEASE        3   //释放双抱闸
#define BKSTOP_TEST_RELEVEL        4   //开始返平层
#define BKSTOP_TEST_CALCULATION    5   //计算各间距

//井道自学习状态，外呼地址自学习状态
#define SHAFT_STUDY_WAITING        0   //未进入学习
#define SHAFT_STUDY_GOING          1   //井道自学习中
#define SHAFT_STUDY_suc            2   //井道自学习完成，学习成功
#define SHAFT_STUDY_FAIL           3   //井道自学习完成，学习失败
#define HBIADDR_STUDY_GOING        4   //外呼地址自学习中
#define HBIADDR_STUDY_suC          5   //外呼地址自学习完成，学习成功
#define HBIADDR_STUDY_FAIL         6   //外呼地址自学习完成，学习失败
#define STUDY_ABEXIT               7   //异常退出
#define STUDY_SELFCHECK_FAIL       8   //触发命令自检不通过


//测试状态
#define DEBUG_TEST_WAIT               0x00
#define DEBUG_TEST_GOING              0x01
#define DEBUG_TEST_FINISH_SUCCESS     0x02
#define DEBUG_TEST_FINISH_FAIL        0x03
#define DEBUG_TEST_ABEXIT             0x04
#define DEBUG_TEST_SELFCHECK_FAIL     0x05 

//抱闸力测试状态
#define BREAK_TEST_WAIT               0x00
#define BREAK_RIGHT_TEST_GOING        0x01
#define BREAK_RIGHT_TEST_SUCCESS      0x02
#define BREAK_RIGHT_TEST_FAIL         0x03
#define BREAK_LEFT_TEST_GOING         0x04
#define BREAK_LEFT_TEST_SUCCESS       0x05
#define BREAK_LEFT_TEST_FAIL          0x06
#define BREAK_TEST_ABEXIT             0x07
#define BREAK_TEST_SELFCHECK_FAIL     0x08



enum CHECK_ITEM_TYPE en_novaApp_itemCurnt;
union NOVA_APP_CONECT_FLAG un_novaApp_deBugFlag;
union NOVA_APP_DEBUG_w_FLAG un_novaApp_testTrigger;
union NOVA_APP_DEBUG_R_FLAG un_novaApp_resultGet;
struct NOVA_APP_DEBUG_w_RGSTR un_novaApp_writeRegister;
struct NOVA_APP_ DEBUG_FLAG st_novaApp_flag;
union NOVA_APP_DEBUG_R_RGSTR_FLAG un_novaApp_readRegister;
union NOVA_APP_DEBUG_CONDITION_FLAG un_novaApp_AutoDebugCndition;
union NOVA_APP_DEBUG_CONDITION_FLAG un_novaApp_InsDebugCndition;
union NOVA_APP_TEST_CONDITION_FLAG un_novaApp_testCndition;
union NOVA_APP_TEST_CONDITION_FLAG un_novaApp_triggerCndition;



unsigned short us_novaApp_ucmpSetSpeed;
unsigned short us_novaApp_torquecur;
unsigned short us_novaApp_dspMainstatusBak;
unsigned short us_novaAPP_stopTime=6;
unsigned short us_novaApp_debugFuncVer=0x0101;
unsigned short us_novaApp_overTime=30;
unsigned short us_novaApp_overTimeCnt=30;
unsigned short us_novaApp_timeRndmFctr1;
unsigned short us_novaApp_timeRndmFctr2;
unsigned short us_novaApp_timeRandomCode;
unsigned short un_novaApp_sucDisplayTimer;
unsigned short un_novaApp_failDisplayTimer;
unsigned short un_novaApp_tstDisplayTimer;
unsigned short un_novaApp_outDisplayTimer;
unsigned short us_novaApp_heartBeatFrm;
unsigned short us_novaApp_conectPswrd;
unsigned short us_novaApp_debugPswrd;
unsigned short us_novaApp_RandomCode;
unsigned short us_novaApp_randomfactor;
unsigned short us_novaApp_sprtStatus;
unsigned short us_novaApp_startStation;
unsigned short us_novaApp_doorCmd;
signed    long l_novaApp_ucmpPosA;
signed    long l_novaApp_ucmpPosB;
signed    long l_novaApp_ucmpPosC;
unsigned short us_novaApp_timeHeart1;    //连接状态下丢失心跳帧时间
unsigned long ul_novaApp_timeHeart2;     //调检状态下丢失心跳帧时间
unsigned long ul_novaApp_timeHeart4;     //单独测试项的超时时间
unsigned long ul_novaApp_posEdge;
unsigned long ul_novaApp_posSoft;
unsigned long ul_novaApp_posLimit;
unsigned long ul_novaApp_posBuffer;
unsigned long ul_novaApp_posBkFall;
unsigned long ul_novaApp_posCarStatic;
unsigned long ul_novaApp_posDoorEdge;
unsigned long ul_novaApp_pulsecurrent;
unsigned long ul_novaApp_HighL1;
unsigned long ul_novaApp_HighL2;
unsigned long ul_novaApp_AHighL1;
unsigned long ul_novaApp_AHighL2;
unsigned long ul_novaApp_ALowL1;
unsigned long ul_novaApp_ALowL2;
unsigned long ul_novaApp_CHighL1;
unsigned long ul_novaApp_CHighL2;
unsigned long ul_novaApp_CLowL1;
unsigned long ul_novaApp_CLowL2;
unsigned short us_novaApp_rightBkPluseRcrd = 0;
unsigned short us_novaApp_leftBkPluseRcrd = 0;
signed long l_novaApp_distncPluseRecord = 0;

unsigned short us_novaApp_barLenth[MAX_FLOOR] = {0};
unsigned short us_novaApp_lenDoorA[MAX_FLOOR] = {0};
unsigned short us_novaApp_lenDoorC[MAX_FLOOR] = {0};
unsigned char uc_novaApp_productID[24];


BOOL NovaCtrlApp_BufferNoSwitchActJudge(BOOL* b_record, enum TEST_DIR e_testDir);
BOOL NovaCtrlApp_RandomCodeDisplay(void);
void NovaCtrlApp_UcmpTest(void);
void NovaCtrlApp_UcmpDeal(unsigned short* us_TestStep, BOOL* b_ucmpTestEn);
void NovaCtrlApp_NewBalancestudyTest(void);
void NovaCtrlApp_ShaftstudyDeal(void);
void NovaCtrlApp_shaftstudyDataDeal(void);
void NovaCtrlApp_SystemReset(void);
void NovaCtrlApp_ClearDebugRgstr(void);
void NovaCtrlApp_HbiCopDisplayT(void);
void NovaCtrlApp_SucDisplay(void);
void NovaCtrlApp_FailDisplay(void);
void NovaCtrlApp_outDisplay(void);
void NovaCtrlApp_TSTDisplay(void);
void NovaCtrlApp_PulseDistncCalDeal(void);
void NovaCtrlApp_EndBoardLenthCalDeal(void);
void NovaCtrlApp_OverTimeDeal(void);
void NovaCtrlApp_ProductUIDGetDeal(void);
void NovaCtrlApp_otherDeal(void);
void NovaCtrlApp_DebugItemDeal(void);
void NovaCtrlApp_DebugMainDeal(void);
void NovaCtrlApp_Enter(void);
void NovaCtrlApp_Quit(void);
void NovaCtrlApp_QuitDeal(void);
void NovaCtrlApp_ConectAppDeal(void);
void NovaCtrlApp_DebugInfoGet(void);
void NovaCtrlApp_ParaInit(void);
void NovaCtrlApp_DoorCmdCtrl(unsigned short* us_doorCmd);
void NovaCtrlApp_StarDeltaBrakeTest(void);
void NovaCtrlApp_BrakeTest(void);
void NovaCtr1App_UpOverSpeedTest(void);
void NovaCtrlApp_SpeedLimiterTest(enum TEST_DIR e_testDir);
void NovaCtrlApp_BufferTest(enum TEST_DIR e_testDir);
void NovaCtrlApp_BufferDeal(unsigned short* us_TestStep, BOOL* b_bufferTestEn, enum TEST_DIR e_testDir);
void NovaCtrlApp_SnglBrakeStopTest(void);
void NovaCtrlApp_DblBreakStopTest(enum TEST_DIR e_testDir);
void NovaCtrlApp_SnglBreakStopDeal(unsigned short* us_TestStep, BOOL* b_singleBkStopTestEn, enum TEST_DIR e_testSide);
void NovaCtrlApp_DblBreakStopDeal(unsigned short* us_TestStep, BOOL* b_doubleBkStopTestEn, enum TEST_DIR e_testDir);
unsigned long NovaCtrlApp_TriggerConditionCheck(void);
unsigned long NovaCtrlApp_TestConditionCheck(void);
unsigned short NovaCtrlApp_liftSprtStatusGet (void);
unsigned short NovaCtrlApp_DebugPswrdGet(unsigned short us_randomFactor);
unsigned short NovaCtrlApp_ConectRandomCodeGet(void);
unsigned short NovaCtrlApp_DebugConditionCheck(enum DEBUG_CHECK e_type);


//井道自学习-中断测试原因查询
const unsigned short us_shaftStudyTestingCheckList[][FAULT_PRONPT_COL_MUN] =
{
    {8,2},          //紧急电动开关有效
    FAULT_PROMPT_PUBLIC_MODULE_1
};

//井道自学习-无法触发原因查询
const unsigned short us_shaftStudyTrigerCheckListApp[][FAULT_PROMPT_COL_NUA] =
{
    {8,2},         //紧急电动开关有效
    {113,2},       //电梯在运行
    FAULT_PROMPT_PUBLIC_MODULE_1
};


#endif

