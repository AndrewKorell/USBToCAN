/*
 * task_handler.c
 *
 *  Created on: Aug 20, 2024
 *      Author: andyk
 */


#include "main.h"
#include "usbd_cdc_if.h"

static const char* msg_inv = "\n Invalid command \n";

typedef struct {
	uint8_t command;
	uint16_t index;
	uint8_t subindex;
	uint32_t value;
}tcommand_t;

uint8_t extract_command(command_t *cmd)
{

	uint8_t item;
	BaseType_t status;

	status = uxQueueMessagesWaiting(q_data);
	if(!status) return -1;

	uint8_t i = 0;

	do
	{
		status = xQueueReceive(q_data,&item,0);
		if(status == pdTRUE) cmd->payload[i++] = item;
	}while(item != '\n');

	cmd->payload[i-1] = '\0';
	cmd->len = i-1;

	return 0;
}

void process_command(command_t *cmd)
{
	extract_command(cmd);

	switch(curr_state)
	{
		case sMainMenu :
			xTaskNotify(menu_task, (uint32_t) cmd, eSetValueWithOverwrite);
			break;

		//case sLedEffect :
		//	xTaskNotify(led_task, (uint32_t) cmd, eSetValueWithOverwrite);
		//	break;

		case sRtcMenu :
		case sRtcTimeConfig :
		case sRtcDateConfig :
		case sRtcReport :
			xTaskNotify(rtc_task, (uint32_t) cmd, eSetValueWithOverwrite);
			break;
		default:
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

uint8_t get_command(const char *token)
{
	if(strcmp(token,"READ"))
	{
		return 1;
	}
	else if(strcmp(token, "WRITE"))
	{
		return 2;
	}
	return 0;
}


uint32_t get_numeric(const char *token)
{
	char *endptr;
	if(strstr(token, "0x") != NULL)
	{
		return strtol(token, &endptr, 16);
	}
	else
	{
		return strtol(token, &endptr, 10);
	}
}

void cmd_task_handler(MessageBufferHandle_t xMessageBuffer)
{
	tcommand_t cmd;
	uint8_t xData[2048];
	size_t xReceivedBytes;
	const char *delimiter = "\n";
	uint8_t status;
	uint8_t arg_count;

	//HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
	while(1)
	{
		arg_count = 0;
		char *token;
		char *saveptr;

		xReceivedBytes = xMessageBufferReceive(xMessageBuffer, (void *) xData, sizeof(xData), portMAX_DELAY);
		if(xReceivedBytes > 0)
		{
			token = strtok_r((char *)xData, delimiter, &saveptr);
			while(token != NULL)
			{
				switch(arg_count++)
				{
					case 0:
						cmd.command = get_command(token);
						if(cmd.command == 0)
						{
							status = 1;
						}
						break;
					case 1:
						cmd.index = get_numeric(token);
						break;
					case 2:
						cmd.subindex = get_numeric(token);
						break;
					case 3:
						cmd.value = get_numeric(token);
						break;
					default :
						status = 2;
						break;
				}
				token = strtok_r(NULL, delimiter, &saveptr);
			}
		}
		else
		{
			status = 3;
		}

		if(status == 0)
		{
			/* perform action  */
		}
		else
		{
			/* send message to user that there is an error */
		}
	}
}

void menu_task_handler(void *parameters)
{

	uint32_t cmd_addr;
	command_t *cmd;
	uint8_t option;
	const char* msg_menu = 	"======================\n"
							"|         Menu        |\n"
							"======================\n"
							"LED effect -------> 0\n"
							"Date and time ----> 1\n"
							"Exit -------------> 2\n"
							"Enter your choice:  ";

	//BaseType_t status;
	xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	while(1)
	{
		xQueueSend(q_print, &msg_menu, portMAX_DELAY);
		xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);

		cmd = (command_t*) cmd_addr;
		if(cmd->len == 1)
		{
			option = cmd->payload[0] - 48;

			switch(option)
			{
//			case 0:
//				curr_state = sLedEffect;
//				xTaskNotify(led_task, 0, eNoAction);
//				break;

			case 1:
				curr_state = sRtcMenu;
				xTaskNotify(rtc_task, 0, eNoAction);
				break;

			case 2:
				break;

			default:
				xQueueSend(q_print, &msg_inv, portMAX_DELAY);
				continue;
			}
		}
		else
		{
			xQueueSend(q_print, &msg_inv, portMAX_DELAY);
			continue;
		}

		//wait to run again when some other task notifies
		xTaskNotifyWait(0,0, NULL, portMAX_DELAY);
	}//end of while super loop

}

void print_task_handler(void *parameters)
{

	uint32_t *msg;
	while(1)
	{
		xQueueReceive(q_print, &msg, portMAX_DELAY);
		CDC_Transmit_FS((uint8_t *)&msg, strlen((char*)msg));

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
		xQueueSend(q_print,&msg_rtc1,portMAX_DELAY);
		show_time_date();
		xQueueSend(q_print,&msg_rtc2,portMAX_DELAY);


		while(curr_state != sMainMenu){

			/*Wait for command notification (Notify wait) */
			xTaskNotifyWait(0,0,&cmd_addr,portMAX_DELAY);
			cmd = (command_t*)cmd_addr;

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

void UsbRxTaskHandler(MessageBufferHandle_t xMessageBuffer)
{
	uint8_t xData[2048];
	uint8_t message[255];
	uint8_t message_len;
	size_t xReceivedBytes;
	size_t xBytesSent;
	//const TickType_t xBlockTime = pdMS_TO_TICKS(1000);

	xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
	while(1)
	{
		xReceivedBytes = xMessageBufferReceive(xMessageBuffer, (void *) xData, sizeof(xData), portMAX_DELAY);
		//if(xQueueReceive(xQueueUsbCdcRx, &( xData ), ( TickType_t ) 10) == pdPASS)
		if(xReceivedBytes > 0)
		{
			for(int i = 0; i < xReceivedBytes; i++)
			{
				message[message_len++] = xData[i];

				if(xData[i] == '\n')
				{
					  xBytesSent = xMessageBufferSend(xCommandBuffer, (void * ) &message, message_len, portMAX_DELAY);
					  if(xBytesSent != message_len)
					  {
						  /*  Todo: error log */
					  }
					//xTaskNotify(cmd_task, 0, eNoAction);
				}

			}
			//CDC_Transmit_FS(xData, xReceivedBytes);
		}
	}
}

