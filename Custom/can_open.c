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




uint32_t get_transmit_header(const uint8_t node_id)
{
//	CAN_TX_HEADER header;
//	header.StdId = 0x600 + node_id;
//	header.Rtr = CAN_RTR_DATA;
//	header.Ide = CAN_ID_STD;
//	header.DLC = 8;
	return 0x600 + node_id;
}

uint32_t data_bytes_to_value(const uint8_t data[8])
{
	uint32_t value = 0;
	for(uint8_t i = 0; i < 4; i++)
	{
		//uint32_t temp = data[i + data_offset];
		value += ((uint32_t) data[(data_offset + i)]) << (i * 8);
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
