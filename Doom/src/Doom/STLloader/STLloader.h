#pragma once

namespace Doom {

	class DOOM_API StlLoader {
	public:
		static Mesh* LoadMesh(std::string name,std::string filePath);
	private:
		static Mesh* ReadFromSTLASCII(std::string name, std::string filePath);
		static Mesh* ReadFromSTLBinary(std::string name, std::string filePath);
		static bool StlFileHasASCIIFormat(std::string filePath);
	};

}