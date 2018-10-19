#include "telnet.h"


int main()
{
	boost::asio::io_context io_context;
	std::string ip("222.173.73.19");
	TelnetClient client(io_context, ip);
	client.scanAll();
//	client.scan(80);
	getchar();
	return 0;
}