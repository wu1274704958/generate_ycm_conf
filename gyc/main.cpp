#include <iostream>
#include <boost/filesystem.hpp>

#define CONFIG_FILE ".gycrc"

bool conf_exist();

int main()
{
	std::cout << std::boolalpha << conf_exist() << std::endl;
}

bool conf_exist()
{
	namespace fs = boost::filesystem;
	fs::path p(CONFIG_FILE);
	return fs::exists(p);
}