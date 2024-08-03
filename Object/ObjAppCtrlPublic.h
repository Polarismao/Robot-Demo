#ifndef _PUBLIC_NOVA_APPCTRL_H
#define _PUBLIC_NOVA_APPCTRL_H

extern unsigned long  ul_novaApp_pulseCurrent;
extern unsigned short us_novaApp_timeRndmFctr1;
extern unsigned short us_novaApp_timeRndmFctr2;
extern unsigned short us_novaApp_rightBkPluseRcrd;
extern unsigned short us_novaApp_leftBkPluseRcrd;
extern unsigned short us_novaApp_heartBeatFrm;
extern unsigned short us_novaApp_conectPswrd;
extern unsigned short us_novaApp_debugPswrd;
extern unsigned short us_novaApp_overTime;
extern unsigned short us_novaApp_sprtStatus;
extern unsigned short us_novaApp_debugFuncVer;
extern unsigned short us_novaApp_randomfactor;
extern unsigned short us_novaApp_startStation;
extern unsigned short us_novaAPP_stopTime;
extern unsigned short us_novaApp_barLenth[];
extern unsigned short us_novaApp_lenDoorA[];
extern unsigned short us_novaApp_lenDoorC[];


//调检状态
#define NOT_IN_DEBUG_STATUS        0x00
#define IN_DEBUG_STATUS            0x01
#define DEBUG_STATUS_ENTER_FAIL    0x02

//调检请求命令
#define DEBUG_MODE_REQT_CLEAR      0x00
#define DEBUG_MODE_ENTER           0x01
#define DEBUG_MODE_QUIT            0x62

//调检密码写入状态
#define DEBUG_CODE_NO_RECEIVE      0x00
#define DEBUG_CODE_OVER_TIME       0x01
#define DEBUG_CODE_CORRECT         0x02
#define DEBUG_CODE_WRONG           0x03


#define APPCTRL_MAx(a,b)           ((a)>(b)?(a):(b))
#define APPCTRL_ABS(a)             ((a)>(0)?(a):(-a))
#define FAULT_PROMPT_COL_NUM       2
#define FAULT_PROMIPT_PUBLIC_MODULE_1     {114,2},{118,2},{9,2},{10,2},{100,2},{101,2},
//电梯有不能运行的故障
//电梯在控制面板模式
//轿顶检修开关有效
//底坑检修开关有效
//轿顶检修状态有效
//底坑检修状态有效


enum CHECK_ITEM_TYPE
{
    APP_NULL_TEST = 0,                    //无测试项
    APP_SHAFT_STUDY = 1,                  //井道自学可
    APP_NEN_BALANCE_STUDY = 21,           //平衡系数测试
    APP_BRAKE_TEST = 22,                  //制动力检测
    APP_DOUBLE_BKSTOP_UP_TEST=23,         //曳引能力试验_空载上行
    APP_UCMP_TEST - 24,                   //UCMP试验
    APP_SKEY_STAR_DELTA_TEST =25,         //制动性能试验_其他制动装置（功能）试验
    APP_uP_OVER_SPEED_TEST = 26,          //轿厢上行超速保护装置试验（ACOP)
    APP_WEIGHT_BUFFER_TEST = 27,          //曳引能力试验_对重压缓冲器
    APP_CAR_BUFFER_TEST = 28,             //缓冲器试验_轿厢压缓冲器
    APP_WEIGHING_TEST = 29,               //轿厢超载保护装置试验
    APP_BREAK_STOP_TEST = 30,             //制动性能试验_额载单臂制动
    APP_DOUBLE_BKSTOP_DONN_TEST = 31,     //制动性能试险_125%额载下行制动
    APP_SPEED_LIMITER_Dw_TEST = 34,       //定期检验-空载轿厢限速器安全钳试验
    APP_SPEED_LIMITER_UP_TEST = 35,       //定期检验-空载对重限速器安全钳试验
};
extern enum CHECK_ITEM_TYPE en_novaApp_itemCurnt;

enum TEST_DIR
{
    NULL_TEST=0,
    UP_DIR_TEST,
    DOMN_DIR_TEST,
    RIGHT_SIDE_TEST,   //右臂制停
    LEFT_SIDE_TEST,    //左臂制停
};

enum DEBUG_CHECK
{
    NULL_DEBUG=0,
    INS_DEBUG,
    NORMAL_DEBUG,
};


struct NOVA_APP_CONECT_BIT
{
    unsigned short b_debugstatus:16;         //读取调检状态R, 0x00表示当前不在调检模式, 0x01表示当前是调检状态;
    unsigned short b_conectReqt:1;           //写入连接命令w, 0x01表示请求与主板连接;
    unsigned short b_conectstatus:2;         //读取连接状态R, 0x00表示当前不在连接状态, 0x01表示当前是连接状态;
    unsigned short b_cnPswrdstatus:2;        //读取连接密码写入状态R. 0x00表示没有收到APP的密码, 0x01密码超时, 0x02表示密码正确, 0x03表示密码错误;
    unsigned short b_debugReqt:2;            //写入调检命令w, 0x01进入, 0x02退出;
    unsigned short b_dePswrdstatus:2;        //读取调检密码写入状态R, 0x00表示没有收到APP的密码, 0x01表示随机因子超时, 0x02表示密码正确, 0x03表示密码错误;
    unsigned short b_overTimewrite:1;        //超时时间写入标记
    unsigned short b_rsvd1:6;                //备用

    unsigned short b_intest:1;               //是否在测试状态
    unsigned short b_blncSuc:1;              //平衡系数测试成功
    unsigned short b_blncFail:1;             //平衡系数测试失败
    unsigned short b_ucmpSu:1;               //UCMP测试成功
    unsigned short b_slipSuc:1;              //打滑测试成功
    unsigned short b_slipFail:1;             //打滑测试失败
    unsigned short b_buffersuc:1;            //缓冲器测试成功
    unsigned short b_bufferFail:1;           //缓冲器测试失败
    unsigned short b_starDeltaSuc:1;         //封星制动测试成功
    unsigned short b_starDeltaFail:1;        //封星制动试失败
    unsigned short b_starDeltaStart:1;       //封星制动开始溜车
    unsigned short b_limitSpeedSuc:1;        //限速器安全钳测试成功
    unsigned short b_limitspeedFail:1;       //限速器安全钳测试失败
    unsigned short b_overSpeedSuc:1;         //上行超速测试成功
    unsigned short b_overSpeedFail:1;        //上行超速测试失败
    unsigned short b_overSpeedSuc:1;         //上行超速测试成功
    unsigned short b_overSpeedFail:1;        //上行超速测试失败
    unsigned short b_doubleBkStopSuc:1;      //双臂制停测试成功

    unsigned short b_doubleBkStopFail:1;     //双臂制停测试失败
    unsigned short b_singleBkStopSuc:1;      //单臂制停测试成功
    unsigned short b_singleBkStopFail:1;     //单臂制停测试失败
    unsigned short b_shaftStudySuc:1;        //井道自学习成功
    unsigned short b_shaftstudyFail:1;       //井道自学习失败
    unsigned short b_hbiAddrStudySuc:1;      //外呼地址自学习成功
    unsigned short b_hbiAddrStudyFail:1;     //外呼地址自学习失败
    unsigned short b_rsvd2:9;                //备用
}


union NOVA_APP_CONECT_FLAG
{
    unsigned short all[4];
    struct NOVA_APP_CONECT_BIT bit;
};
extern union NOVA_APP_CONECT_FLAG un_novaApp_deBugFlag;


//调检触发测试寄存器0x9B1D
struct NOVA_APP_DEBUG_w_BIT
{
    unsigned short b_commond:8;       //调检命令
    unsigned short b_itemCurnt:8;     //调检当前测试项
};
union NOVA_APP_DEBUG_w_FLAG
{
    unsigned short all;
    struct NOVA_APP_DEBUG_w_BIT bit;
};
extern union NOVA_APP_DEBUG_w_FLAG un_novaApp_testTrigger;


//调检测试结果读取寄存器0x9B20
struct NOVA_APP_DEBUG_R_BIT
{
    unsigned short b_result:8;         //测试结果
    unsigned short b_itemCurnt:8;      //调检当前测试项
};
union NOVA_APP_DEBUG_R_FLAG
{
    unsigned short all;
    struct NOVA_APP_DEBUG_R BIT bit;
};
extern union NOVA_APP_DEBUG_R_FLAG un_novaApp_resultGet;

//调检写入命令寄存器
struct NOVA_APP_DEBUG_w_RGSTR
{
    unsigned short us_9B1E;
    unsigned short us_9B1F;
};
extern struct NOVA_APP_DEBUG_w_RGSTR un_novaApp_writeRegister;


//调检读取命令寄存器
struct NOVA_APP_DEBUG_R_RGSTR
{
    unsigned short us_errCode;
    unsigned short us_9B22; 
    unsigned short us_9B23;
    unsigned short us_9B24;
    unsigned short us_9B25;
    unsigned short us_9B26;
    unsigned short us_9B27;
    unsigned short us_9B28;
    unsigned short us_9B29;
    unsigned short us_9B2A;
    unsigned short us_9B2B;
    unsigned short us_9B2C;
    unsigned short us_9B2D;
    unsigned short us_9B2E;
    unsigned short us_9B2F;
}
union NOVA_APP_DEBUG_R_RGSTR_FLAG
{
    unsigned short al1[15];
    struct NOVA_APP_DEBUG_R_RGSTR bit;
};
extern union NOVA_APP_DEBUG_R_RGSTR_FLAG un_novaApp_readRegister;


//紧急电动、快车查询
struct NOVA_APP_DEBUG_CONDITION_BIT
{
    unsigned short b_result:8;     //测试结果0x01
    unsigned short b_reason:8;     //不满足的原因
};
union NOVA_APP_DEBUG_CONDITION_FLAG
{
    unsigned short all;
    struct NOVA_APP_DEBUG_CONDITION_BIT bit;
};
extern union NOVA_APP_DEBUG_CONDITION_FLAG un_novaApp_AutoDebugCndition;
extern union NOVA_APP_DEBUG_CONDITION_FLAG un_novaApp_InsDebugCndition;


//紧急电动、快车查询
struct NOVA_APP_DEBUG_CONDITION_BIT
{
    unsigned short b_result:8;    //测试结果0x01
    unsigned short b_reason:8;    //不满足的原因
};
union NOVA_APP_DEBUG_CONDITION_FLAG
{
    unsigned short all;
    struct NOVA_APP_DEBUG_CONDITION_BIT bit;
};
extern union NOVA_APP_DEBUG_CONDITION_FLAG un_novaApp_AutoDebugCndition;
extern union NOVA_APP_DEBUG_CONDITION_FLAG un_novaApp_InsDebugCndition;


//调检写入命令寄存器
struct NOVA_APP_DEBUG_FLAG
{
    BOOL b_shaftStudyTriger;
    BOOL b_hbiAddrStudyTriger;
    BOOL b_newBlncTriger;
    BOOL b_ucmpTriger;
    BOOL b_relayTriger;
    BOOL b_relaySet;
    BOOL b_relayReset;
    BOOL b_bufferTriger;
    BOOL b_starDeltaTriger;
    BOOL b_breakTriger;
    BOOL b_overSpeedTriger;
    BOOL b_breakStopTriger;
    BOOL b_doubleBkStopTriger;
    BOOL b_snglBkStopTriger;
    BoOL b_overSpeedRun;
};
extern struct NOVA_APP_DEBUG_FLAG st_novaApp_flag;



#endif

