#include "../pch.h"
#include "SceneSerializer.h"
#include "World.h"
#include "../Render/SkyBox.h"
#include "../Core/Utils.h"

namespace YAML {

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

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec3& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, float* v) {
	out << YAML::Flow;
	out << YAML::BeginSeq <<
		v[0]  << v[1]	<< v[2]	 << v[3] <<
		v[4]  << v[5]	<< v[6]	 << v[7] <<
		v[8]  << v[9]	<< v[10] << v[11] <<
		v[12] << v[13]	<< v[14] << v[15] <<
		YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec4& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

void Doom::SceneSerializer::Serialize(const std::string & filePath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene";
	out << YAML::Value << "Untitled";

	out << YAML::Key << "Camera";
	out << YAML::BeginMap;
	out << YAML::Key << "Transform";
	out << YAML::BeginMap;
	out << YAML::Key << "Position" << YAML::Value << Window::GetCamera().GetPosition();
	out << YAML::Key << "Rotation" << YAML::Value << Window::GetCamera().GetRotation();
	out << YAML::EndMap;
	out << YAML::Key << "FOV" << YAML::Value << Window::GetCamera().GetFOV();
	out << YAML::EndMap;

	out << YAML::Key << "GameObjects";
	out << YAML::Value << YAML::BeginSeq;

	for (uint32_t i = 0; i < World::objects.size(); i++)
	{
		if(World::objects[i]->isSerializable)
			SerializeGameObject(out, World::objects[i]);
	}

	out << YAML::EndSeq;

	out << YAML::Key << "Texture atlases";
	out << YAML::Value << YAML::BeginSeq;
	for (auto iter = TextureAtlas::textureAtlases.begin(); iter != TextureAtlas::textureAtlases.end(); iter++)
	{
		SerializeTextureAtlas(out, iter->second);
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream fout(filePath);
	fout << out.c_str();
}

void Doom::SceneSerializer::DeSerialize(const std::string & filePath)
{
	std::ifstream stream(filePath);
	std::stringstream strStream;

	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["Scene"])
		return;

	std::string sceneName = data["Scene"].as<std::string>();


	auto cam = data["Camera"];
	auto camTransform = cam["Transform"];
	glm::vec3 position = camTransform["Position"].as<glm::vec3>();
	glm::vec3 rotation = camTransform["Rotation"].as<glm::vec3>();
	float fov = cam["FOV"].as<float>();
	Camera& camera = Window::GetCamera();
	camera.SetFov(fov);
	camera.SetPosition(position);
	camera.SetRotation(rotation);

	auto textureAtlases = data["Texture atlases"];
	if (textureAtlases) {
		for (auto ta : textureAtlases)
		{
			DeSerializeTextureAtlas(ta);
		}
	}

	std::map<GameObject*, std::vector<int>> childs;

	auto gameobjects = data["GameObjects"];
	if (gameobjects) {
		for (auto go : gameobjects)
		{
			DeSerializeGameObject(go,childs);
		}
	}

	for (auto iter = childs.begin(); iter != childs.end(); iter++)
	{
		for (uint32_t i = 0; i < iter->second.size(); i++)
		{
			GameObject* ch = World::objects[iter->second[i]];
			ch->SetOwner((void*)iter->first);
			iter->first->AddChild((void*)ch);
		}
	}
}

void Doom::SceneSerializer::DeSerializeGameObject(YAML::detail::iterator_value & go, std::map<GameObject*, std::vector<int>>& childs)
{
	//uint64_t id = go["GameObject"].as<uint64_t>();
	std::string name = go["Name"].as<std::string>();
	int id = go["GameObject"].as<int>();
	auto renderer3DComponent = go["Renderer3D"];
	if (renderer3DComponent) {
		bool isSkyBox = renderer3DComponent["SkyBox"].as<bool>();
		if (isSkyBox) {
			std::vector<std::string>faces = renderer3DComponent["Faces"].as<std::vector<std::string>>();
			SkyBox* sb = new SkyBox(faces,nullptr);
			MeshManager::GetMeshWhenLoaded("cube", (void*)(sb->GetComponentManager()->GetComponent<Renderer3D>()));
			return;
		}
	}

	GameObject* obj = new GameObject(name);
	obj->id = id;

	auto transformComponent = go["Transform"];
	if (transformComponent) {
		Transform* tr = obj->GetComponent<Transform>();
		glm::vec3 position = transformComponent["Position"].as<glm::vec3>();
		glm::vec3 scale = transformComponent["Scale"].as<glm::vec3>();
		glm::vec3 rotation = transformComponent["Rotation"].as<glm::vec3>();
		tr->Translate(position.x, position.y, position.z);
		tr->Scale(scale.x, scale.y, scale.z);
		tr->RotateOnce(rotation.x, rotation.y, rotation.z, true);
	}

	if (renderer3DComponent) {
		Renderer3D* r = obj->GetComponentManager()->AddComponent<Renderer3D>();
		auto mat = renderer3DComponent["Material"];
		r->mat.ambient = mat["Ambient"].as<float>();
		r->mat.ambient = mat["Specular"].as<float>();
		r->shader = Shader::Get(Utils::GetNameFromFilePath(renderer3DComponent["Shader"].as<std::string>(), 6));
		r->ChangeRenderTechnic((Renderer3D::RenderTechnic)renderer3DComponent["Render technic"].as<int>());
		bool isTransparent = renderer3DComponent["Transparent"].as<bool>();
		if (isTransparent)
			r->MakeTransparent();
		else
			r->MakeSolid();
		r->isCastingShadows = renderer3DComponent["Casting shadows"].as<bool>();
		r->isWireMesh = renderer3DComponent["Wire mesh"].as<bool>();
		r->useNormalMap = renderer3DComponent["Use normal map"].as<bool>();
		auto textures = renderer3DComponent["Textures"];
		std::string diffuse = textures["Diffuse"].as<std::string>();
		std::string normal = textures["Normal"].as<std::string>();
		if (diffuse == "InvalidTexture")
			r->diffuseTexture = Texture::WhiteTexture;
		else {
			Texture::GetAsync(obj, [=] {
				Texture* t = Texture::Get(diffuse);
				if (t != nullptr)
					r->diffuseTexture = t;
				return t;
			});
		}
		if (normal == "InvalidTexture")
			r->normalMapTexture = Texture::Get("InvalidTexture");
		else {
			Texture::GetAsync(r, [=] {
				Texture* t = Texture::Get(normal);
				if (t != nullptr)
					r->normalMapTexture = t;
				return t;
			});
		}
		auto mesh = renderer3DComponent["Mesh"];
		std::string meshPath = mesh["Mesh"].as<std::string>();
		MeshManager::LoadMesh(Utils::GetNameFromFilePath(meshPath), meshPath);
		r->LoadMesh(MeshManager::GetMesh(Utils::GetNameFromFilePath(meshPath)));

		YAML::Node uniformf = renderer3DComponent["Uniforms float"];
		for (YAML::const_iterator it = uniformf.begin(); it != uniformf.end(); ++it)
		{
			r->floatUniforms.insert(std::make_pair(it->first.as<std::string>(),it->second.as<float>()));
		}
		
	}

	auto dirLightComponent = go["Directional light"];
	if (dirLightComponent) {
		DirectionalLight* dl = obj->GetComponentManager()->AddComponent<DirectionalLight>();
		dl->intensity = dirLightComponent["Intensity"].as<float>();
		dl->color = dirLightComponent["Color"].as<glm::vec3>();
	}

	auto pointLightComponent = go["Point light"];
	if (pointLightComponent) {
		PointLight* pl = obj->GetComponentManager()->AddComponent<PointLight>();
		auto atenuation = pointLightComponent["Attenuation"];
		pl->constant = atenuation["Constant"].as<float>();
		pl->linear = atenuation["Linear"].as<float>();
		pl->quadratic = atenuation["Quadratic"].as<float>();
		pl->color = pointLightComponent["Color"].as<glm::vec3>();
	}

	auto sphereColliderComponent = go["Sphere collider"];
	if (sphereColliderComponent) {
		SphereCollider* sc = obj->GetComponentManager()->AddComponent<SphereCollider>();
		sc->isInBoundingBox = sphereColliderComponent["Inside of bounding box"].as<bool>();
	}

	auto spriteRendererComponent = go["Sprite renderer"];
	if (spriteRendererComponent) {
		SpriteRenderer* sr = obj->GetComponentManager()->AddComponent<SpriteRenderer>();
		sr->color = spriteRendererComponent["Color"].as<glm::vec4>();
		sr->SetTexture(Texture::Create(spriteRendererComponent["Texture"].as<std::string>()));
		std::vector<float> vert = spriteRendererComponent["Vertices"].as<std::vector<float>>();
		for (uint32_t i = 0; i < 16; i++)
		{
			sr->mesh2D[i] = vert[i];
		}
	}

	auto childsId = go["Childs id"];
	if (childsId) {
		std::vector<int> ids = childsId.as<std::vector<int>>();
		childs.insert(std::make_pair(obj, ids));
	}

	auto rEvents = go["Registered events"];
	if (rEvents) {
		std::vector<int> events = rEvents.as<std::vector<int>>();
		for (uint32_t i = 0; i < events.size(); i++)
		{
			EventSystem::GetInstance()->RegisterClient((EventType)events[i], obj);
		}
	}
}

void Doom::SceneSerializer::DeSerializeTextureAtlas(YAML::detail::iterator_value & ta)
{
	std::string name = ta["Texture atlas"].as<std::string>();
	std::string texture = ta["Texture"].as<std::string>();
	float width = ta["Sprite width"].as<float>();
	float height = ta["Sprite height"].as<float>();
	TextureAtlas::CreateTextureAtlas(name,width,height,Texture::Create(texture));
}

void Doom::SceneSerializer::SerializeTextureAtlas(YAML::Emitter & out, TextureAtlas * ta)
{
	out << YAML::BeginMap;
	out << YAML::Key << "Texture atlas" << YAML::Value << ta->name;
	out << YAML::Key << "Texture" << YAML::Value << ta->GetTexture()->GetFilePath();
	out << YAML::Key << "Sprite width" << YAML::Value << ta->GetSpriteWidth();
	out << YAML::Key << "Sprite height" << YAML::Value << ta->GetSpriteHeight();
	out << YAML::EndMap;
}

void Doom::SceneSerializer::SerializeGameObject(YAML::Emitter & out, GameObject * go)
{
	out << YAML::BeginMap;
	out << YAML::Key << "GameObject" << YAML::Value << std::to_string(go->id);
	out << YAML::Key << "Name" << YAML::Value << go->name;
	ComponentManager* cm = go->GetComponentManager();

	SerializeTransformComponent(out, cm);
	SerializeRenderer3DComponent(out, cm);
	SerializeDirectionalLightComponent(out, cm);
	SerializePointLightComponent(out, cm);
	SerializeSpriteRendererComponent(out, cm);
	SerializeCubeColliderComponent(out, cm);
	SerializeSphereColliderComponent(out, cm);
	SerializeGameObjectChilds(out, go);
	SerializeRegisteredEvents(out, go);

	out << YAML::EndMap;
}

void Doom::SceneSerializer::SerializeGameObjectChilds(YAML::Emitter & out, GameObject * go)
{
	if (go->GetChilds().size() > 0) {
		out << YAML::Key << "Childs id";
		std::vector<int> ids;
		for (uint32_t i = 0; i < go->GetChilds().size(); i++)
		{
			ids.push_back(static_cast<GameObject*>(go->GetChilds()[i])->id);
		}
		out << YAML::Value << ids;
	}
}

void Doom::SceneSerializer::SerializeTransformComponent(YAML::Emitter & out, ComponentManager * cm)
{
	if (cm->GetComponent<Transform>() != nullptr) {
		Transform* tr = cm->GetComponent<Transform>();
		out << YAML::Key << "Transform";
		out << YAML::BeginMap;
		out << YAML::Key << "Position" << YAML::Value << tr->GetPosition();
		out << YAML::Key << "Scale" << YAML::Value << tr->GetScale();
		out << YAML::Key << "Rotation" << YAML::Value << tr->rotation;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeRenderer3DComponent(YAML::Emitter & out, ComponentManager * cm)
{
	if (cm->GetComponent<Renderer3D>() != nullptr) {
		Renderer3D* r = cm->GetComponent<Renderer3D>();
		if (typeid(*r) == typeid(Renderer3D)) {
			out << YAML::Key << "Renderer3D";
			out << YAML::BeginMap;

			out << YAML::Key << "Material";
			out << YAML::BeginMap;
			out << YAML::Key << "Ambient" << YAML::Value << r->mat.ambient;
			out << YAML::Key << "Specular" << YAML::Value << r->mat.specular;
			out << YAML::EndMap;

			out << YAML::Key << "Shader" << YAML::Value << r->shader->GetFilePath();
			out << YAML::Key << "Render technic" << YAML::Value << (int)r->renderTechnic;
			out << YAML::Key << "Transparent" << YAML::Value << r->isTransparent;
			out << YAML::Key << "SkyBox" << YAML::Value << r->isSkyBox;
			out << YAML::Key << "Casting shadows" << YAML::Value << r->isCastingShadows;
			out << YAML::Key << "Wire mesh" << YAML::Value << r->isWireMesh;
			out << YAML::Key << "Use normal map" << YAML::Value << r->useNormalMap;

			if (r->isSkyBox) {
				SkyBox* sb = static_cast<SkyBox*>(r->GetOwnerOfComponent());
				out << YAML::Key << "Faces";
				out << YAML::BeginMap;
				out << YAML::Flow;
				out << YAML::BeginSeq << sb->faces[0] << sb->faces[1] << sb->faces[2] << sb->faces[3] << sb->faces[4] << sb->faces[5] << YAML::EndSeq;
				out << YAML::EndMap;
			}

			if(r->floatUniforms.size() > 0) {
				out << YAML::Key << "Uniforms float";
				//out << YAML::BeginMap;
				out << YAML::Value << r->floatUniforms;
				//out << YAML::EndMap;
			}

			out << YAML::Key << "Textures";
			out << YAML::BeginMap;
			out << YAML::Key << "Diffuse" << YAML::Value << (r->diffuseTexture != nullptr ? r->diffuseTexture->GetFilePath() : "InvalidTexture");
			out << YAML::Key << "Normal" << YAML::Value << (r->normalMapTexture != nullptr ? r->normalMapTexture->GetFilePath() : "InvalidTexture");
			out << YAML::EndMap;

			out << YAML::Key << "Mesh";
			out << YAML::BeginMap;
			out << YAML::Key << "Mesh" << YAML::Value << (r->mesh != nullptr ? r->mesh->filePath : "InvalidMesh");
			out << YAML::EndMap;

			out << YAML::EndMap;
		}
	}
}

void Doom::SceneSerializer::SerializeSpriteRendererComponent(YAML::Emitter & out, ComponentManager * cm)
{
	if (cm->GetComponent<SpriteRenderer>() != nullptr) {
		SpriteRenderer* sr = cm->GetComponent<SpriteRenderer>();
		if (typeid(*sr) == typeid(SpriteRenderer)){
			out << YAML::Key << "Sprite renderer";
			out << YAML::BeginMap;
			out << YAML::Key << "Color" << YAML::Value << sr->color;
			out << YAML::Key << "Texture" << YAML::Value << (sr->texture != nullptr ? sr->texture->GetFilePath() : "InvalidTexture");
			out << YAML::Key << "Texture atlas" << YAML::Value << (sr->textureAtlas != nullptr ? sr->textureAtlas->GetTexture()->GetFilePath() : "InvalidTexture");;
			std::vector<float> vert;
			for (uint32_t i = 0; i < 16; i++)
			{
				vert.push_back(sr->mesh2D[i]);
			}
			out << YAML::Key << "Vertices" << YAML::Value << vert;
			out << YAML::EndMap;
		}
	}
}

void Doom::SceneSerializer::SerializeDirectionalLightComponent(YAML::Emitter & out, ComponentManager * cm)
{
	if (cm->GetComponent<DirectionalLight>() != nullptr) {
		DirectionalLight* dl = cm->GetComponent<DirectionalLight>();
		out << YAML::Key << "Directional light";
		out << YAML::BeginMap;
		out << YAML::Key << "Intensity" << YAML::Value << dl->intensity;
		out << YAML::Key << "Color" << YAML::Value << dl->color;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializePointLightComponent(YAML::Emitter & out, ComponentManager * cm)
{
	if (cm->GetComponent<PointLight>() != nullptr) {
		PointLight* pl = cm->GetComponent<PointLight>();
		out << YAML::Key << "Point light";
		out << YAML::BeginMap;
		out << YAML::Key << "Color" << YAML::Value << pl->color;
		out << YAML::Key << "Attenuation";
		out << YAML::BeginMap;
		out << YAML::Key << "Constant" << YAML::Value << pl->constant;
		out << YAML::Key << "Linear" << YAML::Value << pl->linear;
		out << YAML::Key << "Quadratic" << YAML::Value << pl->quadratic;
		out << YAML::EndMap;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeCubeColliderComponent(YAML::Emitter & out, ComponentManager * cm)
{
	if (cm->GetComponent<CubeCollider3D>() != nullptr) {
		CubeCollider3D* cc = cm->GetComponent<CubeCollider3D>();
		out << YAML::Key << "Cube collider";
		out << YAML::BeginMap;
		out << YAML::Key << "Bounding box" << YAML::Value << cc->isBoundingBox;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeSphereColliderComponent(YAML::Emitter & out, ComponentManager * cm)
{
	if (cm->GetComponent<SphereCollider>() != nullptr) {
		SphereCollider* cc = cm->GetComponent<SphereCollider>();
		out << YAML::Key << "Sphere collider";
		out << YAML::BeginMap;
		out << YAML::Key << "Inside of bounding box" << YAML::Value << cc->isInBoundingBox;
		out << YAML::EndMap;
	}
}

void Doom::SceneSerializer::SerializeRegisteredEvents(YAML::Emitter & out, GameObject * go)
{
	out << YAML::Key << "Registered events" << YAML::Value << go->registeredEvents;
}

