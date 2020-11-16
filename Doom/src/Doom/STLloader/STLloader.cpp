#include "../pch.h"
#include "STLloader.h"
#include <fstream>

using namespace Doom;

Mesh * StlLoader::LoadMesh(std::string name, std::string filePath)
{
	if (StlFileHasASCIIFormat(filePath)) {
		return ReadFromSTLASCII(name,filePath);
		//std::cout << "ASCII\n";
	}
	else {
		return ReadFromSTLBinary(name,filePath);
		//std::cout << "Binary\n";
	}
}

Mesh* Doom::StlLoader::ReadFromSTLASCII(std::string name, std::string filePath)
{
	std::vector<float> normals;
	std::vector<float> verteces;
	std::ifstream in(filePath);
	std::string _name;
	in >> _name;
	uint32_t indicesSize = 0;
	while (!(in.eof() || in.fail()))
	{
		std::string token;
		in >> token;
		if (token == "normal") {
			for (uint32_t i = 0; i < 3; i++)
			{
				std::string value;
				in >> value;
				normals.push_back(std::stod(value));
			}
		}
		if (token == "vertex") {
			indicesSize++;
			for (uint32_t i = 0; i < 3; i++)
			{
				std::string value;
				in >> value;
				verteces.push_back(std::stod(value) * 1.f);
			}
			verteces.push_back(normals[normals.size() - 3]);
			verteces.push_back(normals[normals.size() - 2]);
			verteces.push_back(normals[normals.size() - 1]);
			for (uint32_t j = 0; j < 8; j++)
			{
				verteces.push_back(0);
			}
		}
	}
	in.close();
	Mesh* mesh = new Mesh(name,filePath);
	mesh->meshSize = verteces.size();
	mesh->mesh = new float[mesh->meshSize];
	mesh->indicesSize = indicesSize;
	mesh->indices = new uint32_t[mesh->indicesSize];
	for (uint32_t i = 0; i < mesh->meshSize; i++)
	{
		mesh->mesh[i] = verteces[i];
		
	}
	for (uint32_t i = 0; i < indicesSize; i++)
	{
		mesh->indices[i] = i;
	}
	verteces.clear();
	normals.clear();
	return mesh;
}

Mesh* Doom::StlLoader::ReadFromSTLBinary(std::string name, std::string filePath)
{
	return nullptr;
}

bool Doom::StlLoader::StlFileHasASCIIFormat(std::string filePath)
{
	std::ifstream in(filePath);
	std::string firstWord;
	in >> firstWord;
	transform(firstWord.begin(), firstWord.end(), firstWord.begin(), ::tolower);

	return firstWord.compare("solid") == 0;
	in.close();
}