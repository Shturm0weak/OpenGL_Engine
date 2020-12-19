#pragma once

#include "yaml-cpp/yaml.h"

namespace Doom {

	class DOOM_API SceneSerializer {
	public:

		static std::string s_CurrentSceneFilePath;
		static void Serialize(const std::string& filePath);
		static void DeSerialize(const std::string& filePath);
	private:

		static void DeSerializeGameObject(YAML::detail::iterator_value& go, std::map<GameObject*, std::vector<int>>& childs);
		static void DeSerializeTextureAtlas(YAML::detail::iterator_value& ta);
		static void SerializeTextureAtlas(YAML::Emitter& out, TextureAtlas* ta);
		static void SerializeGameObject(YAML::Emitter& out, GameObject* go);
		static void SerializeGameObjectChilds(YAML::Emitter& out, GameObject* go);
		static void SerializeTransformComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeRenderer3DComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeSpriteRendererComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeDirectionalLightComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializePointLightComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeCubeColliderComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeSphereColliderComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeRegisteredEvents(YAML::Emitter& out, GameObject* go);
		static void SerializeRectangularCollider(YAML::Emitter& out, ComponentManager* go);
	};

}