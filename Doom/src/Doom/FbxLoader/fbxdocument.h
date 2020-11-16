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
		float* verteces = nullptr;
		uint32_t normalsSize = 0;
		float* normals = nullptr;
		uint32_t uvSize = 0;
		float* uv = nullptr;
		uint32_t uvIndexSize = 0;
		uint32_t* uvIndex = nullptr;
		uint32_t vertecesSizeForNormals = 0;
		float* vertecesForNormals = nullptr;
		uint32_t uvSizeForVert = 0;
		float* uvForVert = nullptr;
		float* tangent = nullptr;
		float* btangent = nullptr;
    };

	Doom::Mesh* LoadMesh(std::string name,std::string filepath);

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
    std::uint32_t version;
};

} // namespace fbx

#endif // FBXDOCUMENT_H
