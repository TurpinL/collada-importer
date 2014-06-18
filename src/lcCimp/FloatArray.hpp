#ifndef LTCIMP_FLOATARRAY_HPP
#define LTCIMP_FLOATARRAY_HPP

#include <string>

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
	FloatArray ();

	/** Copy Constructor */
	FloatArray (const FloatArray& other);

	/** Copy Assignment Operator */
	FloatArray& operator= (FloatArray other);

	/** Destructor */
	~FloatArray();
};

} // Namespace ltcimp

#endif // LTCIMP_FLOATARRAY_HPP