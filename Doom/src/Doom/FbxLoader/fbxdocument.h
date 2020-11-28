#ifndef FBXDOCUMENT_H
#define FBXDOCUMENT_H

#include "fbxnode.h"
#include "../Core/Core.h"
#include <string>
#include "../Render/Mesh.h"

namespace fbx {

class DOOM_API FBXDocument
{
public:

    struct Data {
		uint32_t vertecesSize = 0;
		std::vector<float> verteces;
		uint32_t normalsSize = 0;
		std::vector<float> normals;
		uint32_t uvSize = 0;
		std::vector<float> uv;
		uint32_t uvIndexSize = 0;
		std::vector<uint32_t> uvIndex;
		uint32_t vertecesSizeForNormals = 0;
		std::vector<float> vertecesForNormals;
		uint32_t uvSizeForVert = 0;
		std::vector<float> uvForVert;
		std::vector<float> tangent;
		std::vector<float> btangent;
    };

	void LoadScene(std::string filepath);
	Doom::Mesh* LoadMesh(std::string name, std::string filepath, uint32_t meshId);

    FBXDocument();
    void read(std::ifstream &input);
    void read(std::string fname);
    void write(std::string fname);
    void write(std::ofstream &output);

    void createBasicStructure();

    std::vector<FBXNode> nodes;

    std::uint32_t getVersion();
    void print();

private:
	void LoadData(Data& data, FBXNode& fbxNode, Doom::Mesh* mesh);
	void GenerateMesh(Data& data, Doom::Mesh* mesh);
	std::string GetNameOfModel(std::string model, uint32_t offset = 1);
	std::string GetNameOfMesh(std::string model, uint32_t offset = 1);

    std::uint32_t version;
};

} // namespace fbx

#endif // FBXDOCUMENT_H
