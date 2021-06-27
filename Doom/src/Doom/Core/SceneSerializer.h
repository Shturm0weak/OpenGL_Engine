#pragma once

#include "yaml-cpp/yaml.h"

namespace Doom {

	class DOOM_API SceneSerializer {
	public:

		static std::string s_CurrentSceneFilePath;
		static void Serialize(const std::string& filePath);
		static void DeSerialize(const std::string& filePath);
	private:

		//Might be crash in deserializetion due to not checking whether iterator is valid or not
		static void DeSerializeCamera(YAML::Node& in);
		static bool DeSerializeSkyBox(YAML::detail::iterator_value& in);
		static void DeSerializeGameObject(YAML::detail::iterator_value& in, std::map<GameObject*, std::vector<int>>& childs);
		static void DeSerializeTextureAtlas(YAML::detail::iterator_value& in);
		static void DeSerializeTransformComponent(YAML::detail::iterator_value& in, ComponentManager* cm);
		static void DeSerializeRenderer3DComponent(YAML::detail::iterator_value& in, ComponentManager* cm);
		static void DeSerializeSpriteRendererComponent(YAML::detail::iterator_value& in, ComponentManager* cm);
		static void DeSerializeDirectionalLightComponent(YAML::detail::iterator_value& in, ComponentManager* cm);
		static void DeSerializePointLightComponent(YAML::detail::iterator_value& in, ComponentManager* cm);
		static void DeSerializeSpotLightComponent(YAML::detail::iterator_value& in, ComponentManager* cm);
		static void DeSerializeSphereColliderComponent(YAML::detail::iterator_value& in, ComponentManager* cm);
		static void DeSerializeRectangularCollider(YAML::detail::iterator_value& in, ComponentManager* cm);
		static void DeSerializeParticleEmitterComponent(YAML::detail::iterator_value& in, ComponentManager* cm);
		static void DeSerializeGameObjectChilds(YAML::detail::iterator_value& in, GameObject* go, std::map<GameObject*, std::vector<int>>& childs);

		static void SerializeCamera(YAML::Emitter& out);
		static void SerializeTextureAtlas(YAML::Emitter& out, TextureAtlas* ta);
		static void SerializeGameObject(YAML::Emitter& out, GameObject* go);
		static void SerializeGameObjectChilds(YAML::Emitter& out, GameObject* go);
		static void SerializeTransformComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeRenderer3DComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeSpriteRendererComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeDirectionalLightComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializePointLightComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeSpotLightComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeCubeColliderComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeSphereColliderComponent(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeRegisteredEvents(YAML::Emitter& out, GameObject* go);
		static void SerializeRectangularCollider(YAML::Emitter& out, ComponentManager* cm);
		static void SerializeParticleEmitterComponent(YAML::Emitter& out, ComponentManager* cm);
	};

}