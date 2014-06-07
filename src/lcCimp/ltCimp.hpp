#ifndef LTCIMP_HPP
#define LTCIMP_HPP

#include "rapidxml.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <iostream>

namespace ltcimp
{
	void parse(const std::string &text);
	void extractMesh(rapidxml::xml_node<> *geomNode);
	void parseSourceNode(rapidxml::xml_node<> *sourceNode);
	void parseSourceData(const std::string &id, unsigned int accessorCount, unsigned int accessorStride, 
			std::string *paramNames, unsigned int paramNamesCount, 
			std::string *paramTypes, unsigned int paramTypesCount);
	void parseVectorData(const std::string &data, unsigned int count, std::string paramNames[3]);

	void parse(const std::string &text)
	{

		// Create a non-const copy of the string
		char *buffer = new char[text.size() + 1];
		strcpy(buffer, text.c_str());

		//std::cout << buffer;

		rapidxml::xml_document<> doc;
		doc.parse<0>(buffer);

		// TO-DO checks
		rapidxml::xml_node<> *node = doc.first_node("COLLADA");
		node = node->first_node("library_geometries");
		node = node->first_node("geometry");

		extractMesh(node);

		std::cout << "";
	}

	void extractMesh(rapidxml::xml_node<> *geomNode)
	{
		std::string geomID;
		std::string geomName;

		// Iterate through the attributes to extract id and name
		rapidxml::xml_attribute<> *curAttrib = geomNode->first_attribute();
		while (curAttrib != nullptr)
		{
			std::string attribName = curAttrib->name();

			if(attribName == "id")
				geomID = curAttrib->value();
			else if(attribName == "name")
				geomName = curAttrib->value();

			curAttrib = curAttrib->next_attribute();
		}

		std::cout << "id: " << geomID << std::endl; 
		std::cout << "name: " << geomName << std::endl;
		std::cout << "---\n";

		// The mesh node holds all the vertex in "source" nodes
		rapidxml::xml_node<> *curMeshSubNode = geomNode->first_node("mesh");
		
		if (curMeshSubNode)
		{
			curMeshSubNode = curMeshSubNode->first_node();

			while (curMeshSubNode != nullptr)
			{
				if ( strcmp(curMeshSubNode->name(),"source") == 0 )
					parseSourceNode(curMeshSubNode);

				curMeshSubNode = curMeshSubNode->next_sibling();
			}
		}
		else
		{
			// Corrupt
		}
	}

	void parseSourceNode(rapidxml::xml_node<> *sourceNode)
	{
		std::string sourceID;
		std::string strArrValue;
		std::string arrID;
		unsigned int arrCount = 0;
		std::string *paramNames = nullptr;
		std::string *paramTypes = nullptr;

		// Iterate through the attributes to extract id
		rapidxml::xml_attribute<> *curAttrib = sourceNode->first_attribute();
		while (curAttrib != nullptr)
		{
			std::string attribName = curAttrib->name();

			if(attribName == "id")
				sourceID = curAttrib->value();

			curAttrib = curAttrib->next_attribute();
		}

		std::cout << sourceID << std::endl;

		// The mesh node holds all the vertex in "source" nodes
		rapidxml::xml_node<> *curSubNode = sourceNode->first_node();
		
		while (curSubNode != nullptr)
		{
			if ( strcmp(curSubNode->name(), "float_array") == 0 )
			{
				arrCount = 0;
				strArrValue = curSubNode->value();

				// Iterate through the float_array's attributes to get id and count
				rapidxml::xml_attribute<> *curArrAttrib = curSubNode->first_attribute();
				while (curArrAttrib != nullptr)
				{
					std::string attribName = curArrAttrib->name();

					if(attribName == "id")
						arrID = curArrAttrib->value();
					else if(attribName == "count")
						arrCount = atoi(curArrAttrib->value());

					curArrAttrib = curArrAttrib->next_attribute();
				}

				std::cout << "id: " << arrID << std::endl;
				std::cout << "count: " << arrCount << std::endl;
				std::cout << strArrValue << std::endl;
			}
			else if ( strcmp(curSubNode->name(), "technique_common") == 0 )
			{
				// The only sub node we care about is the accessor node.
				rapidxml::xml_node<> *accessorNode = curSubNode->first_node("accessor");
				std::string accessorSource;
				unsigned int accessorStride = 0;
				unsigned int accessorCount = 0;

				if (accessorNode)
				{
					// Iterate through the accessor's attributes to get source, count and stride.
					rapidxml::xml_attribute<> *curAccessorAttrib = accessorNode->first_attribute();
					while (curAccessorAttrib != nullptr)
					{
						std::string attribName = curAccessorAttrib->name();

						if(attribName == "source")
							accessorSource = curAccessorAttrib->value();
						else if(attribName == "count")
							accessorCount = atoi(curAccessorAttrib->value());
						else if(attribName == "stride")
							accessorStride = atoi(curAccessorAttrib->value());

						curAccessorAttrib = curAccessorAttrib->next_attribute();
					}

					rapidxml::xml_node<> *curAccessorSubNode = accessorNode->first_node();
					int paramIndex = 0; 
					paramNames = new std::string[accessorStride];
					paramTypes = new std::string[accessorStride];

					while (curAccessorSubNode != nullptr)
					{
						if ( strcmp(curAccessorSubNode->name(), "param") == 0 )
						{
							rapidxml::xml_attribute<> *paramNameAttrib = curAccessorSubNode->first_attribute("name");
							if (paramNameAttrib) 
								paramNames[paramIndex] = paramNameAttrib->value(); 
							else
								paramNames[paramIndex] = "";

							rapidxml::xml_attribute<> *paramTypeAttrib = curAccessorSubNode->first_attribute("type");
							if (paramTypeAttrib) 
								paramTypes[paramIndex] = paramTypeAttrib->value();
							else
								paramTypes[paramIndex] = "";

							paramIndex++;
						}

						curAccessorSubNode = curAccessorSubNode->next_sibling();
					}

					for (int i = 0; i < accessorStride; i++)
					{
						std::cout << paramNames[i] << ", ";
						std::cout << paramTypes[i] << std::endl;
					}

					std::cout << "Accessor source: " << accessorSource << std::endl;
					std::cout << "Accessor count: " << accessorCount << std::endl;
					std::cout << "Accessor stride: " << accessorStride << std::endl;

					delete[] paramTypes;

				}
				else
				{
					// Corrupt
				}
			}

			curSubNode = curSubNode->next_sibling();
		}

		parseVectorData(strArrValue, arrCount, paramNames);
		std::cout << "---\n";
		delete[] paramNames;
	}

	void parseVectorData(const std::string &data, unsigned int count, std::string paramNames[3])
	{
		// Make sure the data is divisible by 3
		if(count % 3 == 0)
		{
			// Tokenize the string
			std::string buf;
			std::stringstream stream(data);

			std::string *tokenizedData = new std::string[count];

			// We know how many tokens there should be,
			// if we find less exit the function.
			for(int i = 0; i < count; i++)
			{
				if(stream >> buf)
				{
					tokenizedData[i] = buf;
				}
				else
				{
					// Error
					return;
				}
			}

			int elementOrder[3];

			// Based on the order of paramNames, we work out the order
			// to place the elements into the vectors.
			for (int i = 0; i < 3; i++)
			{
				if (paramNames[i] == "X" || paramNames[i] == "S") { elementOrder[i] = 0; } 
				else if (paramNames[i] == "Y" || paramNames[i] == "T") { elementOrder[i] = 1; } 
				else if (paramNames[i] == "Z" || paramNames[i] == "P") { elementOrder[i] = 2; } 
			}

			float x, y, z;

			for (int i = 0; i < count; i += 3)
			{
				x = std::stof(tokenizedData[i + elementOrder[0]]);
				y = std::stof(tokenizedData[i + elementOrder[1]]);
				z = std::stof(tokenizedData[i + elementOrder[2]]);

				std::cout << "<" << x << ", " << y << ", " << z << ">\n";
			}
		}
		else
		{
			// Data doesn't fit!
		}
	}

}

#endif //LTCIMP_HPP