#pragma once
#include "Application.h"
#include "Matrix.h"
#include "Vector.h"
#define EPSILON 10e-6

namespace Doom {
	
	class Weight {
	public:
		double r = 0.0;
		double z = 0.0;
		double w = 0.0;
	};

	class Triangle {
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

		math::Matrix CalculateMatrixJ();
		math::Matrix CalculateMatrixB(Weight weight, double Area, double r, double z, glm::dvec3 A, glm::dvec3 B, glm::dvec3 Y);
		math::Matrix CalculateMatrixBAprox(double Area, double R, double Z, glm::dvec3 A, glm::dvec3 B, glm::dvec3 Y);

		double CalculateAreaOfTriangle();
		double CalculateR();
		double CalculateZ();
		glm::dvec3 CalculateAlpha();
		glm::dvec3 CalculateBeta();
		glm::dvec3 CalculateGamma();
	};

	class FEM : public Application {
	public:

		//TODO: Write array of 2 triangles try to sort and store them in appropriate way!!!
		std::vector<Triangle*> triangles;
		std::vector<glm::dvec2> g_Nodes;
		std::vector<Weight> weights;
		double m_E = 2.1e5;
		double m_U = 0.25;
		size_t maxDimension = 0;
		GameObject* m_Go = nullptr;
		float m_DirLightAngle = 0;
		bool m_TurnOffGrid = false;
		bool m_ShowStiffnessMatrix = false;
		math::Vector m_GSMMulVec = math::Vector(0);
		math::Matrix m_GlobalStiffnessMatrix = math::Matrix(0, 0);

		math::Matrix CalculateMatrixD(double m_E, double m_u);

		void AddMatrices(math::Matrix& globalMatK, Triangle* triangle);

		static bool CompareNodes(dvec2 a, dvec2 b);

		void Clear();
		void IndexNodes(std::vector<Triangle*> triangles);
		void CalculateGlobalStiffnessMatrix(Mesh* mesh);

		FEM(std::string name = "FEM axisymmentric problem", int width = 800, int height = 600, bool Vsync = true) : Application(name, TYPE_3D, width, height, Vsync) {}

		virtual void OnStart();

		virtual void OnUpdate();

		virtual void OnGuiRender();

		virtual void OnClose();
	};

}

