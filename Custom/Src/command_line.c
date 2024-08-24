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


const char * CMD_READ = "READ";
const char * CMD_WRITE = "WRITE";
const char * CMD_HELP = "HELP";
const char * CMD_RTC_MENU = "RTC";
const char * CMD_SYNC = "SYNC";
const char * ARG_SYNC_START = "START";
const char * ARG_SYNC_STOP = "STOP";

void get_command(const char *token, command_t *cmd)
{

	if(strcmp(token,CMD_READ) == 0)
	{
		cmd->command = READ;
		cmd->arg_cnt = 2; //index sub-index
	}
	else if(strcmp(token, CMD_WRITE) == 0)
	{
		cmd->command = WRITE;
		cmd->arg_cnt = 3; //index sub-index value
	}
	else if(strcmp(token, CMD_HELP) == 0)
	{
		cmd->command = HELP;
		cmd->arg_cnt = 0;
	}
	else if (strcmp(token, CMD_RTC_MENU) == 0)
	{
		cmd->command = HELP;
		cmd->arg_cnt = 0; //calls RTC Menu
	}
	else if (strcmp(token, CMD_SYNC) == 0)
	{
		cmd->command = SYNC;
		cmd->arg_cnt = 2;
	}
	else{
		cmd->status = INVALID_COMMAND;
	}
}

void get_sub_command(const char *token, command_t *cmd)
{
	if(cmd->command == SYNC)
	{
		if(strcmp(token,CMD_READ) == 0)
		{
			cmd->sub_command = SYNC_START;
		}
		else if(strcmp(token, CMD_WRITE) == 0)
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
