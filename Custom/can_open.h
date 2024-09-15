
#ifndef INC_CAN_OPEN_H_
#define INC_CAN_OPEN_H_


#include <stdbool.h>
#include <stdint.h>

/* transmission headers */
uint32_t get_transmit_header(const uint8_t node_id);


/* Payload */

static const uint8_t data_offset = 4;
static const uint8_t index_offset = 6;
static const uint8_t smd_cmd_offset = 7;

uint8_t sdo_wr_command(uint8_t no_of_bytes);

uint32_t data_bytes_to_value(const uint8_t data[8]);

//void cmd_to_data_bytes(command_t *cmd, uint8_t sdo_command, uint8_t data[8]);

/* COB ID */

uint32_t cob_id_nmt_control(uint8_t node_id);

uint32_t cob_id_sync(uint8_t node_id);

uint32_t cob_id_sdo_send(uint8_t node_id);

uint32_t cob_id_sdo_response(uint8_t node_id);

uint32_t cob_id_nmt_monitoring(uint8_t node_id);

uint32_t cob_id_heartbeat(uint8_t node_id);

/* NMT Control Command */
void nmt_start_node(uint8_t node_id, uint8_t data[8]);

void nmt_stop_node(uint8_t node_id, uint8_t data[8]);

void nmt_go_pre_operational(uint8_t node_id, uint8_t data[8]);

void nmt_reset_node(uint8_t node_id, uint8_t data[8]);

void nmt_reset_reset(uint8_t node_id, uint8_t data[8]);

/* SDO commands */

//TODO: still need to accommodate segmented transfers
#define SDO_cmd_read_size_any  0x40 // any data size
#define SDO_cmd_read_size_1    0x4F // 1 byte
#define SDO_cmd_read_size_2    0x4B // 2 bytes
#define SDO_cmd_read_size_4    0x43 // 4 bytes
#define SDO_cmd_write_size_1   0x2F // 1 byte
#define SDO_cmd_write_size_2   0x2B // 2 bytes
#define SDO_cmd_write_size_4   0x23 // 4 bytes
#define SDO_res_write_success  0x60
#define SDO_res_write_failed   0x80

/* command byte break down */

// 7 downto 5 ccs
//000b download segment
//001b initiate doownload
//010b initiate upload
//011b uplood segment
//100b abort
//101b block upload
//110b block download


// 4 Toggle bit used in segmented mode
// 3 downto 2 no of bytes that do not contain data
// 1 expedited transfer
// 0 data bytes stored in frame's data region

// break down command bytes
// 0x40 = 0100 0000b  // initiate upload
// 0x4F = 0100 1111b // initiate upload, 3 bytes not stuffed, expedite transfer, bytes in frame
// 0x4B = 0100 1011b // initiate upload, 2 bytes not stuffed, expedite transfer, bytes in frame



#endif
