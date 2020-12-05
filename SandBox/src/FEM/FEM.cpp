#include "pch.h"
#include "FEM.h"

double Doom::FEM::CalculateAreaOfTriangle(const Triangle& triangle)
{
    math::Matrix A(3,3);
    A.AssignCol(math::Vector(3, 1), 0);

    math::Vector col2(3);
    col2[0] = triangle.verteces[0].x;
    col2[1] = triangle.verteces[1].x;
    col2[2] = triangle.verteces[2].x;
    A.AssignCol(col2, 1);

    math::Vector col3(3);
    col3[0] = triangle.verteces[0].y;
    col3[1] = triangle.verteces[1].y;
    col3[2] = triangle.verteces[2].y;
    A.AssignCol(col3, 2);

    return math::Determinant(A,A.m_Cols) * 0.5;
}

void Doom::FEM::OnStart()
{
    double area = CalculateAreaOfTriangle(*triangle);
    double R = CalculateR(*triangle);
    double Z = CalculateZ(*triangle);
    glm::dvec3 A = CalculateAlpha(*triangle);
    glm::dvec3 B = CalculateBeta(*triangle);
    glm::dvec3 Y = CalculateGamma(*triangle);
    math::Matrix matD = CalculateMatrixD(m_E, m_U);
    math::Matrix matB = CalculateMatrixB(area, R, Z, A, B, Y);
    math::Matrix matK = (math::Transpose(matB) * (matD * matB)) * 2 * 3.14159 * area;
    std::cout << matK.m_Rows << "x" << matK.m_Cols << std::endl;
    math::Print(matK * (1.0 / 1e7));
}

double Doom::FEM::CalculateR(const Triangle& triangle)
{
    return (triangle.verteces[0].x + triangle.verteces[1].x + triangle.verteces[2].x) / 3.0;
}

double Doom::FEM::CalculateZ(const Triangle& triangle)
{
    return (triangle.verteces[0].y + triangle.verteces[1].y + triangle.verteces[2].y) / 3.0;
}

void Doom::FEM::OnUpdate()
{
}

glm::dvec3 Doom::FEM::CalculateAlpha(const Triangle& triangle)
{
    glm::dvec3 a;
    a.x = triangle.verteces[1].x * triangle.verteces[2].y - triangle.verteces[2].x * triangle.verteces[1].y;
    a.y = triangle.verteces[2].x * triangle.verteces[0].y - triangle.verteces[0].x * triangle.verteces[2].y;
    a.z = triangle.verteces[0].x * triangle.verteces[1].y - triangle.verteces[1].x * triangle.verteces[0].y;
    return a;
}

glm::dvec3 Doom::FEM::CalculateBeta(const Triangle& triangle)
{
    glm::dvec3 b;
    b.x = triangle.verteces[1].y - triangle.verteces[2].y;
    b.y = triangle.verteces[2].y - triangle.verteces[0].y;
    b.z = triangle.verteces[0].y - triangle.verteces[1].y;
    return b;
}

glm::dvec3 Doom::FEM::CalculateGamma(const Triangle& triangle)
{
    glm::dvec3 c;
    c.x = triangle.verteces[2].x - triangle.verteces[1].x;
    c.y = triangle.verteces[0].x - triangle.verteces[2].x;
    c.z = triangle.verteces[1].x - triangle.verteces[0].x;
    return c;
}

void Doom::FEM::OnClose()
{
    delete triangle;
}

math::Matrix Doom::FEM::CalculateMatrixD(double E, double u) {
    math::Matrix D(4, 4);
    math::Vector col(4);

    //1
    col[0] = 1 - u;
    col[1] = u;
    col[2] = u;
    col[3] = 0;
    D.AssignCol(col, 0);

    //2
    col[0] = u;
    col[1] = 1 - u;
    col[2] = u;
    col[3] = 0;
    D.AssignCol(col, 1);

    //3
    col[0] = u;
    col[1] = u;
    col[2] = 1 - u;
    col[3] = 0;
    D.AssignCol(col, 2);

    //4
    col[0] = 0;
    col[1] = 0;
    col[2] = 0;
    col[3] = (1 - 2 * u) * 0.5;
    D.AssignCol(col, 3);
   
    return D * (E / ((1.0 + u) * (1.0 - 2.0 * u)));
}

math::Matrix Doom::FEM::CalculateMatrixB(double Area, double R, double Z,glm::dvec3 A, glm::dvec3 B, glm::dvec3 Y)
{
    math::Matrix matB(4,6);
    math::Vector col(4);

    //1
    col[0] = B.x;
    col[1] = A.x / R + B.x + (Y.x * Z) / R;
    col[2] = 0;
    col[3] = Y.x;
    matB.AssignCol(col, 0);

    //2
    col[0] = 0;
    col[1] = 0;
    col[2] = Y.x;
    col[3] = B.x;
    matB.AssignCol(col, 1);

    //3
    col[0] = B.y;
    col[1] = A.y / R + B.y + (Y.y * Z) / R;
    col[2] = 0;
    col[3] = Y.y;
    matB.AssignCol(col, 2);

    //4
    col[0] = 0;
    col[1] = 0;
    col[2] = Y.y;
    col[3] = B.y;
    matB.AssignCol(col, 3);

    //5
    col[0] = B.z;
    col[1] = A.z / R + B.z + (Y.z * Z) / R;
    col[2] = 0;
    col[3] = Y.z;
    matB.AssignCol(col, 4);

    //6
    col[0] = 0;
    col[1] = 0;
    col[2] = Y.z;
    col[3] = B.z;
    matB.AssignCol(col, 5);

    return matB * (1 / (2 * Area));
}
