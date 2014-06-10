#ifndef LTCIMP_HPP
#define LTCIMP_HPP

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstring>
#include <iostream>

#include <rapidxml.hpp> // rapidxml 1.13
#include "Mesh.hpp"

namespace ltcimp
{

class FloatArray
{
public:
	std::string id;
	unsigned int count;
	unsigned int stride;
	float *data;
		
	/** Default Constructor */
	FloatArray () 
	: id(""), count(0), stride(0), data(nullptr)
	{}

	/** Copy Constructor */
	FloatArray (const FloatArray& other) 
	: id(other.id), count(other.count), stride(other.stride), data(new float[other.count])
	{
		for (unsigned int i = 0; i < other.count; i++)
			data[i] = other.data[i];
	}

	/** Copy Assignment Operator */
	FloatArray& operator= (FloatArray other)
	{
		id = other.id;
		count = other.count;
		stride = other.stride;
		data = new float[other.count];

		for (unsigned int i = 0; i < other.count; i++)
			data[i] = other.data[i];

		return *this;
	}

	/** Destructor */
	~FloatArray() { delete[] data; }
};

void import(const std::string &text, Mesh &mesh);
void importGeometry(rapidxml::xml_node<> &geomNode, Mesh &mesh);
bool readFloatArray(rapidxml::xml_node<> &floatArrayNode, rapidxml::xml_node<> &techniqueNode, FloatArray &destination);
bool readPolyList(rapidxml::xml_node<> &polyListNode, Mesh &mesh, const std::map<std::string, FloatArray*> &sources);

} // Namespace ltcimp

#endif //LTCIMP_HPP