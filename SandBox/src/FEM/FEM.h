#pragma once
#include "Application.h"
#include "Matrix.h"
#include "Vector.h"
#define EPSILON 10e-6

namespace Doom {

	struct Triangle {
	public:

		std::vector<glm::dvec2> m_Nodes;
		std::vector<size_t> m_NodesIndeces;
		math::Matrix m_MatK = math::Matrix(0,0);

		Triangle(glm::dvec2 a, glm::dvec2 b, glm::dvec2 c)
		{
			m_Nodes.push_back(a);
			m_Nodes.push_back(b);
			m_Nodes.push_back(c);
		}
	};

	struct Weight {
	public:
		double r;
		double z;
		double w;
	};

	class FEM : public Application {
	public:

		//TODO: Write array of 2 triangles try to sort and store them in appropriate way!!!
		std::vector<Triangle*> triangles;
		std::vector<glm::dvec2> g_Nodes;
		std::vector<Weight> weights;
		double m_E = 30e6;
		double m_U = 0.25;
		size_t maxDimension = 0;
		GameObject* m_Go = nullptr;
		float m_DirLightAngle = 0;
		bool m_TurnOffGrid = false;
		bool m_ShowStiffnessMatrix = false;
		math::Vector m_GSMMulVec = math::Vector(0);
		math::Matrix m_GlobalStiffnessMatrix = math::Matrix(0, 0);

		math::Matrix CalculateMatrixJ(Triangle& triangle);
		math::Matrix CalculateMatrixD(double m_E, double m_u);
		math::Matrix CalculateMatrixB(double Area, double R, double Z, glm::dvec3 A, glm::dvec3 B, glm::dvec3 Y);
		void AddMatrices(math::Matrix& globalMatK, Triangle* triangle);

		static bool CompareNodes(dvec2 a, dvec2 b);

		void Clear();
		void IndexNodes(std::vector<Triangle*> triangles);
		void CalculateGlobalStiffnessMatrix(Mesh* mesh);

		double CalculateAreaOfTriangle(const Triangle& triangle);
		double CalculateR(const Triangle& triangle);
		double CalculateZ(const Triangle& triangle);
		glm::dvec3 CalculateAlpha(const Triangle& triangle);
		glm::dvec3 CalculateBeta(const Triangle& triangle);
		glm::dvec3 CalculateGamma(const Triangle& triangle);

		FEM(std::string name = "FEM axisymmentric problem", int width = 800, int height = 600, bool Vsync = true) : Application(name, TYPE_3D, width, height, Vsync) {}

		virtual void OnStart();

		virtual void OnUpdate();

		virtual void OnGuiRender();

		virtual void OnClose();
	};

}

