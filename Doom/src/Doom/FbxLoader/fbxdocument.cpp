#include "fbxdocument.h"
#include "fbxutil.h"

using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::uint32_t;
using std::uint8_t;

namespace fbx {
	Mesh * FBXDocument::LoadMesh(std::string* name,std::string filepath)
	{
		try
		{
			read(filepath);
			Mesh* mesh = new Mesh(*name);
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
												mesh->normals = new double[mesh->normalsSize];
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
							}
						}
					}
				}
			}
			mesh->vertecesSizeForNormals = mesh->indicesSize * 3;
			mesh->vertecesForNormals = new double[mesh->vertecesSizeForNormals];
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
			mesh->meshSize = mesh->vertecesSizeForNormals + mesh->normalsSize;
			mesh->mesh = new double[mesh->meshSize];
			uint32_t counter = 0;
			uint32_t normalIndex = 0;
			uint32_t vertecesIndex = 0;
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
				else {
					counter = 0;
					i--;
				}

			}
			delete[] mesh->verteces;
			delete[] mesh->vertecesForNormals;
			delete[] mesh->indices;
			delete[] mesh->normals;
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
