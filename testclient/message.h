// testclient.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <boost/asio/buffer.hpp>
#include <cstdlib>
#include <cassert>
#include <random>
#include <ctime>
#include <chrono>

#include <bitset>

enum MsgType
{
	STATUS_MSG = 0x01,
	LIVE_MSG = 0x03
};

class Message
{
	const size_t status_msg_len = 35+1922+5;
	const size_t live_msg_len = 100;

	const size_t reserve_len = 35;
	const uint16_t status_len = 1440 - 4;
	const uint16_t error_len = 1922 - 1508;

	const size_t tail_offset = 35 + 1922;

public:
	Message()
		: data_(nullptr), length_(0), type_(MsgType::LIVE_MSG)
	{
		if (std::rand() % 2)
		{
			type_ = MsgType::LIVE_MSG;
			length_ = live_msg_len;
		}
		else
		{
			type_ = MsgType::STATUS_MSG;
			length_ = status_msg_len;
		}
		data_ = static_cast<uint8_t *>(std::malloc(length_));
		id_ = std::rand();
	}

	Message(MsgType type)
		: data_(nullptr), length_(0), type_(type)
	{
		if (type == STATUS_MSG)
		{
			length_ = status_msg_len;
		}
		else if (type == LIVE_MSG)
		{
			length_ = live_msg_len;
		}

		data_ = static_cast<uint8_t *>(std::malloc(length_));
		id_ = std::rand();
	}

	Message(const Message &msg) = delete;

	~Message()
	{
		if (data_ != nullptr)
		{
			std::free((void *)data_);
			data_ = 0;
		}
	}

	void randomMsg()
	{
		data_[0] = 0xAA, data_[1] = 0xAB, data_[2] = 0xAC, data_[3] = type_;
		for (auto i = 4; i < length_; ++i)
		{
//			*(data_ + i) = rand() % 0x100;
			*(data_ + i) = 0;
		}

		if (type_ == STATUS_MSG)
		{
			insertKey();
			insertMsgTime();
			insertStatusPort();
			insertErrorPort();
			insertMsgTail();
			insertTestData();
			insertID();
			//randomErrorData();
		}
	}

	void setTrainNo(int train_no)
	{
		train_no_ = train_no;
	}

	uint8_t *data() const
	{
		return data_;
	}

	size_t length() const
	{
		return length_;
	}

	MsgType type() const
	{
		return type_;
	}
	
	int id() const
	{
		return id_;
	}

private:
	void insertKey()
	{
		data_[14] = 3;
		data_[15] = 7;
		*((uint16_t *)(data_ + 16)) = train_no_;
		std::swap(data_[16], data_[17]);
	}

	void insertMsgTime()
	{
		auto now = std::chrono::system_clock::now();
		auto t = std::chrono::system_clock::to_time_t(now);
		tm *timeinfo = localtime(&t);
		data_[19] = timeinfo->tm_year - 100;
		data_[20] = timeinfo->tm_mon + 1;
		data_[21] = timeinfo->tm_mday;
		data_[22] = timeinfo->tm_hour;
		data_[23] = timeinfo->tm_min;
		data_[24] = timeinfo->tm_sec;
		uint16_t *pmsec = (uint16_t *)(data_ + 25);
		// get millisecond
		auto duration = now.time_since_epoch();
		*pmsec = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
		// to big endian
		std::swap(data_[25], data_[26]);

	}

	void insertStatusPort()
	{
		// port_address
		data_[reserve_len] = 0x00;
		data_[reserve_len + 1] = 0x01;
		
		// port_size
		uint16_t *port_size = (uint16_t *)(data_ + reserve_len + 2);
		*port_size = status_len;
		std::swap(data_[reserve_len + 2], data_[reserve_len + 3]);
	}

	void insertErrorPort()
	{
		// port_address
		auto offset = reserve_len + 1508 - 4;
		data_[offset] = 0x00;
		data_[offset + 1] = 0x02;
		uint16_t *port_size = (uint16_t *)(data_ + offset + 2);
		*port_size = error_len; 
		std::swap(data_[offset + 2], data_[offset + 3]);
	}

	void insertMsgTail()
	{
		data_[tail_offset + 2] = 0xBA;
		data_[tail_offset + 3] = 0xBB;
		data_[tail_offset + 4] = 0xBC;
	}

	void insertTestData()
	{
		//年
		data_[reserve_len + 4] = 18;
		//当前站
		*((uint16_t *)(data_ + reserve_len + 37)) = rand() % 32;
		std::swap(data_[reserve_len + 37], data_[reserve_len + 38]);

		*((uint16_t *)(data_ + reserve_len + 39)) = rand() % 32;
		std::swap(data_[reserve_len + 39], data_[reserve_len + 40]);
		//下一站
		*((uint16_t *)(data_ + reserve_len + 41)) = rand() % 32;
		std::swap(data_[reserve_len + 41], data_[reserve_len + 42]);

		*((uint16_t *)(data_ + reserve_len + 43)) = rand() % 32;
		std::swap(data_[reserve_len + 43], data_[reserve_len + 44]);

		*((uint16_t *)(data_ + reserve_len + 45)) = rand() % 32;
		std::swap(data_[reserve_len + 45], data_[reserve_len + 46]);

		*((uint16_t *)(data_ + reserve_len + 47)) = rand() % 32;
		std::swap(data_[reserve_len + 47], data_[reserve_len + 48]);

		data_[reserve_len + 50] = rand() % 9 + 1;
		data_[reserve_len + 51] = rand() % 5 + 1;


	}

	void insertID()
	{
		auto offset = reserve_len + 1508 - 4;
		data_[offset - 1] = rand() % 256;

	}

	void randomErrorData()
	{
		for (auto i = 1508; i < 1922; ++i)
		{
			std::bitset<8> set;
			for (auto i = 0; i < 8; ++i)
			{
				set[i] = std::rand() % 10000;
			}
			set.flip();
			data_[reserve_len + i] = static_cast<char>(set.to_ulong());
		}
	}

private:
	uint8_t *data_;
	size_t length_;
	MsgType type_;
	int id_;
	int train_no_;
};