#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>

#include "client.h"

using boost::asio::ip::tcp;

int main(int argc, char** argv)
{
	try
	{
		int train_no = 0;
		if (argc != 1)
		{
			train_no = std::stoi(argv[1]);
		}

		boost::asio::io_context io_context;

		tcp::resolver resolver(io_context);
		auto endpoints =
			resolver.resolve("10.50.200.66", "19999");


		Client client(io_context, endpoints, train_no);

		io_context.run();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}