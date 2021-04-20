#include "pch.h"
#include "OBJLoader.h"

Doom::Mesh* objl::Loader::Load(const std::string& filePath)
{
	objl::Loader loader;
	bool loadout = loader.LoadFile(filePath);
	if (loadout)
	{
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];
			Doom::Mesh* mesh = new Doom::Mesh(curMesh.MeshName, filePath);
			mesh->m_VertAttribSize = curMesh.Vertices.size() * 17;
			mesh->m_VertAttrib = new float[mesh->m_VertAttribSize];
			size_t count = 0;
			for (size_t j = 0; j < mesh->m_VertAttribSize; j += 17)
			{
				mesh->m_VertAttrib[j + 0] = curMesh.Vertices[count].Position.X;
				mesh->m_VertAttrib[j + 1] = curMesh.Vertices[count].Position.Y;
				mesh->m_VertAttrib[j + 2] = curMesh.Vertices[count].Position.Z;
				mesh->m_VertAttrib[j + 3] = curMesh.Vertices[count].Normal.X;
				mesh->m_VertAttrib[j + 4] = curMesh.Vertices[count].Normal.Y;
				mesh->m_VertAttrib[j + 5] = curMesh.Vertices[count].Normal.Z;
				mesh->m_VertAttrib[j + 6] = curMesh.Vertices[count].TextureCoordinate.X;
				mesh->m_VertAttrib[j + 7] = curMesh.Vertices[count].TextureCoordinate.Y;
				//color
				mesh->m_VertAttrib[j + 8] = 1.0f;
				mesh->m_VertAttrib[j + 9] = 1.0f;
				mesh->m_VertAttrib[j + 10] = 1.0f;
				count++;
			}

			float* verteces = mesh->m_VertAttrib;
			for (size_t j = 0; j < mesh->m_VertAttribSize; j += 17 * 3)
			{
				glm::vec3 tangent, btangent;

				glm::vec3 pos1 = glm::vec3(verteces[j + 0], verteces[j + 1], verteces[j + 2]);
				glm::vec3 pos2 = glm::vec3(verteces[j + 17 + 0], verteces[j + 17 + 1], verteces[j + 17 + 2]);
				glm::vec3 pos3 = glm::vec3(verteces[j + 17 * 2 + 0], verteces[j + 17 * 2 + 1], verteces[j + 17 * 2 + 2]);

				glm::vec2 uv1 = glm::vec2(verteces[j + 6], verteces[j + 7]);
				glm::vec2 uv2 = glm::vec2(verteces[j + 17 + 6], verteces[j + 17 + 7]);
				glm::vec2 uv3 = glm::vec2(verteces[j + 17 * 2 + 6], verteces[j + 17 * 2 + 7]);

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

				for (size_t k = 0; k < 3; k++)
				{
					verteces[j + 17 * k + 11] = tangent.x;
					verteces[j + 17 * k + 12] = tangent.y;
					verteces[j + 17 * k + 13] = tangent.z;
					verteces[j + 17 * k + 14] = btangent.x;
					verteces[j + 17 * k + 15] = btangent.y;
					verteces[j + 17 * k + 16] = btangent.z;
				}
			}

			mesh->m_IndicesSize = curMesh.Indices.size();
			mesh->m_Indices = new unsigned int[mesh->m_IndicesSize];
			for (size_t j = 0; j < mesh->m_IndicesSize; j++)
			{
				mesh->m_Indices[j] = curMesh.Indices[j];
			}
			return mesh;
			Doom::Logger::Success("has been loaded!", "Mesh", curMesh.MeshName.c_str());
		}
	}
}
