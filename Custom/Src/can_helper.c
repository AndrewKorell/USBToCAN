/*
 * canhelper.c
 *
 *  Created on: Aug 28, 2024
 *      Author: andyk
 */
#include "stm32f4xx_hal.h"
#include "can_helper.h"
#include "can_open.h"

extern CAN_HandleTypeDef hcan1;


void cmd_to_data_bytes(command_t *cmd, uint8_t sdo_command, uint8_t data[8])
{

	//TODO: Sanity check byte order of index
	data[smd_cmd_offset] = sdo_command;
	data[index_offset] = (cmd->index >> 8) & 0xff;
	data[index_offset - 1] = cmd->index & 0xff;
	data[index_offset - 2] = cmd->subindex;

	for(int i = 0; i < 4; i++)
	{
		data[i + data_offset] = (cmd->value >> (i * 8)) & 0xff;
	}
}

void can_open_transmission(command_t *cmd, uint8_t sdo_cmd)
{
	HAL_StatusTypeDef can_status;

	CAN_TxHeaderTypeDef header = get_transmit_header(cmd->node_id);
	uint8_t data[8];
	cmd_to_data_bytes(cmd, sdo_cmd, data);

	can_status = HAL_CAN_AddTxMessage(&hcan1, &header, data, (uint32_t *) CAN_TX_MAILBOX0);
	if(can_status != HAL_OK)
	{
		cmd->status = (can_status << 4) | CAN_TRANSMISSION_ERR;
		cmd->value = 0;
	}
}
