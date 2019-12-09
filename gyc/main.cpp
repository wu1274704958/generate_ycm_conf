#include <iostream>
#include <boost/filesystem.hpp>
#include <fstream>
#include <token_stream.hpp>
#include <tuple>

#define CONFIG_FILE ".gycrc"

namespace fs = boost::filesystem;
using namespace token;

bool conf_exist();
std::tuple<fs::path,std::string> get_conf_ycm_path();

int main()
{
	bool exist = conf_exist();
	std::cout << std::boolalpha << exist << std::endl;
	try {
		if (exist)
		{
			auto [cf,str] = get_conf_ycm_path();
			std::cout << str << std::endl;
		}
	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;
		return 0;
	}
}

bool conf_exist()
{
	fs::path p(CONFIG_FILE);
	return fs::exists(p);
}

std::tuple<fs::path, std::string> get_conf_ycm_path()
{
	fs::path p(CONFIG_FILE);
	if (fs::exists(p))
	{
		std::ifstream is(p.c_str(),std::ios::binary);
		TokenStream<std::ifstream> ts(std::move(is));
		ts.analyse();
		std::string str;
		int f = 0;
		for (int i = 0; i < ts.tokens.size(); ++i)
		{
			if (f == 0 && ts.tokens[i].body == "original")
				++f;
			if (f == 1 && ts.tokens[i].back == '=')
			{
				++f;
				continue;
			}
			if (f == 1 && ts.tokens[i].per == '=')
				++f;
			if (f == 2)
			{
				++f;
				while (ts.tokens[i].per == ' ' && ts.tokens[i].body.empty() && ts.tokens[i].back == ' ') { ++i; }
				if (ts.tokens[i].per != ' ' && ts.tokens[i].per != '=')
					str += ts.tokens[i].per;
				if (!ts.tokens[i].body.empty())
					str += ts.tokens[i].body;
				if (ts.tokens[i].back != ' ' && ts.tokens[i].back != '=')
					str += ts.tokens[i].back;
				++i;
				while(i < ts.tokens.size())
				{
					if(ts.tokens[i].per != Token::None)
					str += ts.tokens[i].per;
					str += ts.tokens[i].body;
					if (ts.tokens[i].back == '\n')
						break;
					if (ts.tokens[i].back != Token::None)
						str += ts.tokens[i].back;
					++i;
				}
				break;
			}
		}
		if(f != 3 || str.empty())
			throw std::runtime_error("Not read original!");
		return std::make_tuple(fs::path(str),std::move(str));
	}
	else
	{
		throw std::runtime_error("Not found config file!");
	}
}