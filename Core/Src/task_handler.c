/*
 * task_handler.c
 *
 *  Created on: Aug 20, 2024
 *      Author: andyk
 */


#include "main.h"
#include "command_line.h"
#include "usbd_cdc_if.h"

static const char* msg_inv = "\n Invalid command \n";


void print_help(void);

uint8_t extract_command(command_t *cmd)
{

	QDATA item;
	BaseType_t status;
	status = uxQueueMessagesWaiting(q_data);
	if(!status) return -1;

	//uint8_t i = 0;
	status = xQueueReceive(q_data,&item,0);
	if(status == pdTRUE)
	{
		uint8_t pos = 0;
		uint8_t start = 0;
		uint8_t arg_count = 0;
		char test[10] = {9};


		for(int i = 0; i < item.len; i++)
		{
			if(item.payload[i] == ' ' || item.payload[i] == '\0')
			{
				switch(arg_count)
				{
				case 0:
					cmd->command = get_command(strncpy(test, ((char *) item.payload)+start, pos));
					arg_count++;
					start = pos + 1;
					break;
				case 1:
					cmd->index = get_numeric(strncpy(test, ((char *) item.payload)+start, pos));
					arg_count++;
					start = pos + 1;
					break;

				case 2:
					cmd->subindex = get_numeric(strncpy(test, ((char *) item.payload)+start, pos));
					arg_count++;
					start = pos + 1;
					break;

				case 3:
					cmd->value = get_numeric(strncpy(test, ((char* ) item.payload)+start, pos));
					arg_count++;
					start = pos + 1;
				}
			}
			else
			{
				pos++;
			}

		}
		return 0;
	}
	return 1;
}

void process_command(command_t *cmd)
{
	extract_command(cmd);
	switch(cmd->command)
	{
	case 1 :
		print_help();
		break;
	}
}

uint8_t getnumber(uint8_t *p , int len)
{

	int value ;

	if(len > 1)
	   value =  ( ((p[0]-48) * 10) + (p[1] - 48) );
	else
		value = p[0] - 48;

	return value;

}

void cmd_task_handler(void *parameters)
{
	command_t cmd;
	BaseType_t status;

	while(1)
	{
		status = xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
		if(status == pdTRUE) {
			process_command(&cmd);
		}
	}
}


void print_help()
{
	const uint8_t txt[] = "READ <index> <subindex>\r\n"
	                      "WRITE <index> <subindex> <value>\r\n"
						  "EX READ 0x1000 0\r\n";

	uint8_t txt_len = sizeof(txt);

	QDATA msg;

	//xTaskNotify(print_task, 0, eNoAction);
	memcpy(msg.payload, txt, txt_len);
	msg.len = txt_len;

	//xQueueSend(q_print, &msg, portMAX_DELAY);


	xQueueSend(q_print, &msg, portMAX_DELAY);

}
void menu_task_handler(void *parameters)
{

	//uint32_t cmd_addr;
//	QDATA *cmd;
	//uint8_t option;

	const uint8_t txt[] = "Welcome! To USB to CAN (HELP): \r\n";
	uint8_t txt_len = sizeof(txt);

	QDATA msg;

	   //d.data = (uint8_t*) malloc(sizeof(len));
	memcpy(msg.payload, txt, txt_len);
	msg.len = txt_len;


	//BaseType_t status;
	xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	xTaskNotify(print_task, 0, eNoAction);
	while(1)
	{
		//show_time_date();

		xQueueSend(q_print, &msg, portMAX_DELAY);

		xTaskNotify(cmd_task, 0, eNoAction);
		//wait to run again when some other task notifies
		xTaskNotifyWait(0,0, NULL, portMAX_DELAY);
	}//end of while super loop

}

void print_task_handler(void *parameters)
{

	UBaseType_t cnt;
	QDATA msg;
	xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	while(1)
	{
		xQueueReceive(q_print, &msg, portMAX_DELAY);
		CDC_Transmit_FS(msg.payload, msg.len);

		while(uxQueueMessagesWaiting(q_print) > 0)
		{
			xQueueReceive(q_print, &msg, portMAX_DELAY);
			CDC_Transmit_FS(msg.payload, msg.len);
		}

		//HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg),HAL_MAX_DELAY);
	}

}

void rtc_task_handler(void *parameters)
{
	const char* msg_rtc1 = "========================\n"
							"|         RTC          |\n"
							"========================\n";

	const char* msg_rtc2 = "Configure Time            ----> 0\n"
							"Configure Date            ----> 1\n"
							"Enable reporting          ----> 2\n"
							"Exit                      ----> 3\n"
							"Enter your choice here : ";


	const char *msg_rtc_hh = "Enter hour(1-12):";
	const char *msg_rtc_mm = "Enter minutes(0-59):";
	const char *msg_rtc_ss = "Enter seconds(0-59):";

	const char *msg_rtc_dd  = "Enter date(1-31):";
	const char *msg_rtc_mo  ="Enter month(1-12):";
	const char *msg_rtc_dow  = "Enter day(1-7 sun:1):";
	const char *msg_rtc_yr  = "Enter year(0-99):";

	const char *msg_conf = "Configuration successful\n";
	const char *msg_rtc_report = "Enable time&date reporting(y/n)?: ";


	uint32_t cmd_addr;
	QDATA *cmd;

	static int rtc_state = 0;
	int menu_code;

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;


#define HH_CONFIG 		0
#define MM_CONFIG 		1
#define SS_CONFIG 		2

#define DATE_CONFIG 	0
#define MONTH_CONFIG 	1
#define YEAR_CONFIG 	2
#define DAY_CONFIG 		3


	while(1){
		/*Notify wait (wait till someone notifies) */
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);

		/*Print the menu and show current date and time information */
		xQueueSend(q_print,&msg_rtc1,portMAX_DELAY);
		show_time_date();
		xQueueSend(q_print,&msg_rtc2,portMAX_DELAY);


		while(curr_state != sMainMenu){

			/*Wait for command notification (Notify wait) */
			xTaskNotifyWait(0,0,&cmd_addr,portMAX_DELAY);
			cmd = (QDATA*)cmd_addr;

			switch(curr_state)
			{
				case sRtcMenu:{
					/*process RTC menu commands */
					if(cmd->len == 1)
					{
						menu_code = cmd->payload[0] - 48;
						switch(menu_code)
						{
						case 0:
							curr_state = sRtcTimeConfig;
							xQueueSend(q_print,&msg_rtc_hh,portMAX_DELAY);
							break;
						case 1:
							curr_state = sRtcDateConfig;
							xQueueSend(q_print,&msg_rtc_dd,portMAX_DELAY);
							break;
						case 2 :
							curr_state = sRtcReport;
							xQueueSend(q_print,&msg_rtc_report,portMAX_DELAY);
							break;
						case 3 :
							curr_state = sMainMenu;
							break;
						default:
							curr_state = sMainMenu;
							xQueueSend(q_print,&msg_inv,portMAX_DELAY);
						}

					}else{
						curr_state = sMainMenu;
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);
					}
					break;}

				case sRtcTimeConfig:{
					/*get hh, mm, ss infor and configure RTC */
					/*take care of invalid entries */
					switch(rtc_state)
						{
							case HH_CONFIG:{
								uint8_t hour = getnumber(cmd->payload , cmd->len);
								time.Hours = hour;
								rtc_state = MM_CONFIG;
								xQueueSend(q_print,&msg_rtc_mm,portMAX_DELAY);
								break;}
							case MM_CONFIG:{
								uint8_t min = getnumber(cmd->payload , cmd->len);
								time.Minutes = min;
								rtc_state = SS_CONFIG;
								xQueueSend(q_print,&msg_rtc_ss,portMAX_DELAY);
								break;}
							case SS_CONFIG:{
								uint8_t sec = getnumber(cmd->payload , cmd->len);
								time.Seconds = sec;
								if(!validate_rtc_information(&time,NULL))
								{
									rtc_configure_time(&time);
									xQueueSend(q_print,&msg_conf,portMAX_DELAY);
									show_time_date();
								}else
									xQueueSend(q_print,&msg_inv,portMAX_DELAY);

								curr_state = sMainMenu;
								rtc_state = 0;
								break;}
						}

					break;}

				case sRtcDateConfig:{

					/*get date, month, day , year info and configure RTC */

					/*take care of invalid entries */
					switch(rtc_state)
						{
							case DATE_CONFIG:{
								uint8_t d = getnumber(cmd->payload , cmd->len);
								date.Date = d;
								rtc_state = MONTH_CONFIG;
								xQueueSend(q_print,&msg_rtc_mo,portMAX_DELAY);
								break;}
							case MONTH_CONFIG:{
								uint8_t month = getnumber(cmd->payload , cmd->len);
								date.Month = month;
								rtc_state = DAY_CONFIG;
								xQueueSend(q_print,&msg_rtc_dow,portMAX_DELAY);
								break;}
							case DAY_CONFIG:{
								uint8_t day = getnumber(cmd->payload , cmd->len);
								date.WeekDay = day;
								rtc_state = YEAR_CONFIG;
								xQueueSend(q_print,&msg_rtc_yr,portMAX_DELAY);
								break;}
							case YEAR_CONFIG:{
								uint8_t year = getnumber(cmd->payload , cmd->len);
								date.Year = year;

								if(!validate_rtc_information(NULL,&date))
								{
									rtc_configure_date(&date);
									xQueueSend(q_print,&msg_conf,portMAX_DELAY);
									show_time_date();
								}else
									xQueueSend(q_print,&msg_inv,portMAX_DELAY);

								curr_state = sMainMenu;
								rtc_state = 0;
								break;}
						}


					break;}

				case sRtcReport:{
					/*enable or disable RTC current time reporting over ITM printf */
					if(cmd->len == 1)
					{
						if(cmd->payload[0] == 'y'){
							if(xTimerIsTimerActive(rtc_timer) == pdFALSE)
								xTimerStart(rtc_timer,portMAX_DELAY);
						}else if (cmd->payload[0] == 'n'){
							xTimerStop(rtc_timer,portMAX_DELAY);
						}else{
							xQueueSend(q_print,&msg_inv,portMAX_DELAY);
						}

					}else
					    xQueueSend(q_print,&msg_inv,portMAX_DELAY);

					curr_state = sMainMenu;
					break;}

			}// switch end

		} //while end


		/*Notify menu task */
		xTaskNotify(menu_task,0,eNoAction);

		}//while super loop end
}

