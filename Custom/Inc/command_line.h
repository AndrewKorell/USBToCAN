/*
 * command_line.h
 *
 *  Created on: Aug 21, 2024
 *      Author: andyk
 */

#ifndef INC_COMMAND_LINE_H_
#define INC_COMMAND_LINE_H_

#include "main.h"

typedef enum STATUS {
	NO_ERROR,
	INVALID_COMMAND,
	INVALID_ARG,
	TOO_MANY_ARGS,
	TOO_FEW_ARGS,
	UNKNOWN_CO_OBJECT,
	CO_TIMEOUT,
} ESTATUS;

typedef enum SUB_COMMANDS {
	SYNC_START,
	SYNC_STOP,
} ESCMD;

typedef enum COMMANDS {
	HELP,
	READ,
	WRITE,
	RTC_MENU,
	SYNC,
} ECMD ;

void get_command(const char *token, command_t *cmd);

void get_sub_command(const char *token, command_t *cmd);

uint32_t get_numeric(const char *token);

uint8_t getnumber(uint8_t *p , int len);

#endif /* INC_COMMAND_LINE_H_ */
