#include "ltCimp.hpp"

#include <sstream>
#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>

namespace ltcimp
{

std::string read(const std::string &filename);

Importer::Importer() 
{
	m_contentDirectory = "";
}

void Importer::setContentDirectory(const std::string &directory)
{
	m_contentDirectory = directory; 
}

const Mesh& Importer::getMesh()
{
	return m_mesh;
}

void Importer::importScene(const std::string &filename)
{
	// Read the collada file
	std::string fileContents = read(m_contentDirectory + filename);

	char *buffer = new char[fileContents.size() + 1];
	strcpy(buffer, fileContents.c_str());

	rapidxml::xml_document<> doc;
	doc.parse<0>(buffer);

	// TODO: nullptr checks
	rapidxml::xml_node<> *colladaNode = doc.first_node("COLLADA");

	rapidxml::xml_node<> *curLibraryNode = colladaNode->first_node();

	while (curLibraryNode != nullptr)
	{
		std::string curName = curLibraryNode->name();

		if (curName == "library_geometries")
			importGeometry(*curLibraryNode->first_node("geometry"), m_mesh);

		curLibraryNode = curLibraryNode->next_sibling();
	}
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

void importGeometry(rapidxml::xml_node<> &geomNode, Mesh &mesh)
{
	std::map<std::string, FloatArray*> sources;
	rapidxml::xml_node<> *meshNode = geomNode.first_node("mesh");

	// No mesh :(
	if( !meshNode ) { return; }

	// Get the name and id attributes to set the mesh's attribuets.
	rapidxml::xml_attribute<> *idAttrib = geomNode.first_attribute("id");
	rapidxml::xml_attribute<> *nameAttrib = geomNode.first_attribute("name");
	if(idAttrib) { mesh.id = idAttrib->value(); }
	if(nameAttrib) { mesh.name = nameAttrib->value(); }

	//TODO: Remove when this debugging becomes redundant
	std::cout << "id: " << mesh.id << std::endl; 
	std::cout << "name: " << mesh.name << std::endl;
	std::cout << "---\n";

	// Traverse the mesh node to get to the source node.
	// Source nodes contain vertex, normals and texture co-ordinate data
	rapidxml::xml_node<> *sourceNode = meshNode->first_node("source");

	while (sourceNode != nullptr)
	{
		// Get the source node's id.
		char *sourceID = sourceNode->first_attribute("id")->value();
		FloatArray *floatArray = new FloatArray();

		// Get the float_array and technique_common nodes
		rapidxml::xml_node<> *floatArrayNode = sourceNode->first_node("float_array");
		rapidxml::xml_node<> *techniqueNode = sourceNode->first_node("technique_common");

		// Check we have a float array node then read it
		if( floatArrayNode && readFloatArray(*floatArrayNode, *techniqueNode, *floatArray) )
			sources.insert( std::pair<std::string, FloatArray*>(sourceID, floatArray) );
		else
			delete floatArray;

		sourceNode = sourceNode->next_sibling("source");
	}

	// Collada has this seemingly useless vertices node which in all cases that
	// I've seen just gives the positions float array an alias.
	// TODO: nullptr checks
	rapidxml::xml_node<> *verticesNode = meshNode->first_node("vertices");
	rapidxml::xml_node<> *verticesInputNode = verticesNode->first_node("input");

	// Source references are prepended with '#', we'll ignore it when constructing the string
	std::string verticesSource( verticesInputNode->first_attribute("source")->value()+1 );
	std::string verticesID( verticesNode->first_attribute("id")->value() );

	sources.insert( std::pair<std::string, FloatArray*>(verticesID, sources[verticesSource]) );

	rapidxml::xml_node<> *polylistNode = meshNode->first_node("polylist");

	readPolyList(*polylistNode, mesh, sources);

	std::cout << "test";
}

bool readFloatArray(rapidxml::xml_node<> &floatArrayNode, rapidxml::xml_node<> &techniqueNode, FloatArray &destination)
{	
	// Iterate through the attributes to extract id and count
	// Store these in the destination FloatArray object
	rapidxml::xml_attribute<> *curAttrib = floatArrayNode.first_attribute();
	while (curAttrib != nullptr)
	{
		std::string attribName = curAttrib->name();

		if(attribName == "id")
			destination.id = curAttrib->value();
		else if(attribName == "count")
			destination.count = std::atoi(curAttrib->value());

		curAttrib = curAttrib->next_attribute();
	}

	// Create a string stream from the node's value
	// So we can extract the floats.
	std::stringstream stream(floatArrayNode.value());
	destination.data = new float[destination.count];

	// Attempt to read floats from the string stream 
	// there should be as many as count defined.
	for (unsigned int i = 0; i < destination.count; i++)
	{
		// Check that we read the float succesfully
		// if not the float_array is corrupt and
		// probably didn't contain as many floats
		// as it said it did.
		if ( !(stream >> destination.data[i]) )
		{
			// Error!
			// Free up allocated memory
			delete[] destination.data;
			// Set data to nullptr to indicate an error
			destination.data = nullptr;
			// Nothing more to do.
			return false;
		}
	}

	destination.stride = std::atoi(techniqueNode.first_node("accessor")->first_attribute("stride")->value());

	return true;
}

// Assumes triangulated.
bool readPolyList(rapidxml::xml_node<> &polyListNode, Mesh &mesh, const std::map<std::string, FloatArray*> &sources)
{
	// We set the size to 4, there shouldn't be more than 4?
	FloatArray *orderedSources[4] = {nullptr};
	float **orderedOutput[4] = {nullptr};
	unsigned int inputsCount = 0;

	// Calculate the number of vertices in the polylist
	// Assuming the mesh is triangulated, the number of vertices is 3 * poly count
	unsigned int vertexCount = 3 * std::atoi(polyListNode.first_attribute("count")->value());

	// Iterate through the input nodes.
	rapidxml::xml_node<> *curInputNode = polyListNode.first_node("input");

	while (curInputNode)
	{
		// extract the semantic and source attributes
		
		rapidxml::xml_attribute<> *sourceAttribute = curInputNode->first_attribute("source");
		rapidxml::xml_attribute<> *semanticAttribute = curInputNode->first_attribute("semantic");
		rapidxml::xml_attribute<> *offsetAttribute = curInputNode->first_attribute("offset");
		// Add 1 to the char pointer to skip the prepended '#' on this source references
		std::string source( sourceAttribute->value()+1 );
		std::string semantic( semanticAttribute->value() );
		unsigned int offset = std::atoi(offsetAttribute->value());

		orderedSources[offset] = sources.at(source);

		//TODO: Explain this.
		if(semantic == "VERTEX")
		{
			mesh.numVerts = vertexCount;
			orderedOutput[offset] = &(mesh.verts);
		}
		else if(semantic == "NORMAL")
		{
			mesh.numNorms = vertexCount;
			orderedOutput[offset] = &(mesh.norms);
			mesh.hasNorms = true;
		}
		else if(semantic == "TEXCOORD")
		{
			mesh.numTexCoords = vertexCount;
			orderedOutput[offset] = &(mesh.texCoords);
			mesh.hasTexCoords = true;
		}

		*orderedOutput[offset] = new float[vertexCount * orderedSources[offset]->stride];

		inputsCount++;
		curInputNode = curInputNode->next_sibling("input");
	}

	// 'p' node stores all the indices
	// Access the p node and use the indexes to create a non-indexed
	// mesh to store in the mesh object
	rapidxml::xml_node<> *pNode = polyListNode.first_node("p");

	// Create a string stream from the node's value
	// So we can extract the floats.
	std::stringstream stream(pNode->value());

	unsigned int curIndex;
	unsigned int numIndexes = vertexCount * inputsCount;
	for (unsigned int i = 0; i < numIndexes; i++)
	{
		if (stream >> curIndex)
		{
			FloatArray *curFloatArray = orderedSources[i % inputsCount];
			float *curOutput = *(orderedOutput[i % inputsCount]);

			for(unsigned int j = 0; j < curFloatArray->stride; j++)
			{
				unsigned int &stride = curFloatArray->stride;
				curOutput[(i / inputsCount)*stride + j] = curFloatArray->data[curIndex*stride + j];
			}
		}
		else
		{
			// TODO: some cleanup
			return false;
		}
	}

	return true;
}

} // Namespace ltcimp