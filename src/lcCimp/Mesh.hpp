#ifndef LTCIMP_MESH_HPP
#define LTCIMP_MESH_HPP

#include <string>

namespace ltcimp
{

////////////////////////////////////////////////////////////
/// Stores triangulated data only.
////////////////////////////////////////////////////////////
class Mesh
{
public:
	float *verts;
	unsigned int numVerts;

	float *norms;
	//TODO: Better define what this stores
	unsigned int numNorms;

	std::string name;
	std::string id;

	////////////////////////////////////////////////////////////
	/// Sets everyone to zero
	////////////////////////////////////////////////////////////
	Mesh();

	// TODO: Rule of three
};

}

#endif //LTCIMP_MESH_HPP