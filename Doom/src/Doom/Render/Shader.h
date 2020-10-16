#ifndef SHADER_H
#define SHADER_H

#include <map>

namespace Doom {

	struct DOOM_API ShaderProgramSource {
		std::string VertexSource;
		std::string FragmentSource;
	};

	class DOOM_API Shader {
	private:
		static std::map<std::string,Shader*> shaders;
		static const char** listOfShaders;
		std::string m_FilePath;
		unsigned int m_RendererID;
		std::string m_Name;
		int GetUniformLocation(const std::string& name);
		std::unordered_map<std::string, int> m_UniformLocationCache;
		Shader(std::string name,std::string filepath);
		~Shader();

	public:
		static Shader* Create(std::string name,std::string path);
		static Shader* Get(std::string name);

		void Bind() const;
		void UnBind() const;

		ShaderProgramSource Parseshader(const std::string& filepath);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& FragmentShader);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		void SetUniform1i(const std::string name, int value);
		void SetUniform1iv(const std::string name, int* value);

		void SetUniform4f(const std::string name, float v0, float v1, float v2, float fv3);
		void SetUniform3f(const std::string name, float v0, float v1, float v2);
		void SetUniform2f(const std::string name, float v0, float v1);

		void SetUniform4fv(const std::string name,const glm::vec4& vec4);
		void SetUniform3fv(const std::string name,const glm::vec3& vec3);
		void SetUniform2fv(const std::string name,const glm::vec2& vec);

		void SetUniform1f(const std::string name, float v0);

		void SetUniformMat4f(const std::string name,const glm::mat4& matrix);
	
		void Reload();

		static const char** GetListOfShaders();

		friend class Editor;
	};

}

#endif