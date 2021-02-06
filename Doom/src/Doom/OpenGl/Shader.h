#pragma once

#include <map>

namespace Doom {

	struct DOOM_API ShaderProgramSource {
		std::string m_VertexSource;
		std::string m_FragmentSource;
	};

	class DOOM_API Shader {
	private:

		std::unordered_map<std::string, int> m_UniformLocationCache;
		static std::map<std::string,Shader*> s_Shaders;
		std::string m_Name;
		std::string m_FilePath;
		static const char** s_NamesOfShaders;
		unsigned int m_RendererID;

		int GetUniformLocation(const std::string& name);
		
		Shader(const std::string& name,const std::string& filepath);
		~Shader();
	public:

		static const char** GetListOfShaders();
		static Shader* Create(const std::string& name, const std::string& path);
		static Shader* Get(const std::string& name, bool showErrors = true);
		std::string& GetName() { return m_Name; }
		std::string& GetFilePath() { return m_FilePath; }
		void Bind() const;
		void UnBind() const;
		void Reload();

		ShaderProgramSource Parseshader(const std::string& filepath);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& FragmentShader);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1iv(const std::string& name, int* value);

		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float fv3);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform2f(const std::string& name, float v0, float v1);

		void SetUniform4fv(const std::string& name,const glm::vec4& vec4);
		void SetUniform3fv(const std::string& name,const glm::vec3& vec3);
		void SetUniform2fv(const std::string& name,const glm::vec2& vec);

		void SetUniform1f(const std::string& name, float v0);

		void SetUniformMat4f(const std::string& name,const glm::mat4& matrix);

		friend class Editor;
	};

}