/*
 * task_handler.c
 *
 *  Created on: Aug 20, 2024
 *      Author: andyk
 */


#include "main.h"
#include "command_line.h"
#include "usbd_cdc_if.h"

static const char* msg_inv = "\r\n Invalid command \r\n";

void print_help(void);
void send_print_msg(const char* msg);

uint8_t extract_command_rtc(command_t *cmd)
{
	QDATA item;
	BaseType_t status;

	status = uxQueueMessagesWaiting(q_data);
	if(!status) return -1;

	status = xQueueReceive(q_data,(void *) &item,0);
	if(status == pdTRUE) {
		cmd->index = 0;
		cmd->subindex = 0;
		cmd->command = 4;
		cmd->status = 0;
		memcpy(cmd->queue_raw.payload, item.payload, item.len);
		cmd->queue_raw.len = item.len;
	}
	return 0;
}

uint8_t extract_command(command_t *cmd)
{
	init_command(cmd);
	QDATA item;
	BaseType_t status;
	status = uxQueueMessagesWaiting(q_data);
	if(!status) return -1;

	status = xQueueReceive(q_data,&item,0);
	if(status == pdTRUE)
	{
		uint8_t seg_cnt = 0;
		uint8_t len = 0;
		uint8_t start = 0;
		uint8_t arg_cnt = 0;
		char test[10] = {0};

		for(int i = 0; i < item.len; i++)
		{
			if(item.payload[i] == ' ' || item.payload[i] == '\0')
			{
				switch(seg_cnt)
				{
				case 0:
					get_command(strncpy(test, ((char *) item.payload)+start, len), cmd);
					break;
				case 1:
					arg_cnt++;
					if(cmd->command == SYNC)
					{
						get_sub_command(strncpy(test, ((char *) item.payload)+start, len), cmd);
					}
					else if(cmd->command == BAUD)
					{
						cmd->value = get_numeric(strncpy(test, ((char *) item.payload)+start, len));
					}
					else
					{
						cmd->node_id = get_numeric(strncpy(test, ((char *) item.payload)+start, len));
					}
					break;

				case 2:
					arg_cnt++;
					if(cmd->command == SYNC)
					{
						cmd->value = get_numeric(strncpy(test, ((char *) item.payload)+start, len));
					}
					else
					{
						cmd->index = get_numeric(strncpy(test, ((char *) item.payload)+start, len));
					}
					break;

				case 3:
					arg_cnt++;
					cmd->subindex = get_numeric(strncpy(test, ((char* ) item.payload)+start, len));
					break;

				case 4:
					arg_cnt++;
					cmd->value = get_numeric(strncpy(test, ((char* ) item.payload)+start, len));
					break;
				default :
					cmd->status = INVALID_ARG;
					break;
				}
				seg_cnt++;
				start = start + len + 1;
				len = 0;
				memset(test, '\0', 10);
			}
			else
			{
				len++;
			}
		}

		if(arg_cnt < cmd->exp_arg_cnt)
		{
			cmd->status = TOO_FEW_ARGS;
		}
		else if(arg_cnt > cmd->exp_arg_cnt)
		{
			cmd->status = TOO_MANY_ARGS;
		}
		return 0;
	}
	return 1;
}

void process_command(command_t *cmd)
{
	char response[Lg_Data_Payload];
	memset(response, '\0', Lg_Data_Payload);

	switch(curr_state)
	{
	case sMainMenu:

		extract_command(cmd);
		switch(cmd->command)
		{
		case HELP :
			print_help();
			break;

		case READ :
		    uint32_t r_value = 0x12345678;
		    sprintf((char*)response,"\r\nREAD %d 0x%04x %d 0x%08lx %d %lu\r\n",cmd->index, cmd->node_id, cmd->subindex, r_value, cmd->status, (unsigned long) HAL_GetTick());
			send_print_msg(response);
			break;

		case WRITE :
			sprintf((char*)response,"\r\nWRITE %d 0x%04x %d 0x%08lx %d %lu\r\n",cmd->index, cmd->node_id, cmd->subindex, (unsigned long) cmd->value, cmd->status, (unsigned long) HAL_GetTick());
		    send_print_msg(response);
			break;

		case RTC_MENU :
			curr_state = sRtcMenu;
			xTaskNotify(rtc_task,0,eNoAction);
			break;

		case SYNC :
			if(cmd->sub_command == SYNC_START)
			{
				sprintf((char*)response,"\r\nSYNC START %lu %d %lu\r\n", (unsigned long) cmd->value, cmd->status, (unsigned long) HAL_GetTick());
			}
			else if(cmd->sub_command == SYNC_STOP)
			{
				sprintf((char*)response,"\r\nSYNC STOP %d %lu\r\n", cmd->status, (unsigned long) HAL_GetTick());
			}
			else
			{
				cmd->status = INVALID_SUB_COMMAND;
				sprintf((char*)response,"\r\nSYNC ERROR %d %lu\r\n", cmd->status, (unsigned long) HAL_GetTick());
			}
		    send_print_msg(response);
			break;

		case BAUD :
			sprintf((char*)response,"\r\rBAUD %lu %d %lu\r\n", (unsigned long) cmd->value, cmd->status, (unsigned long) HAL_GetTick());
		    send_print_msg(response);
			break;
		}
		break;
	case sRtcMenu :
	case sRtcTimeConfig :
	case sRtcDateConfig :
			extract_command_rtc(cmd);
			xTaskNotify(rtc_task, (uint32_t) cmd, eSetValueWithOverwrite);
			break;
		default:
			break;
	}
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
	const char* help_text  =  "\r\n\r\nCommands to get started...\r\n"
						   	  "-------------------------------- \r\n"
			                  "READ <node_id> <index> <sub-index>\r\n"
	                          "WRITE <node_id> <index> <sub-index> <value>\r\n"
							  "BAUD <bits per second>\r\n"
							  "SYNC START <time_ms>\r\n"
			                  "SYNC STOP\r\n"
						      "RTC\r\n"
							  "-------------------------------- \r\n";
	send_print_msg(help_text);
}

void menu_task_handler(void *parameters)
{
	const uint8_t txt[] = "\r\nWelcome! To USB to CAN (HELP): \r\n";
	uint8_t txt_len = sizeof(txt);

	QDATA msg;

	memcpy(msg.payload, txt, txt_len);
	msg.len = txt_len;

	xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	xTaskNotify(print_task, 0, eNoAction);
	while(1)
	{
		show_time_date();

		xQueueSend(q_print, (void *) &msg, portMAX_DELAY);

		//wait to run again when some other task notifies
		xTaskNotifyWait(0,0, NULL, portMAX_DELAY);
	}//end of while super loop
}

void print_task_handler(void *parameters)
{
	QDATA msg;
	const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
	uint8_t result;

	xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	while(1)
	{
		do
		{
			xQueueReceive(q_print, (void *) &msg, portMAX_DELAY);
			result = CDC_Transmit_FS(msg.payload, msg.len);
			if(result != USBD_OK) {
				vTaskDelay(xDelay);
				CDC_Transmit_FS(msg.payload, msg.len);
			}

		}while(uxQueueMessagesWaiting(q_print) == pdTRUE);
	}
}

void send_print_msg(const char* msg)
{
	static QDATA item;
	const uint8_t b_size = strlen(msg);
	uint8_t len = b_size;
	uint8_t start = 0;
	char send[Lg_Data_Payload] = {0};

	while(start < b_size)
	{
		if((b_size - start) < (uint8_t) Lg_Data_Payload)
		{
			len = b_size - start;
		}
		else
		{
			len = (uint8_t) Lg_Data_Payload;
		}

		strncpy(send, msg + start, len);
		start = start + len;

		memcpy(item.payload, send, len);
		item.len = len;

		//TODO: can be xQueueSend
		xQueueSendToBack(q_print, (void *) &item, portMAX_DELAY);
	}
}

void rtc_task_handler(void *parameters)
{

	const char* msg_rtc1 = 	"\r\nRTC  Menu  \r\n";

	const char* msg_rtc2 = "Set Time\t\t0\r\n"
							   "Set Date\t\t1\r\n"
							   "Exit\t\t2\r\n"
							   "Enter here : ";

	const char* msg_rtc_hh = "\r\nEnter hour(1-24):";
	const char* msg_rtc_mm = "\r\nEnter minutes(0-59):";
	const char* msg_rtc_ss = "\r\nEnter seconds(0-59):";

	const char* msg_rtc_dd  = "\r\nEnter date(1-31):";
	const char*  msg_rtc_mo  ="\r\nEnter month(1-12):";
	const char* msg_rtc_dow  = "\r\nEnter day(1-7 sun:1):";
	const char* msg_rtc_yr  = "\r\nEnter year(0-99):";

	const char* msg_rtc_conf = "Configuration successful\n";

	uint32_t cmd_addr;
	command_t *cmd;

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
		send_print_msg(msg_rtc1);

		show_time_date();

		send_print_msg(msg_rtc2);

		while(curr_state != sMainMenu){

			xTaskNotifyWait(0,0,&cmd_addr,portMAX_DELAY);
			cmd = (command_t*)cmd_addr;

			switch(curr_state)
			{
				case sRtcMenu:{
					/*process RTC menu commands */
					if(cmd->queue_raw.len == 2)
					{
						menu_code = cmd->queue_raw.payload[0] - 48;
						switch(menu_code)
						{
						case 0:
							curr_state = sRtcTimeConfig;
							send_print_msg(msg_rtc_hh);
							break;
						case 1:
							curr_state = sRtcDateConfig;
							send_print_msg(msg_rtc_dd);
							break;
						case 2 :
							curr_state = sMainMenu;
							break;
						default:
							curr_state = sMainMenu;
							send_print_msg(msg_inv);
						}

					}else{
						curr_state = sMainMenu;
						send_print_msg(msg_inv);
					}
					break;}

				case sRtcTimeConfig:{
					/*get hh, mm, ss infor and configure RTC */
					/*take care of invalid entries */
					switch(rtc_state)
						{
							case HH_CONFIG:{
								uint8_t hour = getnumber(cmd->queue_raw.payload , cmd->queue_raw.len);
								time.Hours = hour;
								rtc_state = MM_CONFIG;
								send_print_msg(msg_rtc_mm);
								break;}
							case MM_CONFIG:{
								uint8_t min = getnumber(cmd->queue_raw.payload , cmd->queue_raw.len);
								time.Minutes = min;
								rtc_state = SS_CONFIG;
								send_print_msg(msg_rtc_ss);
								break;}
							case SS_CONFIG:{
								uint8_t sec = getnumber(cmd->queue_raw.payload , cmd->queue_raw.len);
								time.Seconds = sec;
								if(!validate_rtc_information(&time,NULL))
								{
									rtc_configure_time(&time);
									send_print_msg(msg_rtc_conf);
									show_time_date();
								}else
									send_print_msg(msg_inv);

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
								uint8_t d = getnumber(cmd->queue_raw.payload , cmd->queue_raw.len);
								date.Date = d;
								rtc_state = MONTH_CONFIG;
								send_print_msg(msg_rtc_mo);
								break;}
							case MONTH_CONFIG:{
								uint8_t month = getnumber(cmd->queue_raw.payload , cmd->queue_raw.len);
								date.Month = month;
								rtc_state = DAY_CONFIG;
								send_print_msg(msg_rtc_dow);
								break;}
							case DAY_CONFIG:{
								uint8_t day = getnumber(cmd->queue_raw.payload , cmd->queue_raw.len);
								date.WeekDay = day;
								rtc_state = YEAR_CONFIG;
								send_print_msg(msg_rtc_yr);
								break;}
							case YEAR_CONFIG:{
								uint8_t year = getnumber(cmd->queue_raw.payload , cmd->queue_raw.len);
								date.Year = year;

								if(!validate_rtc_information(NULL,&date))
								{
									rtc_configure_date(&date);
									send_print_msg(msg_rtc_conf);
									show_time_date();
								}else
									send_print_msg(msg_inv);

								curr_state = sMainMenu;
								rtc_state = 0;
								break;}
						}


					break;}
				default:
					send_print_msg(msg_inv);

			}// switch end

		} //while end


		/*Notify menu task */
		xTaskNotify(menu_task,0,eNoAction);

		}//while super loop end
}

