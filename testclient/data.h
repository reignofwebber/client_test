// testclient.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <boost/asio/buffer.hpp>
#include <cstdlib>
#include <cassert>
#include <random>
#include <ctime>
#include <chrono>


enum MsgType
{
	STATUS_MSG = 0x01,
	LIVE_MSG = 0x03
};

class MsgFactory
{
	const static size_t status_msg_len = 35+1922+5;
	const static size_t live_msg_len = 100;

	const static size_t reserve_len = 35;
	const static size_t status_len = 1440 - 4;
	const static size_t error_len = 1922 - 1508;

	const static size_t tail_offset = 35 + 1922;

public:
	static boost::asio::const_buffer getMsgBuffer()
	{
		if (std::rand() % 2)
		{
			return getMsgBuffer(MsgType::LIVE_MSG);
		}
		else
		{
			return getMsgBuffer(MsgType::STATUS_MSG);
		}
	}
	static boost::asio::const_buffer getMsgBuffer(MsgType type)
	{
		size_t data_len(1);
		if (type == STATUS_MSG)
		{
			std::cout << "send status msg\n";
			data_len = status_msg_len;
		}
		else if (type == LIVE_MSG)
		{
			std::cout << "send live msg\n";
			data_len = live_msg_len;
		}

		uint8_t *data = static_cast<uint8_t *>(std::malloc(data_len));
		data[0] = 0xAA, data[1] = 0xAB, data[2] = 0xAC, data[3] = type;
		for (auto i = 4; i < data_len; ++i)
		{
			*(data + i) = 0x00;
		}

		if (type == STATUS_MSG)
		{
			insertMsgTime(data);
			insertStatusPort(data);
			insertErrorPort(data);
			insertMsgTail(data);
		}
		
		return boost::asio::buffer(data, data_len);
	}


private:
	static void insertMsgTime(uint8_t *data)
	{
		auto now = std::chrono::system_clock::now();
		auto t = std::chrono::system_clock::to_time_t(now);
		tm *timeinfo = localtime(&t);
		data[19] = timeinfo->tm_year - 100;
		data[20] = timeinfo->tm_mon + 1;
		data[21] = timeinfo->tm_mday;
		data[22] = timeinfo->tm_hour;
		data[23] = timeinfo->tm_min;
		data[24] = timeinfo->tm_sec;
		uint16_t *pmsec = (uint16_t *)(data + 25);
		// get millisecond
		auto duration = now.time_since_epoch();
		*pmsec = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
		// to big endian
		std::swap(data[25], data[26]);

	}

	static void insertStatusPort(uint8_t *data)
	{
		// port_address
		data[reserve_len] = 0x00;
		data[reserve_len + 1] = 0x01;
		
		// port_size
		uint16_t *port_size = (uint16_t *)(data + reserve_len + 2);
		*port_size = status_len;
		std::swap(data[reserve_len + 2], data[reserve_len + 3]);
	}

	static void insertErrorPort(uint8_t *data)
	{
		// port_address
		auto offset = reserve_len + status_len + 4;
		data[offset] = 0x00;
		data[offset + 1] = 0x02;
		uint16_t *port_size = (uint16_t *)(data + offset + 2);
		*port_size = error_len; 
		std::swap(data[offset + 2], data[offset + 3]);
	}

	static void insertMsgTail(uint8_t *data)
	{
		data[tail_offset + 2] = 0xBA;
		data[tail_offset + 3] = 0xBB;
		data[tail_offset + 4] = 0xBC;
	}

};