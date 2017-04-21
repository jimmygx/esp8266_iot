/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2015/1/23, v1.0 create this file.
*******************************************************************************/

#include "osapi.h"
#include "at_custom.h"
#include "user_interface.h"

#include "ets_sys.h"
#include "os_type.h"
#include "mem.h"
#include "espconn.h"

#include "uart.h"
#include "airkiss_app.h"
#include "my_cJSON.h"


os_event_t Taskqueue;
os_event_t TaskWifiQueue;
os_timer_t checkStationConnTimer;

void ICACHE_FLASH_ATTR
user_strcpy(uint8 *src,uint8 *des)
{
  while(*des)
  {
    *src++=*des++;
  }
}

void init_done_cb(void)
{
	os_printf("\r\ncompile time:%s %s\r\n", __DATE__, __TIME__);
	os_printf("SDK Version %s\r\n", system_get_sdk_version());

	//platform_init();

}

void user_rf_pre_init(void)
{

}

void ICACHE_FLASH_ATTR
Check_StationState(void)//查询wifi状态
{
  uint8 getState;
  getState = wifi_station_get_connect_status();
  uart0_tx_SendStr("\r\n station state:");
  uart0_tx_SendNum(getState);
  if(getState == STATION_GOT_IP)//获取到IP
  {
    os_timer_disarm(&checkStationConnTimer);//关闭查询wifi定时器
    struct ip_info ipinfo;
    wifi_get_ip_info(STATION_IF,&ipinfo);
    uart0_tx_SendStr("\r\nIP:");
    uart0_tx_SendNum(ipinfo.ip.addr&0xFF);
    uart0_tx_SendStr(".");
    uart0_tx_SendNum((ipinfo.ip.addr>>8)&0xFF);
    uart0_tx_SendStr(".");
    uart0_tx_SendNum((ipinfo.ip.addr>>16)&0xFF);
    uart0_tx_SendStr(".");
    uart0_tx_SendNum(ipinfo.ip.addr>>24);
    uart0_tx_SendStr("\r\n");
	//system_os_post(0,0,0);  // 使能远程连接
  }
}

void ICACHE_FLASH_ATTR
Init_InterNet(void)
{
	uart0_tx_SendStr("333333333333\r\n");
  struct station_config stationConf;
  os_bzero(&stationConf, sizeof(struct station_config));
  wifi_set_opmode(STATION_MODE);
  //wifi_station_get_config(&stationConf);
  user_strcpy(stationConf.ssid,"wanrong1");
  user_strcpy(stationConf.password,"wanrong6");
  wifi_station_disconnect();
  ETS_UART_INTR_DISABLE();
  wifi_station_set_config(&stationConf);
  ETS_UART_INTR_ENABLE();
  wifi_station_connect();
}

void ICACHE_FLASH_ATTR
wifiControl(os_event_t *event)
{
	uart0_tx_SendStr("22222222222\r\n");
  Init_InterNet();  //开始连接路由器
  os_timer_disarm(&checkStationConnTimer);
  os_timer_setfn(&checkStationConnTimer,(os_timer_func_t *)Check_StationState,NULL);
  os_timer_arm(&checkStationConnTimer,1500,1);//每隔1500毫秒查询wifi状态
}

void user_init(void)
{
    /*UART_SetPrintPort(0);
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

	wifi_set_opmode(STATION_MODE);

	system_init_done_cb(init_done_cb);*/

	system_timer_reinit(); //重新初始化定时器
		os_printf("SDK version:%s\n", system_get_sdk_version());//查询SDK版本信息
		gpio16_output_conf();
		//PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);//D5
		UART_SetPrintPort(0);
		uart_init(BIT_RATE_115200,BIT_RATE_115200);
		uart0_tx_SendStr("\r\nI'm start!!\r\n");
		system_os_task(wifiControl,1,&TaskWifiQueue,1);
		system_os_post(1,0,0); //向任务发送消息，开始连接路由器
		uart0_tx_SendStr("1111111111111\r\n");
}

