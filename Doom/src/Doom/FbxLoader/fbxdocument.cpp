#include "fbxdocument.h"
#include "fbxutil.h"
#include "glm/glm.hpp"

using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::uint32_t;
using std::uint8_t;

using namespace Doom;

namespace fbx {
	Mesh * FBXDocument::LoadMesh(std::string name,std::string filepath)
	{
		try
		{
			read(filepath);
			Mesh* mesh = new Mesh(name);
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
								//node.print();
								fbx::FBXNode nodeG = node.getChildren()[k];
								if (nodeG.getName() == "Vertices") {

									size_t sizeP = nodeG.properties.size();
									for (size_t l = 0; l < sizeP; l++)
									{
										mesh->vertecesSize = nodeG.properties[l].values.size();
										mesh->verteces = new float[mesh->vertecesSize];
										mesh->theHighestPoint = glm::vec3(0);
										mesh->theLowestPoint = glm::vec3(0);
										uint32_t tcount = 0;
										for (size_t a = 0; a < mesh->vertecesSize; a++)
										{
											mesh->verteces[a] = nodeG.properties[l].values[a].f64;
											if (tcount == 0) {
												if (mesh->theHighestPoint.x < mesh->verteces[a])
													mesh->theHighestPoint.x = mesh->verteces[a];
												else if (mesh->theLowestPoint.x > mesh->verteces[a])
													mesh->theLowestPoint.x = mesh->verteces[a];
												tcount++;
											}
											else if (tcount == 1) {
												if (mesh->theHighestPoint.y < mesh->verteces[a])
													mesh->theHighestPoint.y = mesh->verteces[a];
												else if (mesh->theLowestPoint.y > mesh->verteces[a])
													mesh->theLowestPoint.y = mesh->verteces[a];
												tcount++;
											}
											else if (tcount == 2) {
												if (mesh->theHighestPoint.z < mesh->verteces[a])
													mesh->theHighestPoint.z = mesh->verteces[a];
												else if (mesh->theLowestPoint.z > mesh->verteces[a])
													mesh->theLowestPoint.z = mesh->verteces[a];
												tcount = 0;
											}
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
								if (nodeG.getName() == "LayerElementNormal") {
									size_t sizeF = nodeG.getChildren().size();
									for (size_t m = 0; m < sizeF; m++)
									{
										FBXNode nodeE = nodeG.getChildren()[m];
										if (nodeE.getName() == "Normals") {
											size_t sizeP = nodeE.properties.size();
											for (size_t l = 0; l < sizeP; l++)
											{
												mesh->normalsSize = nodeE.properties[l].values.size();
												mesh->normals = new float[mesh->normalsSize];
												for (size_t a = 0; a < mesh->normalsSize; a++)
												{
													mesh->normals[a] = (nodeE.properties[l].values[a].f64);
													//std::cout << mesh->normals[a] << std::endl;
												}
											}
											break;
										}

									}
								}
								if (nodeG.getName() == "LayerElementUV") {
									//nodeG.print();
									size_t sizeF = nodeG.getChildren().size();
									for (size_t m = 0; m < sizeF; m++)
									{
										FBXNode nodeE = nodeG.getChildren()[m];
										if (nodeE.getName() == "UV") {
											size_t sizeP = nodeE.properties.size();
											for (size_t l = 0; l < sizeP; l++)
											{
												mesh->uvSize = nodeE.properties[l].values.size();
												mesh->uv = new float[mesh->uvSize];
												for (size_t a = 0; a < mesh->uvSize; a++)
												{
													mesh->uv[a] = (nodeE.properties[l].values[a].f64);
													//std::cout << mesh->normals[a] << std::endl;
												}
											}
										}
										if (nodeE.getName() == "UVIndex") {
											size_t sizeP = nodeE.properties.size();
											for (size_t l = 0; l < sizeP; l++)
											{
												mesh->uvIndexSize = nodeE.properties[l].values.size();
												mesh->uvIndex = new uint32_t[mesh->uvIndexSize];
												for (size_t a = 0; a < mesh->uvIndexSize; a++)
												{
													mesh->uvIndex[a] = (nodeE.properties[l].values[a].i32);
													//std::cout << mesh->uvIndex[a] << std::endl;
												}
											}
											break;
										}
									}
								}
							}
						}
					}
				}
			}
			mesh->vertecesSizeForNormals = mesh->indicesSize * 3;
			mesh->vertecesForNormals = new float[mesh->vertecesSizeForNormals];
			for (size_t i = 0; i < mesh->vertecesSizeForNormals; i += 3)
			{
				mesh->vertecesForNormals[i + 0] = mesh->verteces[mesh->indices[i / 3] * 3 + 0];
				mesh->vertecesForNormals[i + 1] = mesh->verteces[mesh->indices[i / 3] * 3 + 1];
				mesh->vertecesForNormals[i + 2] = mesh->verteces[mesh->indices[i / 3] * 3 + 2];
			}
			mesh->indicesForNormals = new uint32_t[mesh->indicesSize];
			for (size_t i = 0; i < mesh->indicesSize; i++)
			{
				mesh->indicesForNormals[i] = i;
			}

			mesh->uvSizeForVert = mesh->uvIndexSize * 2;
			mesh->uvForVert = new float[mesh->uvSizeForVert];
			for (size_t i = 0; i < mesh->uvSizeForVert; i += 2)
			{
				mesh->uvForVert[i + 0] = mesh->uv[mesh->uvIndex[i / 2] * 2 + 0];
				mesh->uvForVert[i + 1] = mesh->uv[mesh->uvIndex[i / 2] * 2 + 1];
			}

			uint32_t uvCounter = 0;
			mesh->tangent = new float[mesh->vertecesSizeForNormals];
			mesh->btangent = new float[mesh->vertecesSizeForNormals];
			for (size_t i = 0; i < mesh->vertecesSizeForNormals; i+=9)
			{
				glm::vec3 tangent, btangent;

				glm::vec3 pos1 = glm::vec3(mesh->vertecesForNormals[i + 0], mesh->vertecesForNormals[i + 1], mesh->vertecesForNormals[i + 2]);
				glm::vec3 pos2 = glm::vec3(mesh->vertecesForNormals[i + 3], mesh->vertecesForNormals[i + 4], mesh->vertecesForNormals[i + 5]);
				glm::vec3 pos3 = glm::vec3(mesh->vertecesForNormals[i + 6], mesh->vertecesForNormals[i + 7], mesh->vertecesForNormals[i + 8]);

				glm::vec2 uv1 = glm::vec2(mesh->uvForVert[uvCounter + 0], mesh->uvForVert[uvCounter + 1]);
				glm::vec2 uv2 = glm::vec2(mesh->uvForVert[uvCounter + 2], mesh->uvForVert[uvCounter + 3]);
				glm::vec2 uv3 = glm::vec2(mesh->uvForVert[uvCounter + 4], mesh->uvForVert[uvCounter + 5]);

				glm::vec3 edge1 = pos2 - pos1;
				glm::vec3 edge2 = pos3 - pos1;
				glm::vec2 deltaUV1 = uv2 - uv1;
				glm::vec2 deltaUV2 = uv3 - uv1;

				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

				tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
				tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
				tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
				tangent = glm::normalize(tangent);

				btangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
				btangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
				btangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
				btangent = glm::normalize(btangent);

				mesh->tangent[i + 0] = tangent.x;
				mesh->tangent[i + 1] = tangent.y;
				mesh->tangent[i + 2] = tangent.z;
				mesh->tangent[i + 3] = tangent.x;
				mesh->tangent[i + 4] = tangent.y;
				mesh->tangent[i + 5] = tangent.z;
				mesh->tangent[i + 6] = tangent.x;
				mesh->tangent[i + 7] = tangent.y;
				mesh->tangent[i + 8] = tangent.z;

				mesh->btangent[i + 0] = btangent.x;
				mesh->btangent[i + 1] = btangent.y;
				mesh->btangent[i + 2] = btangent.z;
				mesh->btangent[i + 3] = btangent.x;
				mesh->btangent[i + 4] = btangent.y;
				mesh->btangent[i + 5] = btangent.z;
				mesh->btangent[i + 6] = btangent.x;
				mesh->btangent[i + 7] = btangent.y;
				mesh->btangent[i + 8] = btangent.z;

				uvCounter += 6;
			}

			mesh->meshSize = mesh->vertecesSizeForNormals * 3 + mesh->normalsSize + mesh->uvSizeForVert;
			mesh->mesh = new float[mesh->meshSize];
			uint32_t counter = 0;
			uint32_t normalIndex = 0;
			uint32_t vertecesIndex = 0;
			uint32_t uvIndex = 0;
			uint32_t tangentIndex = 0;
			uint32_t btangentIndex = 0;
			for (size_t i = 0; i < mesh->meshSize; i++)
			{
				if (counter < 3) {
					mesh->mesh[i] = mesh->vertecesForNormals[vertecesIndex];
					vertecesIndex++;
					//std::cout << "vertex" << std::endl;
					counter++;
				}
				else if (counter < 6) {
					mesh->mesh[i] = mesh->normals[normalIndex];
					//std::cout << "normal" << std::endl;
					normalIndex++;
					counter++;
				}
				else if (counter < 8) {
					mesh->mesh[i] = mesh->uvForVert[uvIndex];
					//std::cout << "uv" << std::endl;
					uvIndex++;
					counter++;
				}
				else if (counter < 11) {
					mesh->mesh[i] = mesh->tangent[tangentIndex];
					//std::cout << "tangent" << std::endl;
					tangentIndex++;
					counter++;
				}
				else if (counter < 14) {
					mesh->mesh[i] = mesh->btangent[btangentIndex];
					//std::cout << "btangent" << std::endl;
					btangentIndex++;
					counter++;
				}
				else {
					counter = 0;
					i--;
				}

			}
			delete[] mesh->verteces;
			delete[] mesh->vertecesForNormals;
			delete[] mesh->indices;
			delete[] mesh->normals;
			//TODO!!! NEED TO FIX THIS
			//delete[] mesh->uv;
			delete[] mesh->uvForVert;
			return mesh;
		}
		catch (std::string e) {
			std::cout << e << std::endl;
			return nullptr;
		}
	}


	FBXDocument::FBXDocument()
{
    version = 7400;
}

void FBXDocument::read(string fname)
{
    ifstream file;

    // buffer
    int bufferSize = 1 << 16;
    char* buffer = new char[bufferSize];
    file.rdbuf()->pubsetbuf(buffer, bufferSize);

    file.open(fname, std::ios::in | std::ios::binary);
    if (file.is_open()) {
        read(file);
    } else {
        throw std::string("Cannot read from file: \"" + fname + "\"");
    }
    file.close();
}

void FBXDocument::write(string fname)
{
    ofstream file;

    // buffer
    int bufferSize = 1 << 16;
    char* buffer = new char[bufferSize];
    file.rdbuf()->pubsetbuf(buffer, bufferSize);

    file.open(fname, std::ios::out | std::ios::binary);
    if (file.is_open()) {
        write(file);
    } else {
        throw std::string("Cannot write to file: \"" + fname + "\"");
    }
    file.close();
}

bool checkMagic(Reader &reader)
{
    string magic("Kaydara FBX Binary  ");
    for(char c : magic) {
        if(reader.readUint8() != c) return false;
    }
    if(reader.readUint8() != 0x00) return false;
    if(reader.readUint8() != 0x1A) return false;
    if(reader.readUint8() != 0x00) return false;
    return true;
}

void FBXDocument::read(std::ifstream &input)
{
    Reader reader(&input);
    input >> std::noskipws;
    if(!checkMagic(reader)) throw std::string("Not a FBX file");

    uint32_t version = reader.readUint32();

    uint32_t maxVersion = 7400;
    if(version > maxVersion) throw "Unsupported FBX version "+std::to_string(version)
                            + " latest supported version is "+std::to_string(maxVersion);

    uint32_t start_offset = 27; // magic: 21+2, version: 4
    do{
        FBXNode node;
        start_offset += node.read(input, start_offset);
        if(node.isNull()) break;
        nodes.push_back(node);
    } while(true);
}

namespace {
    void writerFooter(Writer &writer) {
        uint8_t footer[] = {
            0xfa, 0xbc, 0xab, 0x09,
            0xd0, 0xc8, 0xd4, 0x66, 0xb1, 0x76, 0xfb, 0x83, 0x1c, 0xf7, 0x26, 0x7e, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xe8, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x5a, 0x8c, 0x6a,
            0xde, 0xf5, 0xd9, 0x7e, 0xec, 0xe9, 0x0c, 0xe3, 0x75, 0x8f, 0x29, 0x0b
        };
        for(unsigned int i = 0; i < sizeof(footer); i++) {
            writer.write(footer[i]);
        }
    }

}

void FBXDocument::write(std::ofstream &output)
{
    Writer writer(&output);
    writer.write("Kaydara FBX Binary  ");
    writer.write((uint8_t) 0);
    writer.write((uint8_t) 0x1A);
    writer.write((uint8_t) 0);
    writer.write(version);

    uint32_t offset = 27; // magic: 21+2, version: 4
    for(FBXNode node : nodes) {
        offset += node.write(output, offset);
    }
    FBXNode nullNode;
    offset += nullNode.write(output, offset);
    writerFooter(writer);
}

void FBXDocument::createBasicStructure()
{
    FBXNode headerExtension("FBXHeaderExtension");
    headerExtension.addPropertyNode("FBXHeaderVersion", (int32_t) 1003);
    headerExtension.addPropertyNode("FBXVersion", (int32_t) getVersion());
    headerExtension.addPropertyNode("EncryptionType", (int32_t) 0);
    {
        FBXNode creationTimeStamp("CreationTimeStamp");
        creationTimeStamp.addPropertyNode("Version", (int32_t) 1000);
        creationTimeStamp.addPropertyNode("Year", (int32_t) 2017);
        creationTimeStamp.addPropertyNode("Month", (int32_t) 5);
        creationTimeStamp.addPropertyNode("Day", (int32_t) 2);
        creationTimeStamp.addPropertyNode("Hour", (int32_t) 14);
        creationTimeStamp.addPropertyNode("Minute", (int32_t) 11);
        creationTimeStamp.addPropertyNode("Second", (int32_t) 46);
        creationTimeStamp.addPropertyNode("Millisecond", (int32_t) 917);
        headerExtension.addChild(creationTimeStamp);
    }
    headerExtension.addPropertyNode("Creator", "Blender (stable FBX IO) - 2.78 (sub 0) - 3.7.7");
    {
        FBXNode sceneInfo("SceneInfo");
        sceneInfo.addProperty(std::vector<uint8_t>({'G','l','o','b','a','l','I','n','f','o',0,1,'S','c','e','n','e','I','n','f','o'}), 'S');
        sceneInfo.addProperty("UserData");
        sceneInfo.addPropertyNode("Type", "UserData");
        sceneInfo.addPropertyNode("Version", 100);
        {
            FBXNode metadata("MetaData");
            metadata.addPropertyNode("Version", 100);
            metadata.addPropertyNode("Title", "");
            metadata.addPropertyNode("Subject", "");
            metadata.addPropertyNode("Author", "");
            metadata.addPropertyNode("Keywords", "");
            metadata.addPropertyNode("Revision", "");
            metadata.addPropertyNode("Comment", "");
            sceneInfo.addChild(metadata);
        }
        {
            FBXNode properties("Properties70");
            {
                FBXNode p("P");
                p.addProperty("DocumentUrl");
                p.addProperty("KString");
                p.addProperty("Url");
                p.addProperty("");
                p.addProperty("/foobar.fbx");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("SrcDocumentUrl");
                p.addProperty("KString");
                p.addProperty("Url");
                p.addProperty("");
                p.addProperty("/foobar.fbx");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("Original");
                p.addProperty("Compound");
                p.addProperty("");
                p.addProperty("");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("Original|ApplicationVendor");
                p.addProperty("KString");
                p.addProperty("");
                p.addProperty("");
                p.addProperty("Blender Foundation");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("Original|ApplicationName");
                p.addProperty("KString");
                p.addProperty("");
                p.addProperty("");
                p.addProperty("Blender (stable FBX IO)");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("Original|ApplicationVersion");
                p.addProperty("KString");
                p.addProperty("");
                p.addProperty("");
                p.addProperty("2.78 (sub 0)");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("Original|DateTime_GMT");
                p.addProperty("DateTime");
                p.addProperty("");
                p.addProperty("");
                p.addProperty("01/01/1970 00:00:00.000");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("Original|FileName");
                p.addProperty("KString");
                p.addProperty("");
                p.addProperty("");
                p.addProperty("/foobar.fbx");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("LastSaved");
                p.addProperty("Compound");
                p.addProperty("");
                p.addProperty("");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("LastSaved|ApplicationVendor");
                p.addProperty("KString");
                p.addProperty("");
                p.addProperty("");
                p.addProperty("Blender Foundation");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("LastSaved|ApplicationName");
                p.addProperty("KString");
                p.addProperty("");
                p.addProperty("");
                p.addProperty("Blender (stable FBX IO)");
                properties.addChild(p);
            }
            {
                FBXNode p("P");
                p.addProperty("LastSaved|DateTime_GMT");
                p.addProperty("DateTime");
                p.addProperty("");
                p.addProperty("");
                p.addProperty("01/01/1970 00:00:00.000");
                properties.addChild(p);
            }
            sceneInfo.addChild(properties);
        }
        headerExtension.addChild(sceneInfo);
    }
    nodes.push_back(headerExtension);


}

std::uint32_t FBXDocument::getVersion()
{
    return version;
}

void FBXDocument::print()
{
    cout << "{\n";
    cout << "  \"version\": " << getVersion() << ",\n";
    cout << "  \"children\": [\n";
    bool hasPrev = false;
    for(auto node : nodes) {
        if(hasPrev) cout << ",\n";
        node.print("    ");
        hasPrev = true;
    }
    cout << "\n  ]\n}" << endl;
}

} // namespace fbx
