#ifndef FBXDOCUMENT_H
#define FBXDOCUMENT_H

#include "fbxnode.h"
#include "../Core/Core.h"
#include "../Render/Mesh.h"
#include <string>

namespace fbx {

class DOOM_API FBXDocument
{
public:

	Mesh* LoadMesh(std::string filepath) {
		try
		{
			read(filepath);
			Mesh* mesh = new Mesh;
			size_t size = nodes.size();
			for (size_t i = 0; i < size; i++)
			{
				if (nodes[i].getName() == "Objects") {
					size_t sizec = nodes[i].getChildren().size();
					for (size_t j = 0; j < sizec; j++)
					{
						fbx::FBXNode node = nodes[i].getChildren()[j];
						if (node.getName() == "Geometry") {
							size_t sizeg = node.getChildren().size();
							for (size_t k = 0; k < sizeg; k++)
							{
								fbx::FBXNode nodeG = node.getChildren()[k];
								if (nodeG.getName() == "Vertices") {

									size_t sizeP = nodeG.properties.size();
									for (size_t l = 0; l < sizeP; l++)
									{
										mesh->vertecesSize = nodeG.properties[l].values.size();
										mesh->verteces = new double[mesh->vertecesSize];
										for (size_t a = 0; a < mesh->vertecesSize; a++)
										{
											mesh->verteces[a] = nodeG.properties[l].values[a].f64;
										}
									}
								}
								if (nodeG.getName() == "PolygonVertexIndex") {

									size_t sizeP = nodeG.properties.size();
									for (size_t l = 0; l < sizeP; l++)
									{
										mesh->indicesSize = nodeG.properties[l].values.size();
										mesh->indices = new uint32_t[mesh->indicesSize];
										for (size_t a = 0; a < mesh->indicesSize; a++)
										{
											if (nodeG.properties[l].values[a].i32 < 0)
												mesh->indices[a] = (-nodeG.properties[l].values[a].i32 - 1);
											else
												mesh->indices[a] = (nodeG.properties[l].values[a].i32);
										}
									}
								}
							}
						}
					}
				}
			}
			return mesh;
		}
		catch (std::string e) {
			std::cout << e << std::endl;
			return nullptr;
		}
	}

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
