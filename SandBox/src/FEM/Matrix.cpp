#include <iostream>
#include "Matrix.h"
#include "Vector.h"
#include "MathFunctions.h"

using namespace math;

Matrix::Matrix(size_t rows, size_t cols, double initValue) : m_Cols(cols),m_Rows(rows)
{
//#ifdef _DEBUG
//	std::cout << "Matrix is created\n";
//#endif 

	size_t size = rows * cols;
	m_Matrix = new double[size];
	for (size_t i = 0; i < size; i++)
	{
		m_Matrix[i] = initValue;
	}
}

Matrix::Matrix(size_t rows, size_t cols, double matrix[]) : m_Cols(cols),m_Rows(rows)
{
//#ifdef _DEBUG
//	std::cout << "Matrix is created\n";
//#endif 

	size_t size = rows * cols;
	m_Matrix = new double[size];
	memcpy(m_Matrix, matrix, size * 8);
}

Matrix::Matrix(const Matrix& matrix) : m_Cols(matrix.m_Cols),m_Rows(matrix.m_Rows)
{
//#ifdef _DEBUG
//	std::cout << "Matrix is copied\n";
//#endif 

	if (&matrix == this) return;
	size_t size = m_Rows * m_Cols;
	delete[] m_Matrix;
	m_Matrix = new double[m_Rows * m_Cols];
	memcpy(m_Matrix, matrix.m_Matrix,size * 8);
}

Matrix::~Matrix()
{
//#ifdef _DEBUG
//	std::cout << "Matrix is destroyed " << m_matrix << std::endl;
//#endif

	Clear();
}

double& Matrix::operator()(size_t i, size_t j)
{
	return m_Matrix[i * m_Cols + j];
}

void Matrix::operator+=(const Matrix& matrix)
{
	if (m_Rows != matrix.m_Rows && m_Cols != matrix.m_Cols) throw std::string("Matrices have to have equal amount of rows and columns!");
	size_t size = m_Rows * m_Cols;
	for (size_t i = 0; i < size; i++)
	{
		m_Matrix[i] += matrix.m_Matrix[i];
	}
}

void Matrix::operator-=(const Matrix& matrix)
{
	if (m_Rows != matrix.m_Rows && m_Cols != matrix.m_Cols) throw std::string("Matrices have to have equal amount of rows and columns!");
	size_t size = m_Rows * m_Cols;
	for (size_t i = 0; i < size; i++)
	{
		m_Matrix[i] -= matrix.m_Matrix[i];
	}
}

void Matrix::operator*=(double value)
{
	size_t size = m_Rows * m_Cols;
	for (size_t i = 0; i < size; i++)
	{
		m_Matrix[i] *= value;
	}
}

void Matrix::operator=(const Matrix& matrix)
{
//#ifdef _DEBUG
//	std::cout << "Matrix is copied\n";
//#endif

	if (&matrix == this) return;
	m_Rows = matrix.m_Rows;
	m_Cols = matrix.m_Cols;
	delete[] m_Matrix;
	size_t size = m_Rows * m_Cols;
	m_Matrix = new double[size];
	memcpy(m_Matrix, matrix.m_Matrix, size * 8);
}

void Matrix::operator=(Matrix&& matrix) noexcept
{
	if (&matrix == this) return;
		m_Rows = matrix.m_Rows;
		m_Cols = matrix.m_Cols;
		delete[] m_Matrix;
		m_Matrix = matrix.m_Matrix;
		matrix.m_Matrix = nullptr;
		matrix.m_Rows = 0;
		matrix.m_Cols = 0;
}

void Matrix::Copy(const Matrix& matrix)
{
//#ifdef _DEBUG
//	std::cout << "Matrix is copied\n";
//#endif 

	if (&matrix == this) return;
	m_Rows = matrix.m_Rows;
	m_Cols = matrix.m_Cols;
	delete[] m_Matrix;
	m_Matrix = new double[m_Rows * m_Cols];
	size_t size = m_Rows * m_Cols;
	for (size_t i = 0; i < size; i++)
	{
		m_Matrix[i] = matrix.m_Matrix[i];
	}
}

Matrix Matrix::operator+(const Matrix& matrix)
{
	Matrix c(m_Rows, m_Cols);
	size_t size = m_Rows * m_Cols;
	for (size_t i = 0; i < size; i++)
	{
		c.m_Matrix[i] = m_Matrix[i] + matrix.m_Matrix[i];
	}
	return c;
}

Matrix Matrix::operator-(const Matrix& matrix)
{
	Matrix c(m_Rows, m_Cols);
	size_t size = m_Rows * m_Cols;
	for (size_t i = 0; i < size; i++)
	{
		c.m_Matrix[i] = m_Matrix[i] - matrix.m_Matrix[i];
	}
	return c;
}

Matrix Matrix::operator*(double value)
{
	Matrix c(m_Rows, m_Cols);
	size_t size = m_Rows * m_Cols;
	for (size_t i = 0; i < size; i++)
	{
		c.m_Matrix[i] = m_Matrix[i] * value;
	}
	return c;
}

Vector Matrix::operator*(const Vector& value)
{
	if (m_Cols != value.m_Size) throw std::string("Matrix and Vector have to have equal amount of rows and columns!");
	Vector result(m_Rows);
	for (size_t i = 0; i < m_Rows; i++)
	{
		for (size_t j = 0; j < m_Cols; j++)
		{
			result.m_Array[i] += operator()(i, j) * value.m_Array[j];
		}
	}
	return result;
}

Matrix Matrix::operator*(const Matrix& matrix)
{
	if (m_Cols != matrix.m_Rows) throw std::string("Matrix and Vector have to have equal amount of rows and columns!");
	Matrix c(m_Rows, matrix.m_Cols);
	int K = m_Cols > matrix.m_Rows ? m_Cols : matrix.m_Rows;
	for (int i = 0; i < m_Rows; ++i)
	{
		for (int j = 0; j < c.m_Cols; ++j)
		{
			c.m_Matrix[i * c.m_Cols + j] = 0;
			for (int k = 0; k < K; ++k)
				c.m_Matrix[i * c.m_Cols + j] += m_Matrix[i * K + k] * matrix.m_Matrix[k * c.m_Cols + j];
		}
	}
	return c;
}

void Matrix::AssignCol(const Vector& vector, size_t col)
{
	if(col > m_Cols - 1) throw std::string("Column index is out of range!");
	if (vector.m_Size != m_Rows) throw std::string("Matrix and Vector have to have equal amount of rows and columns!");
	for (size_t i = 0; i < m_Rows; i++)
	{
		operator()(i, col) = vector.m_Array[i];
	}
}

void Matrix::AssignRow(const Vector& vector, size_t row)
{
	if (row > m_Rows - 1) throw std::string("Row index is out of range!");
	if (vector.m_Size != m_Cols) throw std::string("Matrix and Vector have to have equal amount of rows and columns!");
	for (size_t i = 0; i < m_Cols; i++)
	{
		operator()(row, i) = vector.m_Array[i];
	}
}

void Matrix::AppendCol(const Vector& vector)
{
	if (vector.m_Size != m_Rows) throw std::string("Matrix and Vector have to have equal amount of rows and columns!");
	Matrix tempMat(0, 0);
	tempMat = std::move(*this);
	m_Rows = tempMat.m_Rows;
	m_Cols = tempMat.m_Cols;
	m_Matrix = new double[m_Rows * (m_Cols + 1)];
	m_Cols++;
	for (size_t i = 0; i < m_Rows; i++)
	{
		for (size_t j = 0; j < m_Cols - 1; j++)
		{
			operator()(i, j) = tempMat.operator()(i, j);
		}
	}
	for (size_t k = 0; k < m_Rows; k++)
	{
		operator()(k, m_Cols - 1) = vector.m_Array[k];
	}
}

void math::Matrix::AppendRow(const Vector& vector)
{
	if (vector.m_Size != m_Cols) throw std::string("Matrix and Vector have to have equal amount of rows and columns!");
	Matrix tempMat(0, 0);
	tempMat = std::move(*this);
	m_Rows = tempMat.m_Rows;
	m_Cols = tempMat.m_Cols;
	m_Matrix = new double[(m_Rows + 1) * m_Cols];
	m_Rows++;
	for (size_t i = 0; i < m_Rows - 1; i++)
	{
		for (size_t j = 0; j < m_Cols; j++)
		{
			operator()(i, j) = tempMat.operator()(i, j);
		}
	}
	for (size_t k = 0; k < m_Cols; k++)
	{
		operator()(m_Rows - 1, k) = vector.m_Array[k];
	}
}

void Matrix::RemoveCol(size_t col)
{
	if (col > m_Cols - 1) throw std::string("Column index is out of range!");
	Matrix tempMat(0, 0);
	tempMat = std::move(*this);
	m_Rows = tempMat.m_Rows;
	m_Cols = tempMat.m_Cols;
	m_Matrix = new double[m_Rows * (m_Cols - 1)];
	m_Cols--;
	for (size_t i = 0; i < m_Rows; i++)
	{
		for (size_t j = 0; j < col; j++)
		{
			operator()(i, j) = tempMat.operator()(i, j);
		}
	}
	for (size_t i = 0; i < m_Rows; i++)
	{
		for (size_t j = col; j < m_Cols; j++)
		{
			operator()(i, j) = tempMat.operator()(i, j + 1);
		}
	}
}

void Matrix::RemoveRow(size_t row)
{
	if (row > m_Rows - 1)  throw std::string("Row index is out of range!");
	Matrix tempMat(0, 0);
	tempMat = std::move(*this);
	m_Rows = tempMat.m_Rows;
	m_Cols = tempMat.m_Cols;
	m_Matrix = new double[(m_Rows -1) * m_Cols];
	m_Rows--;
	for (size_t i = 0; i < row; i++)
	{
		for (size_t j = 0; j < m_Cols; j++)
		{
			operator()(i, j) = tempMat.operator()(i, j);
		}
	}
	for (size_t i = row; i < m_Rows; i++)
	{
		for (size_t j = 0; j < m_Cols; j++)
		{
			operator()(i, j) = tempMat.operator()(i + 1, j);
		}
	}

}

Vector Matrix::GetCol(size_t col)
{
	if (col > m_Cols - 1) throw std::string("Column index is out of range!");
	Vector result(m_Rows);
	for (size_t i = 0; i < m_Rows; i++)
	{
		result[i] = operator()(i, col);
	}
	return result;
}

Vector Matrix::GetRow(size_t row)
{
	if (row > m_Rows - 1) throw std::string("Row index is out of range!");
	Vector result(m_Rows);
	for (size_t i = 0; i < m_Cols; i++)
	{
		result[i] = operator()(row, i);
	}
	return result;
}

void Matrix::Clear()
{
	m_Rows = 0;
	m_Cols = 0;
	delete[] m_Matrix;
	m_Matrix = nullptr;
}