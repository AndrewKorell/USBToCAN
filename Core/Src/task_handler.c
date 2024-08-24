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
		memcpy(cmd->payload, item.payload, item.len);
		cmd->len = item.len;
	}
	return 0;
}

uint8_t extract_command(command_t *cmd)
{
	QDATA item;
	BaseType_t status;
	status = uxQueueMessagesWaiting(q_data);
	if(!status) return -1;

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
					get_command(strncpy(test, ((char *) item.payload)+start, pos), cmd);
					arg_count++;
					start = pos + 1;
					break;
				case 1:
					if(cmd->command == SYNC)
					{
						cmd->index = get_numeric(strncpy(test, ((char *) item.payload)+start, pos));
					}
					else
					{
						get_sub_command(strncpy(test, ((char *) item.payload)+start, pos), cmd);
					}
					arg_count++;
					start = pos + 1;
					break;

				case 2:
					if(get_command == SYNC)
					{
						cmd->value = get_numeric(strncpy(test, ((char *) item.payload)+start, pos));
					}
					else
					{
						cmd->subindex = get_numeric(strncpy(test, ((char *) item.payload)+start, pos));
					}
					arg_count++;
					start = pos + 1;
					break;

				case 3:
					cmd->value = get_numeric(strncpy(test, ((char* ) item.payload)+start, pos));
					arg_count++;
					start = pos + 1;
					break;
				default :
					cmd->status = 1;
					break;
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
	switch(curr_state)
	{
	case sMainMenu:
		extract_command(cmd);
		switch(cmd->command)
		{
		case 1 :
			print_help();
			break;

		case 2 :
			char read_res[Lg_Data_Payload];
		    uint32_t r_value = 0x12345678;
		    //read_command(cmd) --> read_status
		    sprintf((char*)read_res,"READ 0x%04x %d 0x%08lx %d %lu",cmd->index, cmd->subindex, r_value, cmd->status, (unsigned long) HAL_GetTick());
			send_print_msg(read_res);
			break;

		case 3 :
			char write_res[Lg_Data_Payload];
		    //write_command(cmd) --> write_status
			sprintf((char*)read_res,"WRITE 0x%04x %d 0x%08lx %d %lu",cmd->index, cmd->subindex, cmd->value, cmd->status, (unsigned long) HAL_GetTick());
		    send_print_msg(write_res);
			break;

		case 4 :
			curr_state = sRtcMenu;
			xTaskNotify(rtc_task,0,eNoAction);
			break;

		case 5 :

			break;
		}
		break;
	case sRtcMenu :
	case sRtcTimeConfig :
	case sRtcDateConfig :
	case sRtcReport :
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
	const uint8_t txt[] = "READ <index> <subindex>\r\n"
	                      "WRITE <index> <subindex> <value>\r\n"
						  "RTC\r\n"
					      "-------------------------------- \r\n";

	uint8_t txt_len = sizeof(txt);

	QDATA msg;

	//xTaskNotify(print_task, 0, eNoAction);
	memcpy(msg.payload, txt, txt_len);
	msg.len = txt_len;

	xQueueSend(q_print, (void *) &msg, portMAX_DELAY);

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

	//UBaseType_t cnt;
	QDATA msg;
	//const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
	uint8_t timeout;
	uint8_t result;
	//BaseType_t status;

	xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	while(1)
	{
		timeout = 0;
		xQueueReceive(q_print, (void *) &msg, portMAX_DELAY);
		do
		{
			result = CDC_Transmit_FS(msg.payload, msg.len);
			//vTaskDelay(xDelay);
			timeout++;
		}while(result != USBD_OK && timeout < 5);

		//HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg),HAL_MAX_DELAY);
	}
}


void fill_queue(const uint8_t *msg, uint8_t len, QDATA * qdata)
{
	memcpy(qdata->payload, msg, len);
	qdata->len = len;
}

void send_print_msg(const char* msg)
{
	static QDATA item;
	uint8_t b_size = sizeof(msg);
	uint8_t len = b_size;
	while(b_size > 0)
	{
		if(b_size > (uint8_t) Lg_Data_Payload)
		{
			len = b_size;
			b_size = 0;
		}
		else
		{
			len = (uint8_t) Lg_Data_Payload;
			b_size = b_size - (uint8_t) Lg_Data_Payload;
		}

		memcpy(item.payload, msg, len);
		item.len = len;

		xQueueSendToBack(q_print, (void *) &item, portMAX_DELAY);
	}
}

void rtc_task_handler(void *parameters)
{
	const TickType_t xDelay = 200 / portTICK_PERIOD_MS;

	const uint8_t msg_rtc1[] = 	" RTC  Menu  \r\n";

	QDATA rtc1;
	fill_queue(msg_rtc1, sizeof(msg_rtc1), &rtc1);


	const uint8_t msg_rtc2[] = "Set Time            0\r\n"
							   "Set Date            1\r\n"
							   "Exit                2\r\n"
							   "Enter your choice here : ";

	QDATA rtc2;
	fill_queue(msg_rtc2, sizeof(msg_rtc2), &rtc2);

	QDATA qinvalid;
	uint8_t *temp;
	temp = (uint8_t *) msg_inv;
	fill_queue(temp, sizeof(temp), &qinvalid);


	const uint8_t msg_rtc_hh[] = "Enter hour(1-12):";
	const uint8_t msg_rtc_mm[] = "Enter minutes(0-59):";
	const uint8_t msg_rtc_ss[] = "Enter seconds(0-59):";

	const uint8_t msg_rtc_dd[]  = "Enter date(1-31):";
	const uint8_t msg_rtc_mo[]  ="Enter month(1-12):";
	const uint8_t msg_rtc_dow[]  = "Enter day(1-7 sun:1):";
	const uint8_t msg_rtc_yr[]  = "Enter year(0-99):";

	const uint8_t msg_conf[] = "Configuration successful\n";
	//const uint8_t msg_rtc_report[] = "Enable time&date reporting(y/n)?: ";

	QDATA qrtc_hh;
	QDATA qrtc_mm;
	QDATA qrtc_ss;
	QDATA qrtc_dd;
	QDATA qrtc_mo;
	QDATA qrtc_dow;
	QDATA qrtc_yr;
	QDATA qrtc_conf;
	//QDATA qrtc_report;

	fill_queue(msg_rtc_hh, sizeof(msg_rtc_hh), &qrtc_hh);
	fill_queue(msg_rtc_mm, sizeof(msg_rtc_mm), &qrtc_mm);
	fill_queue(msg_rtc_ss, sizeof(msg_rtc_ss), &qrtc_ss);
	fill_queue(msg_rtc_dd, sizeof(msg_rtc_dd), &qrtc_dd);
	fill_queue(msg_rtc_mo, sizeof(msg_rtc_mo), &qrtc_mo);
	fill_queue(msg_rtc_dow, sizeof(msg_rtc_dow), &qrtc_dow);
	fill_queue(msg_rtc_yr, sizeof(msg_rtc_yr), &qrtc_yr);
	fill_queue(msg_conf, sizeof(msg_conf), &qrtc_conf);
	//fill_queue(msg_rtc_report, sizeof(msg_rtc_report), &qrtc_report);


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
		xQueueSendToBack(q_print, (void *) &rtc1,portMAX_DELAY);
		//show_time_date();
		vTaskDelay(xDelay);

		show_time_date();

		vTaskDelay(xDelay);

		xQueueSendToBack(q_print, (void *) &rtc2,portMAX_DELAY);

		//vTaskDelay(xDelay);

		while(curr_state != sMainMenu){

			//xTaskNotify(cmd_task, 0, eNoAction);
			/*Wait for command notification (Notify wait) */

			/*Wait for command notification (Notify wait) */
			xTaskNotifyWait(0,0,&cmd_addr,portMAX_DELAY);
			cmd = (command_t*)cmd_addr;


			switch(curr_state)
			{
				case sRtcMenu:{
					/*process RTC menu commands */
					if(cmd->len == 2)
					{
						menu_code = cmd->payload[0] - 48;
						switch(menu_code)
						{
						case 0:
							curr_state = sRtcTimeConfig;
							xQueueSend(q_print,&qrtc_hh,portMAX_DELAY);
							break;
						case 1:
							curr_state = sRtcDateConfig;
							xQueueSend(q_print,&qrtc_dd,portMAX_DELAY);
							break;
//						case 2 :
//							curr_state = sRtcReport;
//							xQueueSend(q_print,&qrtc_report,portMAX_DELAY);
//							break;
						case 2 :
							curr_state = sMainMenu;
							break;
						default:
							curr_state = sMainMenu;
							xQueueSend(q_print, (void *) &qinvalid,portMAX_DELAY);
						}

					}else{
						curr_state = sMainMenu;

						xQueueSend(q_print, (void *) &qinvalid,portMAX_DELAY);
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
								xQueueSend(q_print,&qrtc_mm,portMAX_DELAY);
								break;}
							case MM_CONFIG:{
								uint8_t min = getnumber(cmd->payload , cmd->len);
								time.Minutes = min;
								rtc_state = SS_CONFIG;
								xQueueSend(q_print,&qrtc_ss,portMAX_DELAY);
								break;}
							case SS_CONFIG:{
								uint8_t sec = getnumber(cmd->payload , cmd->len);
								time.Seconds = sec;
								if(!validate_rtc_information(&time,NULL))
								{
									rtc_configure_time(&time);
									xQueueSend(q_print,&qrtc_conf,portMAX_DELAY);
									show_time_date();
								}else
									xQueueSend(q_print, (void *) &qinvalid,portMAX_DELAY);

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
								xQueueSend(q_print,&qrtc_mo,portMAX_DELAY);
								break;}
							case MONTH_CONFIG:{
								uint8_t month = getnumber(cmd->payload , cmd->len);
								date.Month = month;
								rtc_state = DAY_CONFIG;
								xQueueSend(q_print,&qrtc_dow,portMAX_DELAY);
								break;}
							case DAY_CONFIG:{
								uint8_t day = getnumber(cmd->payload , cmd->len);
								date.WeekDay = day;
								rtc_state = YEAR_CONFIG;
								xQueueSend(q_print,&qrtc_yr,portMAX_DELAY);
								break;}
							case YEAR_CONFIG:{
								uint8_t year = getnumber(cmd->payload , cmd->len);
								date.Year = year;

								if(!validate_rtc_information(NULL,&date))
								{
									rtc_configure_date(&date);
									xQueueSend(q_print,&qrtc_conf,portMAX_DELAY);
									show_time_date();
								}else
									xQueueSend(q_print,&qinvalid,portMAX_DELAY);

								curr_state = sMainMenu;
								rtc_state = 0;
								break;}
						}


					break;}

//				case sRtcReport:{
//					/*enable or disable RTC current time reporting over ITM printf */
//					if(cmd->len == 1)
//					{
//						if(cmd->payload[0] == 'y'){
//							if(xTimerIsTimerActive(rtc_timer) == pdFALSE)
//								xTimerStart(rtc_timer,portMAX_DELAY);
//						}else if (cmd->payload[0] == 'n'){
//							xTimerStop(rtc_timer,portMAX_DELAY);
//						}else{
//							xQueueSend(q_print, (void *) &qinvalid,portMAX_DELAY);
//						}
//
//					}else
//					    xQueueSend(q_print, (void *) &qinvalid,portMAX_DELAY);
//
//					curr_state = sMainMenu;
//					break;}
//
			}// switch end

		} //while end


		/*Notify menu task */
		xTaskNotify(menu_task,0,eNoAction);

		}//while super loop end
}

