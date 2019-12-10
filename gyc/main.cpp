#include <iostream>
#include <boost/filesystem.hpp>
#include <fstream>
#include <token_stream.hpp>
#include <tuple>
#include <vector>
#include <optional>

#define CONFIG_FILE ".gycrc"
#define BACKUP_BUFFIX ".gycbak"

namespace fs = boost::filesystem;
using namespace token;

bool conf_exist();
std::tuple<fs::path,std::string> get_conf_ycm_path();

std::vector<fs::path> find_paths(fs::path& dir, std::function<bool(const fs::path&)> interest);
void ycm_conf_append_include(const fs::path& ycm_path, const std::vector<std::string>& is);
fs::path get_backup_path(const fs::path& src);
fs::path copy_backup(const fs::path& src);
std::optional<std::string> get_source_dir(fs::path& root);

int main(int argc,char **argv)
{
	getchar();
	bool exist = conf_exist();
	std::cout << std::boolalpha << exist << std::endl;
	try {
		if (exist)
		{
			auto [cf,str] = get_conf_ycm_path();
			std::cout << str << std::endl;
			std::vector<std::string> is = { "sss","asdaasd" };
			ycm_conf_append_include(cf,is);
		}
	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;
		return 0;
	}
	if (argc == 1)
		return 0;
	fs::path root(argv[1]);
	root.append("CMakeFiles");
	auto chs = find_paths(root, [](const fs::path& p) {
		fs::path ex = p.extension();
		if (ex == ".dir")
			return true;
		else
			return false;
	});

	for (auto& p : chs)
	{
		auto temp = p.generic_string();
		std::cout << temp << std::endl;
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

std::vector<fs::path> find_paths(fs::path& dir, std::function<bool(const fs::path&)> interest)
{
	std::vector<fs::path> res;
	if (fs::exists(dir))
	{
		for (auto& it: fs::directory_iterator(dir))
		{
			if (interest(it))
			{
				res.push_back(it);
			}
		}
	}
	return res;
}

std::optional<std::string> get_source_dir(fs::path& root)
{
	root.append("");
	return {};
}

fs::path copy_backup(const fs::path& src)
{
	fs::path backup = get_backup_path(src);
	fs::copy_file(src, backup);
	return backup;
}

fs::path get_backup_path(const fs::path& src)
{
	fs::path backup = src.parent_path();
	fs::path name = src.filename();
	auto ns = name.generic_string();
	ns += BACKUP_BUFFIX;
	backup.append(ns);
	return backup;
}

void ycm_conf_append_include(const fs::path& ycm_path,const std::vector<std::string>& ins)
{
	if (fs::exists(ycm_path))
	{
		std::string ycm_path_s = ycm_path.generic_string();
		std::ifstream is(ycm_path_s, std::ios::binary);
		TokenStream<std::ifstream> ts(std::move(is));
		ts.analyse();
		auto insert_it = ts.tokens.end();
		auto end_it = ts.tokens.end();
		int insert_i = -1;

		std::vector<Token> vs;
		vs.push_back(Token("", '[', '\n'));

		bool in_flags = false;
		for (int i = 0; i < ts.tokens.size(); ++i)
		{
			auto& it = ts.tokens[i];
			if (it.body == "flags")
			{
				++i;
				while (i < ts.tokens.size() && ts.tokens[i].per != '[' ) { ++i; };
				if (i >= ts.tokens.size())
					break;
				insert_i = i;
				insert_it = ts.tokens.begin() + i;
				in_flags = true;
			}
			if (in_flags)
			{
				while (i < ts.tokens.size() && ts.tokens[i].back != ']') 
				{
					if ((ts.tokens[i].per == '\'' || ts.tokens[i].per == '"') &&
						(ts.tokens[i].back == '\'' || ts.tokens[i].back == '"') &&
						!ts.tokens[i].body.empty())
					{
						vs.push_back(std::move(ts.tokens[i]));
						vs.push_back(Token("", Token::None, ','));
						vs.push_back(Token("", Token::None, '\n'));
					}
					++i;
				};
				if (i >= ts.tokens.size())
					break;
				end_it = ts.tokens.begin() + (i + 1);
				break;
			}
		}
		if (insert_it != ts.tokens.end() && end_it != ts.tokens.end())
		{
			ts.tokens.erase(insert_it, end_it);

			for (auto& s : ins)
			{
				vs.push_back(Token("-I", '\'', '\''));
				vs.push_back(Token("", Token::None, ','));
				vs.push_back(Token("", Token::None, '\n'));
				vs.push_back(Token(s,'\'','\''));
				vs.push_back(Token("", Token::None, ','));
				vs.push_back(Token("", Token::None, '\n'));
			}

			vs.push_back(Token("", Token::None, ']'));
			vs.push_back(Token("", Token::None, '\n'));
			
			auto temp_it = ts.tokens.begin() + insert_i;
			for (auto& s : vs)
			{
				temp_it = ts.tokens.insert(temp_it,std::move(s));
				++temp_it;
			}

			for (int i = 0; i < ts.tokens.size() - 1; ++i)
			{
				if (ts.tokens[i].back == '\n' && ts.tokens[i + 1].per == ' ')
				{
					ts.tokens[i + 1].per = '\t';
				}
			}

			ts.save(ycm_path_s, true);
		}
		else {
			throw std::runtime_error("Parser failed!");
		}

	}else{
		throw std::runtime_error("Not exists!");
	}
}
