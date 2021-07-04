#include "../pch.h"
#include "SceneSerializer.h"
#include "World.h"
#include "../Objects/SkyBox.h"
#include "../Core/Utils.h"
#include "Core/Editor.h"
#include "Components/ParticleEmitter.h"

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
	YAML::Emitter out;

	out << YAML::BeginMap;

	SerializeCamera(out);

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

	DeSerializeCamera(data);

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
	Logger::Success("has been loaded!", "Scene", s_CurrentSceneFilePath.c_str());
}

void Doom::SceneSerializer::DeSerializeGameObject(YAML::detail::iterator_value& in, std::map<GameObject*, std::vector<int>>& childs)
{
	std::string name = in["Name"].as<std::string>();
	std::string tag = in["Tag"].as<std::string>();
	int id = in["GameObject"].as<int>();

	if (DeSerializeSkyBox(in)) return;

	GameObject* obj = GameObject::Create(name);
	obj->m_Id = id;
	obj->m_Tag = tag;

	DeSerializeTransformComponent(in, &obj->m_ComponentManager);
	DeSerializeRenderer3DComponent(in, &obj->m_ComponentManager);
	DeSerializeSpriteRendererComponent(in, &obj->m_ComponentManager);
	DeSerializeDirectionalLightComponent(in, &obj->m_ComponentManager);
	DeSerializePointLightComponent(in, &obj->m_ComponentManager);
	DeSerializeSpotLightComponent(in, &obj->m_ComponentManager);
	DeSerializeSphereColliderComponent(in, &obj->m_ComponentManager);
	DeSerializeRectangularCollider(in, &obj->m_ComponentManager);
	DeSerializeParticleEmitterComponent(in, &obj->m_ComponentManager);
	DeSerializeScriptComponent(in, &obj->m_ComponentManager);
	DeSerializeGameObjectChilds(in, obj, childs);

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

void Doom::SceneSerializer::DeSerializeTextureAtlas(YAML::detail::iterator_value& in)
{
	std::string name = in["Texture atlas"].as<std::string>();
	std::string texture = in["Texture"].as<std::string>();
	float width = in["Sprite width"].as<float>();
	float height = in["Sprite height"].as<float>();
	TextureAtlas::CreateTextureAtlas(name, width, height, Texture::Create(texture));
}

void Doom::SceneSerializer::DeSerializeCamera(YAML::Node& in)
{
	auto cam = in["Camera"];
	Camera& camera = Window::GetInstance().GetCamera();
	Camera::CameraTypes type = (Camera::CameraTypes)cam["Type"].as<int>();
	if (type == Camera::CameraTypes::PERSPECTIVE)
		camera.SetFov(cam["FOV"].as<float>());
	else
		camera.Zoom(cam["Zoom"].as<float>());
	auto camTransform = cam["Transform"];
	camera.SetPosition(camTransform["Position"].as<glm::vec3>());
	camera.SetRotation(camTransform["Rotation"].as<glm::vec3>());
}

bool Doom::SceneSerializer::DeSerializeSkyBox(YAML::detail::iterator_value& in)
{
	auto renderer3DComponent = in["Renderer3D"];
	if (renderer3DComponent)
	{
		bool isSkyBox = renderer3DComponent["SkyBox"].as<bool>();
		if (isSkyBox)
		{
			std::vector<std::string>faces = renderer3DComponent["Faces"].as<std::vector<std::string>>();
			GameObject* skybox = SkyBox::CreateSkyBox(faces);
			return true;
		}
	}
	return false;
}

void Doom::SceneSerializer::DeSerializeTransformComponent(YAML::detail::iterator_value& in, ComponentManager* cm)
{
	auto transformComponent = in["Transform"];
	if (transformComponent)
	{
		Transform* tr = cm->GetComponent<Transform>();
		tr->Translate(transformComponent["Position"].as<glm::vec3>());
		tr->Scale(transformComponent["Scale"].as<glm::vec3>());
		tr->RotateOnce(transformComponent["Rotation"].as<glm::vec3>(), true);
	}
}

void Doom::SceneSerializer::DeSerializeRenderer3DComponent(YAML::detail::iterator_value& in, ComponentManager* cm)
{
	auto renderer3DComponent = in["Renderer3D"];
	if (renderer3DComponent)
	{
		Renderer3D* r = cm->AddComponent<Renderer3D>();

		auto mat = renderer3DComponent["Material"];
		r->m_Material.m_Ambient = mat["Ambient"].as<float>();
		r->m_Material.m_Specular = mat["Specular"].as<float>();
		r->m_Color = mat["Color"].as<glm::vec4>();

		r->m_Shader = Shader::Get(Utils::GetNameFromFilePath(renderer3DComponent["Shader"].as<std::string>(), 6));

		bool isTransparent = renderer3DComponent["Transparent"].as<bool>();
		if (isTransparent)
			r->MakeTransparent();
		else 
			r->MakeSolid();

		r->m_Emissive = renderer3DComponent["Emissive"].as<bool>();
		r->m_IsCastingShadows = renderer3DComponent["Casting shadows"].as<bool>();
		r->m_IsWireMesh = renderer3DComponent["Wire mesh"].as<bool>();
		r->m_IsUsingNormalMap = renderer3DComponent["Use normal map"].as<bool>();
		r->m_IsCullingFace = renderer3DComponent["Is culling face"].as<bool>();
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
		auto meshIdTemp = mesh["Mesh Id"];
		uint32_t meshId = 0;
		if (meshIdTemp)
			meshId = meshIdTemp.as<uint32_t>();
		std::string meshName = Utils::GetNameFromFilePath(meshPath);
		Mesh::AsyncLoad(meshName, meshPath, meshId);
		meshName = meshId > 0 ? meshName.append(std::to_string(meshId)) : meshName;
		r->m_RenderTechnic = ((Renderer3D::RenderTechnic)renderer3DComponent["Render technic"].as<int>());
		Mesh::AsyncGet([=](Mesh* m) {
			r->LoadMesh(m);
			r->ChangeRenderTechnic(r->m_RenderTechnic);
			}, meshName);
		YAML::Node uniformf = renderer3DComponent["Uniforms float"];
		for (YAML::const_iterator it = uniformf.begin(); it != uniformf.end(); ++it)
		{
			r->m_FloatUniforms.insert(std::make_pair(it->first.as<std::string>(), it->second.as<float>()));
		}

	}
}

void Doom::SceneSerializer::DeSerializeSpriteRendererComponent(YAML::detail::iterator_value& in, ComponentManager* cm)
{
	auto spriteRendererComponent = in["Sprite renderer"];
	if (spriteRendererComponent)
	{
		SpriteRenderer* sr = cm->AddComponent<SpriteRenderer>();
		sr->m_Color = spriteRendererComponent["Color"].as<glm::vec4>();
		sr->m_Texture = Texture::Create(spriteRendererComponent["Texture"].as<std::string>());
		sr->m_Emissive = spriteRendererComponent["Emissive"].as<bool>();
		std::vector<float> vert = spriteRendererComponent["Vertices"].as<std::vector<float>>();
		for (uint32_t i = 0; i < 16; i++)
		{
			sr->m_Mesh2D[i] = vert[i];
		}
	}
}

void Doom::SceneSerializer::DeSerializeDirectionalLightComponent(YAML::detail::iterator_value& in, ComponentManager* cm)
{
	auto dirLightComponent = in["Directional light"];
	if (dirLightComponent)
	{
		DirectionalLight* dl = cm->AddComponent<DirectionalLight>();
		dl->m_Intensity = dirLightComponent["Intensity"].as<float>();
		dl->m_Color = dirLightComponent["Color"].as<glm::vec3>();
	}
}

void Doom::SceneSerializer::DeSerializePointLightComponent(YAML::detail::iterator_value& in, ComponentManager* cm)
{
	auto pointLightComponent = in["Point light"];
	if (pointLightComponent)
	{
		PointLight* pl = cm->AddComponent<PointLight>();
		auto atenuation = pointLightComponent["Attenuation"];
		pl->m_Constant = atenuation["Constant"].as<float>();
		pl->m_Linear = atenuation["Linear"].as<float>();
		pl->m_Quadratic = atenuation["Quadratic"].as<float>();
		pl->m_Color = pointLightComponent["Color"].as<glm::vec3>();
	}
}

void Doom::SceneSerializer::DeSerializeSpotLightComponent(YAML::detail::iterator_value& in, ComponentManager* cm)
{
	auto spotLightComponent = in["Spot light"];
	if (spotLightComponent)
	{
		SpotLight* sl = cm->AddComponent<SpotLight>();
		sl->m_InnerCutOff = spotLightComponent["InnerCutOff"].as<float>();
		sl->m_OuterCutOff = spotLightComponent["OuterCutOff"].as<float>();
		sl->m_Color = spotLightComponent["Color"].as<glm::vec3>();
		auto atenuation = spotLightComponent["Attenuation"];
		sl->m_Constant = atenuation["Constant"].as<float>();
		sl->m_Linear = atenuation["Linear"].as<float>();
		sl->m_Quadratic = atenuation["Quadratic"].as<float>();
	}
}

void Doom::SceneSerializer::DeSerializeSphereColliderComponent(YAML::detail::iterator_value& in, ComponentManager* cm)
{
	auto sphereColliderComponent = in["Sphere collider"];
	if (sphereColliderComponent)
	{
		SphereCollider* sc = cm->AddComponent<SphereCollider>();
		sc->m_IsInBoundingBox = sphereColliderComponent["Inside of bounding box"].as<bool>();
	}
}

void Doom::SceneSerializer::DeSerializeRectangularCollider(YAML::detail::iterator_value& in, ComponentManager* cm)
{
	auto rectangularCollider2DComponent = in["Rectangle collider 2D"];
	if (rectangularCollider2DComponent)
	{
		RectangleCollider2D* rc = cm->AddComponent<RectangleCollider2D>();
		rc->m_Enable = rectangularCollider2DComponent["Is Enable"].as<bool>();
		rc->m_IsTrigger = rectangularCollider2DComponent["Is trigger"].as<bool>();
		glm::vec3 offset = rectangularCollider2DComponent["OffSet"].as<glm::vec3>();
		rc->SetOffset(offset.x, offset.y);
		glm::vec3 scale = rectangularCollider2DComponent["Scale"].as<glm::vec3>();
	}
}

void Doom::SceneSerializer::DeSerializeParticleEmitterComponent(YAML::detail::iterator_value& in, ComponentManager* cm)
{
	auto particleEmitterComponent = in["Particle emitter"];
	if (particleEmitterComponent)
	{
		ParticleEmitter* pe = cm->AddComponent<ParticleEmitter>();
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
}

void Doom::SceneSerializer::DeSerializeScriptComponent(YAML::detail::iterator_value& in, ComponentManager* cm)
{
	auto scriptComponent = in["Scripts"];
	if (scriptComponent)
	{
		size_t size = scriptComponent["Size"].as<size_t>();
		for (size_t i = 0; i < size; i++)
		{
			std::string filePath = scriptComponent[("Script" + std::to_string(i))].as<std::string>();
			cm->AddComponent<ScriptComponent>()->AssignScript(filePath.c_str());
		}
	}
}

void Doom::SceneSerializer::DeSerializeGameObjectChilds(YAML::detail::iterator_value& in, GameObject* go, std::map<GameObject*, std::vector<int>>& childs)
{
	auto childsId = in["Childs id"];
	if (childsId)
	{
		std::vector<int> ids = childsId.as<std::vector<int>>();
		childs.insert(std::make_pair(go, ids));
	}
}

void Doom::SceneSerializer::SerializeCamera(YAML::Emitter& out)
{
	Camera& camera = Window::GetInstance().GetCamera();
	out << YAML::Key << "Camera";
	out << YAML::BeginMap;
	out << YAML::Key << "Transform";
	out << YAML::BeginMap;
	out << YAML::Key << "Position" << YAML::Value << camera.GetPosition();
	out << YAML::Key << "Rotation" << YAML::Value << camera.GetRotation();
	out << YAML::EndMap;
	out << YAML::Key << "Zoom" << YAML::Value << camera.m_ZoomLevel;
	out << YAML::Key << "FOV" << YAML::Value << camera.m_Fov;
	out << YAML::Key << "Type" << YAML::Value << (int)camera.m_Type;
	out << YAML::EndMap;
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
	out << YAML::Key << "Tag" << YAML::Value << go->m_Tag;
	ComponentManager* cm = &go->m_ComponentManager;

	SerializeTransformComponent(out, cm);
	SerializeRenderer3DComponent(out, cm);
	SerializeDirectionalLightComponent(out, cm);
	SerializePointLightComponent(out, cm);
	SerializeSpotLightComponent(out, cm);
	SerializeSpriteRendererComponent(out, cm);
	SerializeSphereColliderComponent(out, cm);
	SerializeGameObjectChilds(out, go);
	SerializeRegisteredEvents(out, go);
	SerializeRectangularCollider(out, cm);
	SerializeParticleEmitterComponent(out, cm);
	SerializeScriptComponent(out, cm);
	//SerializeCubeColliderComponent(out, cm);

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
	Transform* tr = cm->GetComponent<Transform>();
	if (tr != nullptr)
	{
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
	Renderer3D* r = cm->GetComponent<Renderer3D>();
	if (r != nullptr) 
	{
		
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
			out << YAML::Key << "Emissive" << YAML::Value << r->m_Emissive;
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
	SpriteRenderer* sr = cm->GetComponent<SpriteRenderer>();
	if (sr != nullptr) 
	{
		if (typeid(*sr) == typeid(SpriteRenderer)) 
		{
			out << YAML::Key << "Sprite renderer";
			out << YAML::BeginMap;
			out << YAML::Key << "Emissive" << YAML::Value << sr->m_Emissive;
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
	DirectionalLight* dl = cm->GetComponent<DirectionalLight>();
	if (dl != nullptr)
	{
		out << YAML::Key << "Directional light";
		out << YAML::BeginMap;
		out << YAML::Key << "Intensity" << YAML::Value << dl->m_Intensity;
		out << YAML::Key << "Color" << YAML::Value << dl->m_Color;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializePointLightComponent(YAML::Emitter& out, ComponentManager* cm)
{
	PointLight* pl = cm->GetComponent<PointLight>();
	if (pl != nullptr)
	{
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

void Doom::SceneSerializer::SerializeSpotLightComponent(YAML::Emitter& out, ComponentManager* cm)
{
	SpotLight* sl = cm->GetComponent<SpotLight>();
	if (sl != nullptr)
	{
		out << YAML::Key << "Spot light";
		out << YAML::BeginMap;
		out << YAML::Key << "Color" << YAML::Value << sl->m_Color;
		out << YAML::Key << "InnerCutOff" << YAML::Value << sl->m_InnerCutOff;
		out << YAML::Key << "OuterCutOff" << YAML::Value << sl->m_OuterCutOff;
		out << YAML::Key << "Attenuation";
		out << YAML::BeginMap;
		out << YAML::Key << "Constant" << YAML::Value << sl->m_Constant;
		out << YAML::Key << "Linear" << YAML::Value << sl->m_Linear;
		out << YAML::Key << "Quadratic" << YAML::Value << sl->m_Quadratic;
		out << YAML::EndMap;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeCubeColliderComponent(YAML::Emitter& out, ComponentManager* cm)
{
	CubeCollider3D* cc = cm->GetComponent<CubeCollider3D>();
	if (cc != nullptr) 
	{
		out << YAML::Key << "Cube collider";
		out << YAML::BeginMap;
		out << YAML::Key << "Bounding box" << YAML::Value << cc->m_IsBoundingBox;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeSphereColliderComponent(YAML::Emitter& out, ComponentManager* cm)
{
	SphereCollider* sc = cm->GetComponent<SphereCollider>();
	if (sc != nullptr) 
	{
		out << YAML::Key << "Sphere collider";
		out << YAML::BeginMap;
		out << YAML::Key << "Inside of bounding box" << YAML::Value << sc->m_IsInBoundingBox;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeRegisteredEvents(YAML::Emitter& out, GameObject* go)
{
	//out << YAML::Key << "Registered events" << YAML::Value << go->m_RegisteredEvents;
}

void Doom::SceneSerializer::SerializeRectangularCollider(YAML::Emitter& out, ComponentManager* cm)
{
	RectangleCollider2D* rc = cm->GetComponent<RectangleCollider2D>();
	if (rc != nullptr) 
	{
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

#include "Lua/LuaState.h"

void Doom::SceneSerializer::SerializeScriptComponent(YAML::Emitter& out, ComponentManager* cm)
{
	std::vector<ScriptComponent*> scripts = cm->GetScripts();
	out << YAML::Key << "Scripts";
	out << YAML::BeginMap;
	out << YAML::Key << "Size" << YAML::Value << scripts.size();
	for (size_t i = 0; i < scripts.size(); i++)
	{
		
		out << YAML::Key << "Script" + std::to_string(i) << YAML::Value << "src/Scripts/" + Utils::GetNameFromFilePath(scripts[i]->m_LState->m_FilePath, -1);
	}
	out << YAML::EndMap;
}
