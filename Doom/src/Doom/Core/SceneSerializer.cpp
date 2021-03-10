#include "../pch.h"
#include "SceneSerializer.h"
#include "World.h"
#include "../Objects/SkyBox.h"
#include "../Core/Utils.h"
#include "Core/Editor.h"
#include "ParticleSystem/ParticleEmitter.h"

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec2& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, float* v) 
{
	out << YAML::Flow;
	out << YAML::BeginSeq <<
		v[0] << v[1] << v[2] << v[3] <<
		v[4] << v[5] << v[6] << v[7] <<
		v[8] << v[9] << v[10] << v[11] <<
		v[12] << v[13] << v[14] << v[15] <<
		YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec4& v) 
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

void Doom::SceneSerializer::Serialize(const std::string& filePath)
{
	Camera& camera = Window::GetInstance().GetCamera();
	s_CurrentSceneFilePath = filePath;
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene";
	out << YAML::Value << ((Application*)World::GetInstance().s_Application)->m_Type;
	out << YAML::Key << "Camera";
	out << YAML::BeginMap;
	out << YAML::Key << "Transform";
	out << YAML::BeginMap;
	out << YAML::Key << "Position" << YAML::Value << camera.GetPosition();
	out << YAML::Key << "Rotation" << YAML::Value << camera.GetRotation();
	out << YAML::EndMap;
	out << YAML::Key << "Zoom" << YAML::Value << camera.GetZoomLevel();
	out << YAML::Key << "FOV" << YAML::Value <<  camera.GetFOV();
	out << YAML::EndMap;

	out << YAML::Key << "GameObjects";
	out << YAML::Value << YAML::BeginSeq;

	for (uint32_t i = 0; i < World::GetInstance().s_GameObjects.size(); i++)
	{
		if (World::GetInstance().s_GameObjects[i]->m_IsSerializable)
			SerializeGameObject(out, World::GetInstance().s_GameObjects[i]);
	}

	out << YAML::EndSeq;

	out << YAML::Key << "Texture atlases";
	out << YAML::Value << YAML::BeginSeq;
	for (auto iter = TextureAtlas::s_TextureAtlases.begin(); iter != TextureAtlas::s_TextureAtlases.end(); iter++)
	{
		SerializeTextureAtlas(out, iter->second);
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream fout(filePath);
	fout << out.c_str();
}

void Doom::SceneSerializer::DeSerialize(const std::string& filePath)
{
	s_CurrentSceneFilePath = filePath;
	std::ifstream stream(filePath);
	std::stringstream strStream;

	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["Scene"])
		return;

	RenderType type = (RenderType)data["Scene"].as<int>();

	auto cam = data["Camera"];
	auto camTransform = cam["Transform"];
	glm::vec3 position = camTransform["Position"].as<glm::vec3>();
	glm::vec3 rotation = camTransform["Rotation"].as<glm::vec3>();
	float fov = cam["FOV"].as<float>();
	double zoom = cam["Zoom"].as<float>();
	Camera& camera = Window::GetInstance().GetCamera();
	if (type == RenderType::TYPE_3D)
		camera.SetFov(fov);
	else
		camera.Zoom(zoom);
	camera.SetPosition(position);
	camera.SetRotation(rotation);

	auto textureAtlases = data["Texture atlases"];
	if (textureAtlases) 
	{
		for (auto ta : textureAtlases)
		{
			DeSerializeTextureAtlas(ta);
		}
	}

	std::map<GameObject*, std::vector<int>> childs;

	auto gameobjects = data["GameObjects"];
	if (gameobjects)
	{
		for (auto go : gameobjects)
		{
			DeSerializeGameObject(go, childs);
		}
	}

	for (auto iter = childs.begin(); iter != childs.end(); iter++)
	{
		for (uint32_t i = 0; i < iter->second.size(); i++)
		{
			GameObject* ch = World::GetInstance().s_GameObjects[iter->second[i]];
			iter->first->AddChild((void*)ch);
		}
	}
	std::cout << BOLDGREEN << "Scene: <" << NAMECOLOR << s_CurrentSceneFilePath << BOLDGREEN << "> has been loaded\n" << RESET;
}

void Doom::SceneSerializer::DeSerializeGameObject(YAML::detail::iterator_value& go, std::map<GameObject*, std::vector<int>>& childs)
{
	//uint64_t id = go["GameObject"].as<uint64_t>();
	std::string name = go["Name"].as<std::string>();
	int id = go["GameObject"].as<int>();
	auto renderer3DComponent = go["Renderer3D"];
	if (renderer3DComponent)
	{
		bool isSkyBox = renderer3DComponent["SkyBox"].as<bool>();
		if (isSkyBox) 
		{
			std::vector<std::string>faces = renderer3DComponent["Faces"].as<std::vector<std::string>>();
			GameObject* skybox = SkyBox::CreateSkyBox(faces);
			return;
		}
	}

	GameObject* obj = GameObject::Create(name);
	obj->m_Id = id;

	auto transformComponent = go["Transform"];
	if (transformComponent)
	{
		Transform* tr = &(obj->m_Transform);
		glm::vec3 position = transformComponent["Position"].as<glm::vec3>();
		glm::vec3 scale = transformComponent["Scale"].as<glm::vec3>();
		glm::vec3 rotation = transformComponent["Rotation"].as<glm::vec3>();
		tr->Translate(position.x, position.y, position.z);
		tr->Scale(scale.x, scale.y, scale.z);
		tr->RotateOnce(rotation.x, rotation.y, rotation.z, true);
	}

	if (renderer3DComponent)
	{
		Renderer3D* r = obj->m_ComponentManager.AddComponent<Renderer3D>();
		auto mat = renderer3DComponent["Material"];
		r->m_Material.m_Ambient = mat["Ambient"].as<float>();
		r->m_Material.m_Specular = mat["Specular"].as<float>();
		r->m_Color = mat["Color"].as<glm::vec4>();
		r->m_Shader = Shader::Get(Utils::GetNameFromFilePath(renderer3DComponent["Shader"].as<std::string>(), 6));
		bool isTransparent = renderer3DComponent["Transparent"].as<bool>();
		if (isTransparent) r->MakeTransparent();
		else r->MakeSolid();
		r->m_IsCastingShadows = renderer3DComponent["Casting shadows"].as<bool>();
		r->m_IsWireMesh = renderer3DComponent["Wire mesh"].as<bool>();
		r->m_IsUsingNormalMap = renderer3DComponent["Use normal map"].as<bool>();
		if (renderer3DComponent["Is culling face"])
			r->m_IsCullingFace = renderer3DComponent["Is culling face"].as<bool>();
		if (renderer3DComponent["Is renderable"])
			r->m_IsRenderable = renderer3DComponent["Is renderable"].as<bool>();
		auto textures = renderer3DComponent["Textures"];
		std::string diffuse = textures["Diffuse"].as<std::string>();
		std::string normal = textures["Normal"].as<std::string>();
		if (diffuse == "WhiteTexture")
			r->m_DiffuseTexture = Texture::s_WhiteTexture;
		else 
		{
			r->m_DiffuseTexture = Texture::Create(diffuse);
			//Texture::AsyncLoadTexture(diffuse);
			//Texture::GetAsync(obj, [=] {
			//	Texture* t = Texture::Get(diffuse, false);
			//	if (t != nullptr)
			//		r->m_DiffuseTexture = t;
			//	return t;
			//	});
		}
		if (normal == "InvalidTexture")
			r->m_NormalMapTexture = Texture::Get("InvalidTexture");
		else 
		{
			r->m_NormalMapTexture = Texture::Create(normal);
			/*Texture::AsyncLoadTexture(normal);
			Texture::GetAsync(r, [=] {
				Texture* t = Texture::Get(normal, false);
				if (t != nullptr)
					r->m_NormalMapTexture = t;
				return t;
				});*/
		}
		auto mesh = renderer3DComponent["Mesh"];
		std::string meshPath = mesh["Mesh"].as<std::string>();
		auto _meshId = mesh["Mesh Id"];
		uint32_t meshId = 0;
		if (_meshId)
			meshId = _meshId.as<uint32_t>();
		std::string meshName = Utils::GetNameFromFilePath(meshPath);
		MeshManager::GetInstance().AsyncLoadMesh(meshName, meshPath, meshId);
		meshName = meshId > 0 ? meshName.append(std::to_string(meshId)) : meshName;
		r->m_RenderTechnic = ((Renderer3D::RenderTechnic)renderer3DComponent["Render technic"].as<int>());
		MeshManager::GetInstance().GetMeshWhenLoaded(meshName, (void*)r);
		YAML::Node uniformf = renderer3DComponent["Uniforms float"];
		for (YAML::const_iterator it = uniformf.begin(); it != uniformf.end(); ++it)
		{
			r->m_FloatUniforms.insert(std::make_pair(it->first.as<std::string>(), it->second.as<float>()));
		}

	}

	auto dirLightComponent = go["Directional light"];
	if (dirLightComponent) 
	{
		DirectionalLight* dl = obj->m_ComponentManager.AddComponent<DirectionalLight>();
		dl->m_Intensity = dirLightComponent["Intensity"].as<float>();
		dl->m_Color = dirLightComponent["Color"].as<glm::vec3>();
	}

	auto pointLightComponent = go["Point light"];
	if (pointLightComponent) 
	{
		PointLight* pl = obj->m_ComponentManager.AddComponent<PointLight>();
		auto atenuation = pointLightComponent["Attenuation"];
		pl->m_Constant = atenuation["Constant"].as<float>();
		pl->m_Linear = atenuation["Linear"].as<float>();
		pl->m_Quadratic = atenuation["Quadratic"].as<float>();
		pl->m_Color = pointLightComponent["Color"].as<glm::vec3>();
	}

	auto sphereColliderComponent = go["Sphere collider"];
	if (sphereColliderComponent)
	{
		SphereCollider* sc = obj->m_ComponentManager.AddComponent<SphereCollider>();
		sc->m_IsInBoundingBox = sphereColliderComponent["Inside of bounding box"].as<bool>();
	}

	auto spriteRendererComponent = go["Sprite renderer"];
	if (spriteRendererComponent) 
	{
		SpriteRenderer* sr = obj->m_ComponentManager.AddComponent<SpriteRenderer>();
		sr->m_Color = spriteRendererComponent["Color"].as<glm::vec4>();
		sr->m_Texture = (Texture::Create(spriteRendererComponent["Texture"].as<std::string>()));
		std::vector<float> vert = spriteRendererComponent["Vertices"].as<std::vector<float>>();
		for (uint32_t i = 0; i < 16; i++)
		{
			sr->m_Mesh2D[i] = vert[i];
		}
	}

	auto rectangularCollider2DComponent = go["Rectangle collider 2D"];
	if (rectangularCollider2DComponent)
	{
		RectangleCollider2D* rc = obj->m_ComponentManager.AddComponent<RectangleCollider2D>();
		rc->m_Enable = rectangularCollider2DComponent["Is Enable"].as<bool>();
		rc->m_IsTrigger = rectangularCollider2DComponent["Is trigger"].as<bool>();
		glm::vec3 offset = rectangularCollider2DComponent["OffSet"].as<glm::vec3>();
		rc->SetOffset(offset.x, offset.y);
		glm::vec3 scale = rectangularCollider2DComponent["Scale"].as<glm::vec3>();
	}

	auto particleEmitterComponent = go["Particle emitter"];
	if (particleEmitterComponent)
	{
		ParticleEmitter* pe = obj->m_ComponentManager.AddComponent<ParticleEmitter>();
		pe->Init(particleEmitterComponent["Particles amount"].as<size_t>());
		pe->m_Dir[0] = particleEmitterComponent["DirX"].as<glm::vec2>();
		pe->m_Dir[1] = particleEmitterComponent["DirY"].as<glm::vec2>();
		pe->m_Dir[2] = particleEmitterComponent["DirZ"].as<glm::vec2>(); 
		pe->m_RadiusToSpawn[0] = particleEmitterComponent["RadiusToSpawnX"].as<glm::vec2>();
		pe->m_RadiusToSpawn[1] = particleEmitterComponent["RadiusToSpawnY"].as<glm::vec2>();
		pe->m_RadiusToSpawn[2] = particleEmitterComponent["RadiusToSpawnZ"].as<glm::vec2>();
		pe->m_MaxTimeToLive = particleEmitterComponent["Max time to live"].as<float>();
		pe->m_Speed = particleEmitterComponent["Speed"].as<float>();
		pe->m_TimeToSpawn = particleEmitterComponent["Time to spawn"].as<glm::vec2>();
		pe->m_Scale = particleEmitterComponent["Scale"].as<glm::vec2>();
		pe->m_Color = particleEmitterComponent["Color"].as<glm::vec4>();
	}

	auto childsId = go["Childs id"];
	if (childsId) 
	{
		std::vector<int> ids = childsId.as<std::vector<int>>();
		childs.insert(std::make_pair(obj, ids));
	}

	//@deprecated
	/*auto rEvents = go["Registered events"];
	if (rEvents) {
		std::vector<int> events = rEvents.as<std::vector<int>>();
		for (uint32_t i = 0; i < events.size(); i++)
		{
			EventSystem::GetInstance()->RegisterClient((EventType)events[i], obj);
		}
	}*/
}

void Doom::SceneSerializer::DeSerializeTextureAtlas(YAML::detail::iterator_value& ta)
{
	std::string name = ta["Texture atlas"].as<std::string>();
	std::string texture = ta["Texture"].as<std::string>();
	float width = ta["Sprite width"].as<float>();
	float height = ta["Sprite height"].as<float>();
	TextureAtlas::CreateTextureAtlas(name, width, height, Texture::Create(texture));
}

void Doom::SceneSerializer::SerializeTextureAtlas(YAML::Emitter& out, TextureAtlas* ta)
{
	out << YAML::BeginMap;
	out << YAML::Key << "Texture atlas" << YAML::Value << ta->m_Name;
	out << YAML::Key << "Texture" << YAML::Value << ta->GetTexture()->m_FilePath;
	out << YAML::Key << "Sprite width" << YAML::Value << ta->GetSpriteWidth();
	out << YAML::Key << "Sprite height" << YAML::Value << ta->GetSpriteHeight();
	out << YAML::EndMap;
}

void Doom::SceneSerializer::SerializeGameObject(YAML::Emitter& out, GameObject* go)
{
	out << YAML::BeginMap;
	out << YAML::Key << "GameObject" << YAML::Value << std::to_string(go->m_Id);
	out << YAML::Key << "Name" << YAML::Value << go->m_Name;
	ComponentManager* cm = &go->m_ComponentManager;

	SerializeTransformComponent(out, cm);
	SerializeRenderer3DComponent(out, cm);
	SerializeDirectionalLightComponent(out, cm);
	SerializePointLightComponent(out, cm);
	SerializeSpriteRendererComponent(out, cm);
	SerializeCubeColliderComponent(out, cm);
	SerializeSphereColliderComponent(out, cm);
	SerializeGameObjectChilds(out, go);
	SerializeRegisteredEvents(out, go);
	SerializeRectangularCollider(out, cm);
	SerializeParticleEmitterComponent(out, cm);

	out << YAML::EndMap;
}

void Doom::SceneSerializer::SerializeGameObjectChilds(YAML::Emitter& out, GameObject* go)
{
	if (go->GetChilds().size() > 0 && go->m_IsSerializalbeChilds) 
	{
		out << YAML::Key << "Childs id";
		std::vector<int> ids;
		for (uint32_t i = 0; i < go->GetChilds().size(); i++)
		{
			ids.push_back(static_cast<GameObject*>(go->GetChilds()[i])->m_Id);
		}
		out << YAML::Value << ids;
	}
}

void Doom::SceneSerializer::SerializeTransformComponent(YAML::Emitter& out, ComponentManager* cm)
{
	if (cm->GetComponent<Transform>() != nullptr)
	{
		Transform* tr = cm->GetComponent<Transform>();
		out << YAML::Key << "Transform";
		out << YAML::BeginMap;
		out << YAML::Key << "Position" << YAML::Value << tr->GetPosition();
		out << YAML::Key << "Scale" << YAML::Value << tr->GetScale();
		out << YAML::Key << "Rotation" << YAML::Value << tr->GetRotation();
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeRenderer3DComponent(YAML::Emitter& out, ComponentManager* cm)
{
	if (cm->GetComponent<Renderer3D>() != nullptr) 
	{
		Renderer3D* r = cm->GetComponent<Renderer3D>();
		if (typeid(*r) == typeid(Renderer3D)) 
		{
			out << YAML::Key << "Renderer3D";
			out << YAML::BeginMap;

			out << YAML::Key << "Material";
			out << YAML::BeginMap;
			out << YAML::Key << "Ambient" << YAML::Value << r->m_Material.m_Ambient;
			out << YAML::Key << "Specular" << YAML::Value << r->m_Material.m_Specular;
			out << YAML::Key << "Color" << YAML::Value << r->m_Color;
			out << YAML::EndMap;

			out << YAML::Key << "Shader" << YAML::Value << r->m_Shader->m_FilePath;
			out << YAML::Key << "Render technic" << YAML::Value << (int)r->m_RenderTechnic;
			out << YAML::Key << "Transparent" << YAML::Value << r->m_IsTransparent;
			out << YAML::Key << "SkyBox" << YAML::Value << r->m_IsSkyBox;
			out << YAML::Key << "Casting shadows" << YAML::Value << r->m_IsCastingShadows;
			out << YAML::Key << "Wire mesh" << YAML::Value << r->m_IsWireMesh;
			out << YAML::Key << "Is renderable" << YAML::Value << r->m_IsRenderable;
			out << YAML::Key << "Use normal map" << YAML::Value << r->m_IsUsingNormalMap;
			out << YAML::Key << "Is culling face" << YAML::Value << r->m_IsCullingFace;

			if (r->m_IsSkyBox)
			{
				out << YAML::Key << "Faces";
				out << YAML::BeginMap;
				out << YAML::Flow;
				out << YAML::BeginSeq << SkyBox::s_Faces[0] << SkyBox::s_Faces[1] << SkyBox::s_Faces[2] << SkyBox::s_Faces[3] << SkyBox::s_Faces[4] << SkyBox::s_Faces[5] << YAML::EndSeq;
				out << YAML::EndMap;
			}

			if (r->m_FloatUniforms.size() > 0)
			{
				out << YAML::Key << "Uniforms float";
				//out << YAML::BeginMap;
				out << YAML::Value << r->m_FloatUniforms;
				//out << YAML::EndMap;
			}

			out << YAML::Key << "Textures";
			out << YAML::BeginMap;
			out << YAML::Key << "Diffuse" << YAML::Value << (r->m_DiffuseTexture != nullptr ? r->m_DiffuseTexture->m_FilePath : "WhiteTexture");
			out << YAML::Key << "Normal" << YAML::Value << (r->m_NormalMapTexture != nullptr ? r->m_NormalMapTexture->m_FilePath : "InvalidTexture");
			out << YAML::EndMap;

			out << YAML::Key << "Mesh";
			out << YAML::BeginMap;
			out << YAML::Key << "Mesh" << YAML::Value << (r->m_Mesh != nullptr ? r->m_Mesh->m_FilePath : "InvalidMesh");
			out << YAML::Key << "Mesh Id" << YAML::Value << (r->m_Mesh != nullptr ? r->m_Mesh->m_IdOfMeshInFile : 0);
			out << YAML::EndMap;

			out << YAML::EndMap;
		}
	}
}

void Doom::SceneSerializer::SerializeSpriteRendererComponent(YAML::Emitter& out, ComponentManager* cm)
{
	if (cm->GetComponent<SpriteRenderer>() != nullptr) 
	{
		SpriteRenderer* sr = cm->GetComponent<SpriteRenderer>();
		if (typeid(*sr) == typeid(SpriteRenderer)) 
		{
			out << YAML::Key << "Sprite renderer";
			out << YAML::BeginMap;
			out << YAML::Key << "Color" << YAML::Value << sr->m_Color;
			out << YAML::Key << "Texture" << YAML::Value << (sr->m_Texture != nullptr ? sr->m_Texture->m_FilePath : "InvalidTexture");
			out << YAML::Key << "Texture atlas" << YAML::Value << (sr->m_TextureAtlas != nullptr ? sr->m_TextureAtlas->GetTexture()->m_FilePath : "InvalidTexture");;
			std::vector<float> vert;
			for (uint32_t i = 0; i < 16; i++)
			{
				vert.push_back(sr->m_Mesh2D[i]);
			}
			out << YAML::Key << "Vertices" << YAML::Value << vert;
			out << YAML::EndMap;
		}
	}
}

void Doom::SceneSerializer::SerializeDirectionalLightComponent(YAML::Emitter& out, ComponentManager* cm)
{
	if (cm->GetComponent<DirectionalLight>() != nullptr)
	{
		DirectionalLight* dl = cm->GetComponent<DirectionalLight>();
		out << YAML::Key << "Directional light";
		out << YAML::BeginMap;
		out << YAML::Key << "Intensity" << YAML::Value << dl->m_Intensity;
		out << YAML::Key << "Color" << YAML::Value << dl->m_Color;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializePointLightComponent(YAML::Emitter& out, ComponentManager* cm)
{
	if (cm->GetComponent<PointLight>() != nullptr)
	{
		PointLight* pl = cm->GetComponent<PointLight>();
		out << YAML::Key << "Point light";
		out << YAML::BeginMap;
		out << YAML::Key << "Color" << YAML::Value << pl->m_Color;
		out << YAML::Key << "Attenuation";
		out << YAML::BeginMap;
		out << YAML::Key << "Constant" << YAML::Value << pl->m_Constant;
		out << YAML::Key << "Linear" << YAML::Value << pl->m_Linear;
		out << YAML::Key << "Quadratic" << YAML::Value << pl->m_Quadratic;
		out << YAML::EndMap;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeCubeColliderComponent(YAML::Emitter& out, ComponentManager* cm)
{
	if (cm->GetComponent<CubeCollider3D>() != nullptr) 
	{
		CubeCollider3D* cc = cm->GetComponent<CubeCollider3D>();
		out << YAML::Key << "Cube collider";
		out << YAML::BeginMap;
		out << YAML::Key << "Bounding box" << YAML::Value << cc->m_IsBoundingBox;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeSphereColliderComponent(YAML::Emitter& out, ComponentManager* cm)
{
	if (cm->GetComponent<SphereCollider>() != nullptr) 
	{
		SphereCollider* cc = cm->GetComponent<SphereCollider>();
		out << YAML::Key << "Sphere collider";
		out << YAML::BeginMap;
		out << YAML::Key << "Inside of bounding box" << YAML::Value << cc->m_IsInBoundingBox;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeRegisteredEvents(YAML::Emitter& out, GameObject* go)
{
	//out << YAML::Key << "Registered events" << YAML::Value << go->m_RegisteredEvents;
}

void Doom::SceneSerializer::SerializeRectangularCollider(YAML::Emitter& out, ComponentManager* cm)
{
	if (cm->GetComponent<RectangleCollider2D>() != nullptr) 
	{
		RectangleCollider2D* rc = cm->GetComponent<RectangleCollider2D>();
		out << YAML::Key << "Rectangle collider 2D";
		out << YAML::BeginMap;
		out << YAML::Key << "Is Enable" << YAML::Value << rc->m_Enable;
		out << YAML::Key << "Is trigger" << YAML::Value << rc->m_IsTrigger;
		out << YAML::Key << "OffSet" << YAML::Value << glm::vec3(rc->m_Offset , 0);
		out << YAML::Key << "Scale" << YAML::Value << Utils::GetScale(glm::mat4(1.0f));
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeParticleEmitterComponent(YAML::Emitter& out, ComponentManager* cm)
{
	ParticleEmitter* pe = cm->GetComponent<ParticleEmitter>();
	if (pe != nullptr)
	{
		out << YAML::Key << "Particle emitter";
		out << YAML::BeginMap;
		out << YAML::Key << "Particles amount" << YAML::Value << pe->m_ParticlesPool.size();
		out << YAML::Key << "Max time to live" << YAML::Value << pe->m_MaxTimeToLive;
		out << YAML::Key << "Speed" << YAML::Value << pe->m_Speed;
		out << YAML::Key << "Color" << YAML::Value << pe->m_Color;
		out << YAML::Key << "Scale" << YAML::Value << pe->m_Scale;
		out << YAML::Key << "Time to spawn" << YAML::Value << pe->m_TimeToSpawn;
		out << YAML::Key << "DirX" << YAML::Value << pe->m_Dir[0];
		out << YAML::Key << "DirY" << YAML::Value << pe->m_Dir[1];
		out << YAML::Key << "DirZ" << YAML::Value << pe->m_Dir[2];
		out << YAML::Key << "RadiusToSpawnX" << YAML::Value << pe->m_RadiusToSpawn[0];
		out << YAML::Key << "RadiusToSpawnY" << YAML::Value << pe->m_RadiusToSpawn[1];
		out << YAML::Key << "RadiusToSpawnZ" << YAML::Value << pe->m_RadiusToSpawn[2];
		out << YAML::EndMap;
	}
}
