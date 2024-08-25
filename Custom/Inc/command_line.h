/*
 * command_line.h
 *
 *  Created on: Aug 21, 2024
 *      Author: andyk
 */

#ifndef INC_COMMAND_LINE_H_
#define INC_COMMAND_LINE_H_

#include "main.h"


typedef struct {
	uint8_t command;
	uint8_t sub_command;
	uint16_t index;
	uint8_t subindex;
	uint32_t value;
	uint8_t subcommand;
	uint8_t payload[Sm_Data_Payload];
	uint8_t len;
	uint8_t status;
	uint8_t exp_arg_cnt;
}command_t;

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
	NO_SUB_COMMAND,
	SYNC_START,
	SYNC_STOP,
} ESCMD;

typedef enum COMMANDS {
	NO_COMMAND,
	HELP,
	READ,
	WRITE,
	RTC_MENU,
	SYNC,
	BAUD
} ECMD ;

void init_command(command_t *);

void get_command(const char *token, command_t *cmd);

void get_sub_command(const char *token, command_t *cmd);

uint32_t get_numeric(const char *token);

uint8_t getnumber(uint8_t *p , int len);

#endif /* INC_COMMAND_LINE_H_ */
