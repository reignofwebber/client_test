#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

using boost::asio::ip::tcp;

class TelnetClient
{
public:
	TelnetClient(boost::asio::io_context &io_context, const std::string &ip)
		: io_context_(io_context), ip_(ip)
	{

	}

	void scanAll()
	{
		auto th_cont = std::thread::hardware_concurrency() - 1;
		auto unit = 65535 / th_cont;
		std::vector<std::thread> threads;
		for (auto i = 0; i < th_cont; ++i)
		{
			threads.emplace_back([=]()
			{
				for (auto p = unit * i; p != unit * (i + 1); ++p)
				{
					scan(p);
				}
			});
		}
		for (auto p = unit * th_cont; p < 65535; ++p)
		{
			scan(p);
		}

		std::for_each(threads.begin(), threads.end(), [](std::thread &t) {
			t.join();
		});
	}

	void scan(int port)
	{
		tcp::resolver resolver(io_context_);
		auto endpoints = resolver.resolve(ip_, std::to_string(port));
		
		tcp::socket socket(io_context_);
		boost::system::error_code ec;
		boost::asio::connect(socket, endpoints, ec);
//		std::cout << "port is " << port << " , code is " << ec.value() << " , message is " << ec.message() << std::endl;
		if (!ec)
		{
			std::cout << "port <<" << port << ">> is open!" << std::endl;
		}
		if (socket.is_open())
		{
			socket.shutdown(tcp::socket::shutdown_both, ec);
			socket.close();
		}


	}



private:
	boost::asio::io_context &io_context_;
	std::string ip_;
};