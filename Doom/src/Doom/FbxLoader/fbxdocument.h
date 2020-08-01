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
