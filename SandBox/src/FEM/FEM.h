#pragma once

#include "Application.h"
#include "Matrix.h"
#include "Vector.h"

#define EPSILON 10e-6

namespace Doom {
	
	class Weight {
	public:
		double m_L1 = 0.0;
		double m_L2 = 0.0;
		double m_W = 0.0;
	};

	struct Node {
	public:

		dvec2 m_Pos;
		dvec2 m_Forces;
		size_t m_Index;
		glm::vec4 m_Stress;
		glm::bvec2 m_Constraints;

		GameObject* m_ForceArrowX = nullptr;
		GameObject* m_ForceArrowY = nullptr;

		GameObject* m_ConstraintShpereX = nullptr;
		GameObject* m_ConstraintShpereY = nullptr;
	};

	class Triangle {
	public:

		std::vector<Node*> m_Nodes;
		float* m_XPtr = nullptr;
		math::Matrix m_MatK = math::Matrix(0, 0);
		math::Matrix m_MatB = math::Matrix(0, 0);
		math::Vector m_Stress = math::Vector(4, 0.0);

		glm::dvec3 m_A;
		glm::dvec3 m_B;
		glm::dvec3 m_Y;

		double m_Area = 0.0;

		Triangle(Node* a, Node* b, Node* c);

		math::Matrix CalculateMatrixB(Weight weight, double r); //in L coordinates
		math::Matrix CalculateMatrixBAproximation(double R, double Z); //at centroid of the triangle
		math::Matrix CalculateMatrixK(math::Matrix matD, std::vector<Weight>& weights); //gauss-legendre quadrature

		double CalculateAreaOfTriangle();
		double CalculateRatCentroid();
		double CalculateZatCentroid();

		glm::dvec3 CalculateAlpha();
		glm::dvec3 CalculateBeta();
		glm::dvec3 CalculateGamma();
	};

	class FEM : public Application {
	public:

		std::vector<Triangle*> m_Triangles;
		std::vector<Node*> g_Nodes;
		std::vector<Weight> m_Weights;
		std::vector<size_t> m_StaticNodes;
		std::vector<std::wstring> m_StressesNames;
		std::vector<std::wstring> m_SideNames;

		glm::dvec2 m_MaxDisplacement;
		double m_E = 3.0 * 10e7;
		int64_t m_InputE = m_E / 10e6;
		double m_U = 0.3;
		int64_t m_InputU = m_U * 10;
		bool m_ShowRelativeToStress = false;
		int64_t m_RelativeStress = 10e7;

		Node* m_CurrentNode = nullptr;
		GameObject* m_NodeSphere = nullptr;
		GameObject* m_Go = nullptr;
		Renderer3D* m_R3d = nullptr;

		float* m_DefaultMesh = nullptr;
		float* m_DeformedMesh = nullptr;
		float m_DirLightAngle = 0;

		std::string m_ProgressMessage;
		double m_Progress = 0.0;

		bool m_IsSolvingSOLE = false;
		bool m_IsIndexed = false;
		bool m_TurnOffGrid = false;
		bool m_ShowStiffnessMatrix = false;
		bool m_ShowDisplacements = false;
		bool m_IsCalculatedGSM = false;
		bool m_ShowForces = true;
		bool m_ShowConstraints = true;
		glm::vec4 m_MinAndMaxSidePos = { -1e7, 1e7, -1e7, 1e7};
		glm::bvec4 m_ConstraintsSidesX = glm::bvec4(false);
		glm::bvec4 m_ConstraintsSidesY = glm::bvec4(false);
		glm::bvec4 m_ShowStress = glm::bvec4(false);
		glm::i64vec4 m_ForcesValuesX = glm::i64vec4(0);
		glm::i64vec4 m_ForcesValuesY = glm::i64vec4(0);
		math::Vector m_MaxStress = math::Vector(4);
		math::Matrix m_GlobalStiffnessMatrix = math::Matrix(0, 0);

		math::Matrix CalculateMatrixD(double m_E, double m_u);

		void AddMatrices(math::Matrix& globalMatK, Triangle* triangle);

		static bool CompareNodes(glm::vec2 a, glm::vec2 b);

		void ChoseNode();

		GameObject* CreateForceArrow(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);
		GameObject* CreateConstraintsSphere(glm::vec3 position, glm::vec3 scale);
		void ShowForces();
		void ShowConstraints();
		void SetForcesOnSides();
		void SetConstraintsOnSides();
		void ClearConstraints();
		void ClearForces();

		glm::vec3 HslToRGB(double h, double s, double l);
		glm::vec3 DoubleToColor(double value);
		void Clear();
		void ChangeColor(int stressType, bool isWhite);
		void IndexNodes(Mesh* mesh);
		void CalculateGlobalStiffnessMatrix(Mesh* mesh);

		FEM(std::string name = "FEM axisymmentric problem", int width = 800, int height = 600, bool Vsync = true) : Application(name, width, height, Vsync) {}

		virtual void OnStart();

		virtual void OnUpdate();

		virtual void OnGuiRender();

		virtual void OnClose();
	};

}

