#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include"CServer.h"
#include"ConfigMgr.h"
int main()
{
	auto& gCfgMgr=ConfigMgr::Inst();
	std::string gate_port_str = gCfgMgr["GateServer"]["Port"];
	//std::cout << gate_port_str;
	unsigned short gate_port = stoi(gate_port_str);
	try
	{
		unsigned short port = static_cast<unsigned short>(8081);
		net::io_context ioc{ 1 };
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
			if (error)return;
			ioc.stop();
			});
		std::make_shared<CServer>(ioc, port)->Start();
		std::cout << "服务器已启动\n";
		std::cout << "Gate Server listen on port: " << port << "\n";
		ioc.run();
		
	}
	catch (const std::exception&e)
	{
		std::cerr << "Error" << e.what() << "\n";
		return EXIT_FAILURE;
	}
} 