#include "FloatArray.hpp"

namespace ltcimp
{

FloatArray::FloatArray() 
: id(""), count(0), stride(0), data(nullptr)
{}

FloatArray::FloatArray(const FloatArray& other) 
: id(other.id), count(other.count), stride(other.stride), data(new float[other.count])
{
	for (unsigned int i = 0; i < other.count; i++)
		data[i] = other.data[i];
}

FloatArray& FloatArray::operator= (FloatArray other)
{
	id = other.id;
	count = other.count;
	stride = other.stride;
	data = new float[other.count];

	for (unsigned int i = 0; i < other.count; i++)
		data[i] = other.data[i];

	return *this;
}

FloatArray::~FloatArray() { delete[] data; }

} // Namespace ltcimp
