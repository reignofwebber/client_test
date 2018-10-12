#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>

#include "data.h"

using boost::asio::ip::tcp;

int main(void)
{
	try
	{
		boost::asio::io_context io_context;

		tcp::resolver resolver(io_context);
		auto endpoints =
			resolver.resolve("10.50.200.66", "19999");

		tcp::socket socket(io_context);
		boost::asio::connect(socket, endpoints);

		boost::system::error_code ignored_error;
		
		while (true)
		{
			boost::asio::const_buffer buf = MsgFactory::getMsgBuffer();
			boost::asio::write(socket, buf, ignored_error);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}