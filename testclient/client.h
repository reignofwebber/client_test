#include <boost/asio.hpp>
#include <thread>
#include <iostream>
#include <memory>
#include <chrono>
#include <fstream>

#include "message.h"

using boost::asio::ip::tcp;


class Client
{
public:
	Client(boost::asio::io_context &io_context, const tcp::resolver::results_type &endpoints, int train_no)
		: io_context_(io_context),
		  socket_(io_context),
		  endpoints_(endpoints),
		  train_no_(train_no)
	{
		do_connect();
	}

	~Client()
	{
		boost::system::error_code ec;
		socket_.shutdown(tcp::socket::shutdown_both, ec);
		if (ec)
		{
			std::cout << "shutdown error, code is " << ec << std::endl;
		}
		socket_.close();
	}

private:
	void do_connect()
	{
		boost::asio::async_connect(socket_, endpoints_, 
			[this](boost::system::error_code ec, tcp::endpoint endpoint)
		{
			if (!ec)
			{
				if (std::rand() % 2)
				{
					do_snip_write();
				}
				else
				{
					do_write();
				}
			}
			else
			{
				std::cout << ec.message()  << ", code is "<< ec.value() << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				do_connect();
			}
		});
	}

	void do_snip_write()
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));

		auto msgPtr = std::make_shared<Message>(MsgType::STATUS_MSG);

		msgPtr->setTrainNo(train_no_);
		msgPtr->randomMsg();
		uint8_t *t_data = msgPtr->data();
		auto type = msgPtr->type();

		boost::asio::write(socket_, boost::asio::buffer(t_data, 1460));
		std::this_thread::sleep_for(std::chrono::seconds(1));
		boost::asio::async_write(socket_, boost::asio::buffer(t_data+1460, msgPtr->length()-1460), 
			[this, msgPtr](boost::system::error_code ec, size_t /*length*/)
		{
			if (!ec)
			{
				if (msgPtr->type() == STATUS_MSG)
				{
					do_read();
				}
				else
				{
					if (std::rand() % 2)
					{
						do_snip_write();
					}
					else
					{
						do_write();
					}

				}

			}
			else
			{
				std::cout << "write error " << ec.message() << ec.value();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				//do_connect();
			}

		});
	}

	void do_write()
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));

		auto msgPtr = std::make_shared<Message>();
		msgPtr->setTrainNo(train_no_);
		msgPtr->randomMsg();
		uint8_t *t_data = msgPtr->data();
		auto type = msgPtr->type();

		if (msgPtr->type() == STATUS_MSG)
		{
			std::ofstream out(std::to_string(msgPtr->id()), std::ios::binary);
			out << std::hex;
			for (auto i = 0; i < msgPtr->length(); ++i)
			{
				out << *(t_data + i);
			}
			std::cout << "write msg, msg'id is " << msgPtr->id() << std::endl;
		}

		boost::asio::async_write(socket_, boost::asio::buffer(msgPtr->data(), msgPtr->length()),
			[this, msgPtr](boost::system::error_code ec, size_t /*length*/) 
		{
			if (!ec)
			{
				if (msgPtr->type() == STATUS_MSG)
				{
					do_read();
				}
				else
				{
					if (std::rand() % 2)
					{
						do_snip_write();
					}
					else
					{
						do_write();
					}
				}

			}
			else
			{
				std::cout << "write error " << ec.message() << ec.value();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				//do_connect();
			}
		});
	}

	void do_read()
	{
		auto msgPtr = std::make_shared<Message>(STATUS_MSG);
		
		boost::asio::async_read(socket_, boost::asio::buffer(msgPtr->data(), msgPtr->length()), 
			[this, msgPtr](boost::system::error_code ec, size_t length)
		{
			if (!ec)
			{
				std::cout << "receive msg" << std::endl;
				if (std::rand() % 2)
				{
					do_snip_write();
				}
				else
				{
					do_write();
				}

			}
			else
			{
				std::cout << "read error" << ec.message() << ec.value();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				//do_connect();
			}
		});

	}

private:
	boost::asio::io_context &io_context_;
	tcp::socket socket_;
	tcp::resolver::results_type endpoints_;
	int train_no_;

};
