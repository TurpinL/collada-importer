#include "Mesh.hpp"

namespace ltcimp
{
	Mesh::Mesh()
	: verts(nullptr), numVerts(0), 
	  norms(nullptr), numNorms(0), hasNorms(false),
	  texCoords(nullptr), numTexCoords(0), hasTexCoords(false),
	  name("")
	{}
}