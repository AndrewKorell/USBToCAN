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


const char * READ = "READ";
const char * WRITE = "WRITE";
const char * HELP = "HELP";
const char * RTC = "RTC";

uint8_t get_command(const char *token)
{

	if(strcmp(token,READ) == 0)
	{
		return 2;
	}
	else if(strcmp(token, WRITE) == 0)
	{
		return 3;
	}
	else if(strcmp(token, HELP) == 0)
	{
		return 1;
	}
	else if (strcmp(token, RTC) == 0)
	{
		return 4;
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


