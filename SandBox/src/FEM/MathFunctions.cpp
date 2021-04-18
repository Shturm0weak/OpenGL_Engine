#include <iostream>
#include "Vector.h"
#include "Matrix.h"
#include "MathFunctions.h"

using namespace math;

Vector math::Normalize(Vector& vector)
{
	return (Vector(vector * (1 / vector.Length())));
}

double math::Magnitude(Matrix& matrix)
{
	double max = 0;
	for (size_t i = 0; i < matrix.m_Rows; i++) {
		double sum = 0.0;
		for (size_t j = 0; j < matrix.m_Cols; j++) {
			sum += abs(matrix.operator()(i, j));
		}
		if (sum > max)
			max = sum;
	}
	return max;
}

bool math::IsCloseEnough(double a, double b)
{
	return (fabs(a - b) < 10e-6);
}

Matrix math::Transpose(const Matrix& matrix)
{
	Matrix mat(matrix.m_Cols, matrix.m_Rows);
	for (size_t i = 0; i < matrix.m_Cols; i++)
	{
		for (size_t j = 0; j < matrix.m_Rows; j++)
		{
			mat.m_Matrix[matrix.m_Rows * i + j] = matrix.m_Matrix[matrix.m_Cols * j + i];
		}
	}
	return mat;
}

Matrix math::Cofactor(const Matrix& matrix, int64_t p, int64_t q)
{
	int64_t i = 0, j = 0;
	Matrix c(matrix.m_Rows, matrix.m_Cols);
	for (int64_t row = 0; row < c.m_Cols; row++)
	{
		for (int64_t col = 0; col < c.m_Cols; col++)
		{
			if (row != p && col != q)
			{
				c.m_Matrix[i * matrix.m_Cols + j++] = matrix.m_Matrix[row * matrix.m_Cols + col];
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

int64_t math::Determinant(const Matrix& matrix, int64_t n)
{
	int64_t D = 0;

	if (n == 1)
		return matrix.m_Matrix[0];

	int64_t sign = 1;

	for (int64_t f = 0; f < n; f++)
	{
		Matrix temp = Cofactor(matrix, 0, f);
		D += sign * matrix.m_Matrix[0 * n + f] * Determinant(temp, n - 1);

		sign = -sign;
	}

	return D;
}

Matrix math::Adjoint(const Matrix& matrix)
{
	Matrix c(matrix.m_Rows, matrix.m_Rows);
	int64_t N = matrix.m_Cols;
	if (N == 1)
	{
		c.m_Matrix[0] = 1;
		return Matrix(0,0);
	}
	int64_t sign = 1;

	for (int64_t i = 0; i < N; i++)
	{
		for (int64_t j = 0; j < N; j++)
		{
			sign = ((i + j) % 2 == 0) ? 1 : -1;

			Matrix cof = Cofactor(matrix, i, j);
			c.m_Matrix[j * N + i] = (sign) * (Determinant(cof, N - 1));
		}
	}
	return c;
}

Matrix math::Inverse(const Matrix& matrix)
{
	int64_t det = Determinant(matrix, matrix.m_Cols);
	if (det == 0)
	{
		std::cout << "Singular matrix, can't find its inverse\n";
		return Matrix(0,0);
	}
	Matrix inverse(matrix.m_Rows, matrix.m_Cols);
	int64_t N = inverse.m_Cols;
	Matrix adjoint = Adjoint(matrix);

	for (int64_t i = 0; i < N; i++)
		for (int64_t j = 0; j < N; j++)
			inverse.m_Matrix[i * N + j] = adjoint.m_Matrix[i * N + j] / float(det);

	return inverse;
}

Vector math::Solve(const Matrix& matrix, const Vector& vector, double& progress, std::string& progressMessage)
{
	Matrix mat = matrix;
	mat.AppendCol(vector);

	progressMessage = "Forward elimination";
	int isSingular = ForwardElimination(mat, progress);

	if (isSingular != -1)
	{
		if (mat(isSingular, mat.m_Rows))
			throw std::string("Singular matrix");
		else
			throw std::string("May have infinitely many solutions.");
	}

	progressMessage = "Back substitutian";
	return BackSubstitutian(mat, progress);
}

int64_t math::ForwardElimination(Matrix& matrix, double& progress)
{
	progress = 0.0;
	for (size_t k = 0; k < matrix.m_Rows; k++)
	{
		double i_max = k;
		double v_max = matrix(i_max, k);
		for (size_t i = k + 1; i < matrix.m_Rows; i++)
		{
			if (abs(matrix(i, k)) > v_max)
			{
				v_max = matrix(i, k), i_max = (double)i;
			}
		}

		if (!matrix(k, i_max))
		{
			progress = 1.0;
			return k;
		}

		if (i_max != k)
		{
			for (size_t l = 0; l <= matrix.m_Rows; l++)
			{
				double temp = matrix(k, l);
				matrix(k, l) = matrix(i_max, l);
				matrix(i_max, l) = temp;
			}
		}

		for (size_t i = k + 1; i < matrix.m_Rows; i++)
		{
			double f = matrix(i, k) / matrix(k, k);
			for (size_t j = k + 1; j <= matrix.m_Rows; j++)
			{
				matrix(i, j) -= matrix(k, j) * f;
			}
			matrix(i, k) = 0.0;
		}
		progress = (double)k / (double)matrix.m_Rows;
	}
	return -1;
}

Vector math::BackSubstitutian(Matrix& matrix, double& progress)
{
	progress = 0.0;
	Vector vec(matrix.m_Rows);
	for (int64_t i = matrix.m_Rows - 1; i >= 0; i--)
	{
		vec[i] = matrix(i, matrix.m_Rows);
		for (size_t j = i + 1; j < matrix.m_Rows; j++)
		{
			vec[i] -= matrix(i, j) * vec[j];
		}
		vec[i] /= matrix(i, i);
		progress = 1.0 - ((double)i / (double)matrix.m_Rows);
	}
	return vec;
}

void math::Input(Matrix& matrix)
{
	std::cout << "Input {" << matrix.m_Rows << "," << matrix.m_Cols << "}\n";
	for (size_t i = 0; i < matrix.m_Rows; i++)
	{
		for (size_t j = 0; j < matrix.m_Cols; j++)
		{
			std::cin >> matrix.operator()(i, j);
		}
	}
}

void math::Print(const Matrix& matrix)
{
	for (size_t i = 0; i < matrix.m_Rows; i++) {
		for (size_t j = 0; j < matrix.m_Cols; j++) {
			printf_s("%.3lf	", (matrix.m_Matrix[i * matrix.m_Cols + j]));
		}
		printf_s("\n");
	}
}

void math::Print(const Vector& vector)
{
	for (size_t i = 0; i < vector.m_Size; i++)
	{
		printf_s("%.32lf\n", vector.m_Array[i]);
	}
}
