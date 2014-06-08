#include <iostream>
#include <fstream>
#include <string>

#include "ltCimp.hpp"

std::string read(const std::string &filename);

int main()
{
	ltcimp::import(read("../../content/testModel.dae"));

	std::cout << "Done... ";
	std::cin.get();

	return 0;
}

std::string read(const std::string &filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);

	if (file.is_open())
	{
		std::string contents;
		
		// Work out the size of the file so we can
		// set the string size.
		file.seekg(0, std::ios::end);
		contents.resize((unsigned int )file.tellg());

		// Read the entire file into contents.
		file.seekg(0, std::ios::beg);
		file.read(&contents[0], contents.size());

		file.close();

		return contents;
	}

	// File didn't open, return a blank string.
	std::cerr << "File Not Found: \"" << filename << "\"" << std::endl;
	return "";
}