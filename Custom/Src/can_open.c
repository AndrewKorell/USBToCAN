/*
 * can_open.c
 *
 *  Created on: Aug 26, 2024
 *      Author: andyk
 */

#include "can_open.h"



uint8_t sdo_wr_command(uint8_t no_of_bytes)
{
	return ((4 - no_of_bytes) & 0x3) << 2;
}

CAN_TxHeaderTypeDef get_transmit_header(const uint8_t node_id)
{
	CAN_TxHeaderTypeDef header;
	header.StdId = 0x600 + node_id;
	header.RTR = CAN_RTR_DATA;
	header.IDE = CAN_ID_STD;
	header.DLC = 8;
	return header;
}

uint32_t data_bytes_to_value(uint8_t data[8])
{
	uint32_t value;
	for(int i = 0; i < 4; i++)
	{
		value |= (data[i + data_offset] << (i & 8)) & 0xff ;
	}
	return value;
}



/* COB ID */

uint32_t cob_id_nmt_control(uint8_t node_id)
{
	return 0x000 + node_id;
}

uint32_t cob_id_sync(uint8_t node_id)
{
	return 0x080 + node_id;
}

uint32_t cob_id_sdo_send(uint8_t node_id)
{
	return 0x600 + node_id;
}

uint32_t cob_id_sdo_response(uint8_t node_id)
{
	return 0x580 + node_id;
}

uint32_t cob_id_nmt_monitoring(uint8_t node_id)
{
	return 0x700 + node_id;
}

uint32_t cob_id_heartbeat(uint8_t node_id)
{
	return 0x700 + node_id;
}

/* NMT Control Command */
//For NMT function loads the node_id and the command byte
//function is named after NMT instruction

void nmt_start_node(uint8_t node_id, uint8_t data[8])
{
	data[7] = 0x01;
	data[6] = node_id;
}

void nmt_stop_node(uint8_t node_id, uint8_t data[8])
{
	data[7] = 0x02;
	data[6] = node_id;
}

void nmt_go_pre_operational(uint8_t node_id, uint8_t data[8])
{
	data[7] = 0x80;
	data[6] = node_id;
}

void nmt_reset_node(uint8_t node_id, uint8_t data[8])
{
	data[7] = 0x81;
	data[6] = node_id;
}

void nmt_reset_reset(uint8_t node_id, uint8_t data[8])
{
	data[7] = 0x82;
	data[6] = node_id;
}
