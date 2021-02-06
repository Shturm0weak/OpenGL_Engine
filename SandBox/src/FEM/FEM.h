#pragma once
#include "Application.h"
#include "Matrix.h"
#include "Vector.h"

namespace Doom {

	struct Triangle {
		std::vector<glm::dvec2> verteces;

		Triangle(glm::dvec2 a, glm::dvec2 b, glm::dvec2 c)
		{
			verteces.push_back(a);
			verteces.push_back(b);
			verteces.push_back(c);
		}
	};

	class FEM : public Application {
	public:

		//TODO: Write array of 2 triangles try to sort and store them in appropriate way!!!

		double m_E = 30e6;
		double m_U = 0.25;

		math::Matrix CalculateMatrixD(double m_E, double m_u);
		math::Matrix CalculateMatrixB(double Area, double R, double Z, glm::dvec3 A, glm::dvec3 B, glm::dvec3 Y);

		double CalculateAreaOfTriangle(const Triangle& triangle);
		double CalculateR(const Triangle& triangle);
		double CalculateZ(const Triangle& triangle);
		glm::dvec3 CalculateAlpha(const Triangle& triangle);
		glm::dvec3 CalculateBeta(const Triangle& triangle);
		glm::dvec3 CalculateGamma(const Triangle& triangle);

		FEM(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}

		Triangle* triangle = new Triangle(glm::dvec2(0,2), glm::dvec2(0,0), glm::dvec2(3,2));

		virtual void OnStart();

		virtual void OnUpdate();

		virtual void OnClose();
	};

}

