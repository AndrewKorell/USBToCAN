
#include <gtest/gtest.h>

extern "C"
{
#include "../Custom/can_open.h"
}


TEST(CanOpenTest, DataBytesToValueCorrectValue)
{
	uint32_t expected_value = 0x08070605;
	const uint8_t test_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

	uint32_t value = data_bytes_to_value(test_data);

	EXPECT_EQ(value, expected_value);
}


TEST(CanOpenTest, ReturnNmtControlCobId)
{
	uint8_t node_id = 1;
	uint32_t expected = 0x000 + node_id;
	uint32_t value = cob_id_nmt_control(node_id);

	EXPECT_EQ(value, expected);
}

TEST(CanOpenTest, ReturnSyncCobId)
{
	uint8_t node_id = 1;
	uint32_t expected = 0x080 + node_id;
	uint32_t value = cob_id_sync(node_id);

	EXPECT_EQ(value, expected);
}

TEST(CanOpenTest, ReturnSdoSendCobId)
{
	uint8_t node_id = 1;
	uint32_t expected = 0x600 + node_id;
	uint32_t value = cob_id_sdo_send(node_id);

	EXPECT_EQ(value, expected);
}

TEST(CanOpenTest, ReturnSdoResponseCobId)
{
	uint8_t node_id = 1;
	uint32_t expected = 0x580 + node_id;
	uint32_t value = cob_id_sdo_response(node_id);

	EXPECT_EQ(value, expected);
}

TEST(CanOpenTest, ReturnSdoNmtMonitoringCobId)
{
	uint8_t node_id = 1;
	uint32_t expected = 0x700 + node_id;
	uint32_t value = cob_id_nmt_monitoring(node_id);

	EXPECT_EQ(value, expected);
}

TEST(CanOpenTest, ReturnHeartbeatCobId)
{
	uint8_t node_id = 1;
	uint32_t expected = 0x700 + node_id;
	uint32_t value = cob_id_heartbeat(node_id);

	EXPECT_EQ(value, expected);
}


TEST(CanOpenTest, ReturnHeartBeatId)
{
	uint8_t node_id = 1;
	uint32_t cob_id = cob_id_heartbeat(node_id);
	EXPECT_EQ(cob_id, 0x700 + node_id);
}


TEST(CanOpenTest, ReturnNmtStartNode)
{
	const uint8_t test_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

	uint8_t node_id = 1;
	uint32_t cob_id = nmt_start_node(node_id, test_data);

	EXPECT_EQ(test_data[7], 0x01);
	EXPECT_EQ(test_data[6], node_id);
	EXPECT_EQ(test_data[5], 0x05);

}

TEST(CanOpenTest, ReturnNmtStopNode)
{
	const uint8_t test_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

	uint8_t node_id = 1;
	uint32_t cob_id = nmt_stop_node(node_id, test_data);

	EXPECT_EQ(test_data[7], 0x02);
	EXPECT_EQ(test_data[6], node_id);
	EXPECT_EQ(test_data[5], 0x05);

}

TEST(CanOpenTest, ReturnNmtGoPreOp)
{
	const uint8_t test_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

	uint8_t node_id = 1;
	uint32_t cob_id = nmt_go_pre_operational(node_id, test_data);

	EXPECT_EQ(test_data[7], 0x80);
	EXPECT_EQ(test_data[6], node_id);
	EXPECT_EQ(test_data[5], 0x05);

}

TEST(CanOpenTest, ReturnNmtResetNode)
{
	const uint8_t test_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

	uint8_t node_id = 1;
	uint32_t cob_id = nmt_reset_node(node_id, test_data);

	EXPECT_EQ(test_data[7], 0x81);
	EXPECT_EQ(test_data[6], node_id);
	EXPECT_EQ(test_data[5], 0x05);

}

TEST(CanOpenTest, ReturnNmtResetNode)
{
	const uint8_t test_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

	uint8_t node_id = 1;
	uint32_t cob_id = nmt_reset_reset(node_id, test_data);

	EXPECT_EQ(test_data[7], 0x82);
	EXPECT_EQ(test_data[6], node_id);
	EXPECT_EQ(test_data[5], 0x05);
}
