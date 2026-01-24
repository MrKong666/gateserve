#include "LogicSystem.h"
#include"HttpConnection.h"
bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection>con)
{
	if (_get_handlers.find(path) == _get_handlers.end()) {
		return false;
	}
	_get_handlers[path](con);
	return true;
}
void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
	_get_handlers.insert(std::make_pair(url, handler));
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection>con)
{
	if (_post_handlers.find(path) == _post_handlers.end()) {
		return false;
	}
	_post_handlers[path](con);
	return true;
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
	_post_handlers.insert(std::make_pair(url, handler));
}

LogicSystem::LogicSystem()
{
	RegGet("/get_test", [](std::shared_ptr<HttpConnection>connection) {
		beast::ostream(connection->_response.body())<<"receive get_test req";
		int i = 0;
		for (auto& elem : connection->_get_params) {
			i++;
			beast::ostream(connection->_response.body()) << "param" << i << "key is " << elem.first;
			beast::ostream(connection->_response.body()) << " ," << i << "value is" << elem.second<<"\n";
		}	
		});
	RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection>connection) {
		auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
		std::cout << "receive body is" << body_str << "\n";
		connection->_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success) {
			std::cout << "Failed to parse JSON data!" << "\n";
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		}
		 
		if (src_root.isMember("email")) {
			auto email = src_root["email"].asString(); 
			std::cout << "return email is " << email << "\n";
			root["error"] = 0;
			root["email"] = src_root["email"]; 
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		}
		else {
			std::cout << "Failed to parse JSON data!" << "\n";
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		}
	});
}
/*路由注册机制：回调函数与映射表
在 LogicSystem 中，使用了 std::map 配合 std::function 来实现路由分发。
这是一种非常经典且高效的设计模式。
解耦请求处理：通过 RegGet 将 URL 路径与具体的业务逻辑（Lambda 表达式或函数）绑定。
这意味着 HttpConnection 不需要知道如何处理具体的业务，它只需要调用 HandleGet 即可。
灵活性：可以随时在 LogicSystem 的构造函数中添加新的接口，而无需修改网络层的底层代码。*/