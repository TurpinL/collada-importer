#ifndef LTCIMP_HPP
#define LTCIMP_HPP

#include <string>
#include <map>

#include <rapidxml.hpp> // rapidxml 1.13
#include "Mesh.hpp"
#include "FloatArray.hpp"

namespace ltcimp
{

class Importer
{
public:
	Importer();

	void setContentDirectory(const std::string &directory);
	void importScene(const std::string &filename);
	const Mesh& getMesh();

private:
	std::string m_contentDirectory;
	Mesh m_mesh;
};

void importGeometry(rapidxml::xml_node<> &geomNode, Mesh &mesh);
bool readFloatArray(rapidxml::xml_node<> &floatArrayNode, rapidxml::xml_node<> &techniqueNode, FloatArray &destination);
bool readPolyList(rapidxml::xml_node<> &polyListNode, Mesh &mesh, const std::map<std::string, FloatArray*> &sources);

} // Namespace ltcimp

#endif //LTCIMP_HPP