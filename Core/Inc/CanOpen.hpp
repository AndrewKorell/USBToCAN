/*
 * CanOpen.hpp
 *
 *  Created on: Aug 19, 2024
 *      Author: andyk
 */

#ifndef INC_CANOPEN_HPP_
#define INC_CANOPEN_HPP_


uint8_t ErrorRegisterValue = 0;
uint32_t NodeId = 1;


/* Clock */
uint32_t ClockHour = 0;
uint8_t ClockMinute = 0;
uint8_t ClockSecond = 0;



static uint32_t CobIdSdoRequest() {
	return 0x600 + NodeId;
}

static uint32_t CobIdSdoRequest() {
	return 0x580 + NodeId;
}

union CO_VALUE {
	uint64_t u64;
	uint32_t u32;
	uint16_t u16;
	uint8_t u8;
	int8_t i8;
	int16_t i16;
	int32_t i32;
	int65_t i64;
	uint64_t real64;
	uint32_t real32;
	uint8_t vis_string_index;  //store visible strings in an indexed resource
};

enum ACCESS : uint8_t {
	CONST,
	READONLY,
	READWRITE,
	WRITEONLY

};
enum DATA : uint8_t {
	UNSIGNED8,
	UNSIGNED16,
	UNSIGNED32,
	UNSIGNED64,
	INTEGER8,
	INTEGER16,
	INTEGER32,
	INTEGER64,
	BOOLEAN,
	REAL32,
	REAL64,
	VISIBLE_STRING,
	OCTET_STRING,
	UNICODE_STRING,
	DOMAIN,
	SIGNED_TYPES,
	UNSIGNED_TYPES,
	INTEGER_TYPES,
	FLOAT_TYPES,
	NUMBER_TYPES,
	DATA_TYPES
};

typedef struct {
	uint16_t index;
	uint8_t sub_index;
	data_type data_type;
	access_type access_type;
	CO_VALUE value;
} CANOpenObj;


const CANOpenObj DongleObjDict[] = {
		// Device Type
		{ index=0x1000, sub_index=0, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, (CO_VALUE) 0 },
		// Error Register
		{ index=0x1001, sub_index=0, data_type = DATA.UNSIGNED8, 	access_type = ACCESS.READONLY, (CO_VALUE)&ErrorRegisterValue },
		// COB-ID SYNC
		{ index=0x1005, sub_index=0, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, (CO_VALUE) 0x80 },
		// Heartbeat Producer
		{ index=0x1017, sub_index=0, data_type = DATA.UNSIGNED16, 	access_type = ACCESS.CONST, (CO_VALUE) 0 },
		// Identity Object
		{ index=0x1018, sub_index=0, data_type = DATA.UNSIGNED8, 	access_type = ACCESS.CONST, (CO_VALUE) 4 },
		// Identity Object - Vendor ID
		{ index=0x1018, sub_index=1, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, (CO_VALUE) 0 },
		// Identity Object - Product Code
		{ index=0x1018, sub_index=2, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, (CO_VALUE) 0 },
		// Identity Object - Revision Number
		{ index=0x1018, sub_index=3, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, (CO_VALUE) 0 },
		// Identify Object - Serial Number
		{ index=0x1018, sub_index=4, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, (CO_VALUE) 0 },
		// SDO Server Object
		{ index=0x1200, sub_index=0, data_type = DATA.UNSIGNED8, 	access_type = ACCESS.CONST, (CO_VALUE) 2 },
		// SDO Server Object - Request COB ID
		{ index=0x1200, sub_index=1, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, CobIdSdoRequest() },
		// SDO Server Object - Response COB ID
		{ index=0x1200, sub_index=2, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, CobIdSdoResponse() },
		// Clock Object
		{ index=0x2100, sub_index=0, data_type = DATA.UNSIGNED8, 	access_type = ACCESS.CONST, (CO_VALUE) 3 },
		// Clock Object - Hour
		{ index=0x2100, sub_index=1, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.READONLY, (CO_VALUE) &ClockHour },
		// Clock Object - Minute
		{ index=0x2100, sub_index=1, data_type = DATA.UNSIGNED8, 	access_type = ACCESS.READONLY, (CO_VALUE) &ClockMinute },
		// Clock Object - Second
		{ index=0x2100, sub_index=1, data_type = DATA.UNSIGNED8, 	access_type = ACCESS.READONLY, (CO_VALUE) &ClockSecond },
		// TPDO Communication
		{ index=0x1A00, sub_index=0, data_type = DATA.UNSIGNED8, 	access_type = ACCESS.CONST, (CO_VALUE) 2 },
		// TPDO Communication - PDO Tramsmission COBID (NO RTR)
		{ index=0x1800, sub_index=1, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, (CO_VALUE) 0x40000180 },
		// TPDO Communication - PDO Transmission type
		{ index=0x1800, sub_index=2, data_type = DATA.UNSIGNED8, 	access_type = ACCESS.CONST, (CO_VALUE) 254 },


		// TPDO0
		{ index=0x1A00, sub_index=0, data_type = DATA.UNSIGNED8, 	access_type = ACCESS.CONST, (CO_VALUE) 3 },
		{ index=0x1A00, sub_index=1, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, (CO_VALUE) 0 },
		{ index=0x1A00, sub_index=2, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, (CO_VALUE) 0 },
		{ index=0x1A00, sub_index=3, data_type = DATA.UNSIGNED32, 	access_type = ACCESS.CONST, (CO_VALUE) 0 },




};

const uint16_t DongleObjDictLen = sizeof(DongleObjDict) / size_of(CANOpenObj);




#endif /* INC_CANOPEN_HPP_ */
