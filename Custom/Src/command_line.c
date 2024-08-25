/*
 * command_line.c
 *
 *  Created on: Aug 21, 2024
 *      Author: andyk
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command_line.h"

const char* CMD_BAUD = "BAUD";
const char* CMD_READ = "READ";
const char* CMD_HELP = "HELP";
const char* CMD_RTC_MENU = "RTC";
const char* CMD_SYNC = "SYNC";
const char* CMD_WRITE = "WRITE";
const char* ARG_SYNC_START = "START";
const char* ARG_SYNC_STOP = "STOP";

void init_command(command_t *cmd)
{
	cmd->command = NO_COMMAND;
	cmd->sub_command = NO_SUB_COMMAND;
	memset(cmd->payload, '\0', Sm_Data_Payload);
	cmd->len = 0;
	cmd->index = 0xfff;
	cmd->subindex = 0xff;
	cmd->status = NO_ERROR;
	cmd->exp_arg_cnt = 0;

}

void get_command(const char *token, command_t *cmd)
{

	if(strcmp(token,CMD_READ) == 0)
	{
		cmd->command = READ;
		cmd->exp_arg_cnt = 2; //index sub-index
	}
	else if(strcmp(token, CMD_WRITE) == 0)
	{
		cmd->command = WRITE;
		cmd->exp_arg_cnt = 3; //index sub-index value
	}
	else if(strcmp(token, CMD_HELP) == 0)
	{
		cmd->command = HELP;
		cmd->exp_arg_cnt = 0;
	}
	else if (strcmp(token, CMD_RTC_MENU) == 0)
	{
		cmd->command = RTC_MENU;
		cmd->exp_arg_cnt = 0; //calls RTC Menu
	}
	else if (strcmp(token, CMD_SYNC) == 0)
	{
		cmd->command = SYNC;
		cmd->exp_arg_cnt = 1;
	}
	else if (strcmp(token, CMD_BAUD) == 0)
	{
		cmd->command = BAUD;
		cmd->exp_arg_cnt = 1;
	}
	else{
		cmd->status = INVALID_COMMAND;
	}
}

void get_sub_command(const char *token, command_t *cmd)
{
	if(cmd->command == SYNC)
	{
		if(strcmp(token,ARG_SYNC_START) == 0)
		{
			cmd->sub_command = SYNC_START;
		}
		else if(strcmp(token, ARG_SYNC_STOP) == 0)
		{
			cmd->sub_command = SYNC_STOP;
		}
		else{
			cmd->status = INVALID_ARG;
		}
	}
	else
	{
		cmd->status = INVALID_ARG;
	}
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

uint8_t getnumber(uint8_t *p , int len)
{
	int value ;

	if(len > 2) // value + '\0'
	   value =  ( ((p[0]-48) * 10) + (p[1] - 48) );
	else
		value = p[0] - 48;

	return value;

}
