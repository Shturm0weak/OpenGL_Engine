#include <iostream>
#include "Vector.h"
#include "Matrix.h"
#include "MathFunctions.h"

using namespace math;

Vector math::Normalize(Vector& vec)
{
	return (Vector(vec * (1 / vec.Length())));
}

double math::Magnitude(Matrix& matrix)
{
	double max = 0;
	for (uint32_t i = 0; i < matrix.m_Rows; i++) {
		double s = 0.0;
		for (uint32_t j = 0; j < matrix.m_Cols; j++) {
			s += abs(matrix.operator()(i, j));
		}
		if (s > max)
			max = s;
	}
	return max;
}

Matrix math::Transpose(const Matrix& matrix)
{
	Matrix mat(matrix.m_Cols, matrix.m_Rows);
	for (uint32_t i = 0; i < matrix.m_Cols; i++)
	{
		for (uint32_t j = 0; j < matrix.m_Rows; j++)
		{
			mat.m_matrix[matrix.m_Rows * i + j] = matrix.m_matrix[matrix.m_Cols * j + i];
		}
	}
	return mat;
}

Matrix math::Cofactor(const Matrix & matrix, int p, int q)
{
	int i = 0, j = 0;
	Matrix c(matrix.m_Rows, matrix.m_Cols);
	for (int row = 0; row < c.m_Cols; row++)
	{
		for (int col = 0; col < c.m_Cols; col++)
		{
			if (row != p && col != q)
			{
				c.m_matrix[i * matrix.m_Cols + j++] = matrix.m_matrix[row * matrix.m_Cols + col];
				if (j == matrix.m_Cols - 1)
				{
					j = 0;
					i++;
				}
			}
		}
	}
	return c;
}

int math::Determinant(const Matrix & matrix,int n)
{
	int D = 0;

	if (n == 1)
		return matrix.m_matrix[0];

	int sign = 1;

	for (int f = 0; f < n; f++)
	{
		Matrix temp = Cofactor(matrix, 0, f);
		D += sign * matrix.m_matrix[0 * n + f] * Determinant(temp,n - 1);

		sign = -sign;
	}

	return D;
}

Matrix math::Adjoint(const Matrix & matrix)
{
	Matrix c(matrix.m_Rows, matrix.m_Rows);
	int N = matrix.m_Cols;
	if (N == 1)
	{
		c.m_matrix[0] = 1;
		return Matrix(0,0);
	}
	int sign = 1;

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			sign = ((i + j) % 2 == 0) ? 1 : -1;

			Matrix cof = Cofactor(matrix,i,j);
			c.m_matrix[j * N + i] = (sign)*(Determinant(cof, N - 1));
		}
	}
	return c;
}

Matrix math::Inverse(const Matrix & matrix)
{
	Matrix inverse(matrix.m_Rows, matrix.m_Cols);
	int N = inverse.m_Cols;
	int det = Determinant(matrix, matrix.m_Cols);
	if (det == 0)
	{
		std::cout << "Singular matrix, can't find its inverse\n";
		return Matrix(0,0);
	}
	Matrix adjoint = Adjoint(matrix);

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			inverse.m_matrix[i * N + j] = adjoint.m_matrix[i * N + j] / float(det);

	return inverse;
}

void math::Input(Matrix & matrix)
{
	std::cout << "Input {" << matrix.m_Rows << "," << matrix.m_Cols << "}\n";
	for (uint32_t i = 0; i < matrix.m_Rows; i++)
	{
		for (uint32_t j = 0; j < matrix.m_Cols; j++)
		{
			std::cin >> matrix.operator()(i, j);
		}
	}
}

void math::Print(const Matrix & matrix)
{
	for (uint32_t i = 0; i < matrix.m_Rows; i++) {
		for (uint32_t j = 0; j < matrix.m_Cols; j++) {
			printf_s("%.16lf	", (matrix.m_matrix[i * matrix.m_Cols + j]));
		}
		printf_s("\n");
	}
}

void math::Print(const Vector & vec)
{
	for (uint32_t i = 0; i < vec.m_size; i++)
	{
		printf_s("%.16lf\n", vec.m_array[i]);
	}
}
