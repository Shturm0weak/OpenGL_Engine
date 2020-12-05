#include <iostream>
#include "Matrix.h"
#include "Vector.h"
#include "MathFunctions.h"

using namespace math;

Matrix::Matrix(uint32_t rows, uint32_t cols) : m_Cols(cols),m_Rows(rows)
{
#ifdef _DEBUG
	std::cout << "Matrix is created\n";
#endif 

	uint32_t size = rows * cols;
	m_matrix = new double[size];
	for (uint32_t i = 0; i < size; i++)
	{
		m_matrix[i] = 0;
	}
}

Matrix::Matrix(uint32_t rows, uint32_t cols, double matrix[]) : m_Cols(cols),m_Rows(rows)
{
#ifdef _DEBUG
	std::cout << "Matrix is created\n";
#endif 

	uint32_t size = rows * cols;
	m_matrix = new double[size];
	for (uint32_t i = 0; i < size; i++)
	{
		m_matrix[i] = matrix[i];
	}
}

Matrix::Matrix(const Matrix & matrix) : m_Cols(matrix.m_Cols),m_Rows(matrix.m_Rows)
{
#ifdef _DEBUG
	std::cout << "Matrix is copied\n";
#endif 

	m_matrix = new double[m_Rows * m_Cols];

	uint32_t size = m_Rows * m_Cols;
	for (uint32_t i = 0; i < size; i++)
	{
		m_matrix[i] = matrix.m_matrix[i];
	}
}

Matrix::~Matrix()
{
#ifdef _DEBUG
	std::cout << "Matrix is destroyed " << m_matrix << std::endl;
#endif
	delete[] m_matrix;
	m_matrix = nullptr;
}

double& Matrix::operator()(uint32_t i, uint32_t j)
{
	return m_matrix[i * m_Cols + j];
}

void Matrix::operator+=(const Matrix & matrix)
{
	if (m_Rows == matrix.m_Rows && m_Cols == matrix.m_Cols) {
		uint32_t size = m_Rows * m_Cols;
		for (uint32_t i = 0; i < size; i++)
		{
			m_matrix[i] += matrix.m_matrix[i];
		}
	}
}

void Matrix::operator-=(const Matrix & matrix)
{
	if (m_Rows == matrix.m_Rows && m_Cols == matrix.m_Cols) {
		uint32_t size = m_Rows * m_Cols;
		for (uint32_t i = 0; i < size; i++)
		{
			m_matrix[i] -= matrix.m_matrix[i];
		}
	}
}

void Matrix::operator*=(double value)
{
	uint32_t size = m_Rows * m_Cols;
	for (uint32_t i = 0; i < size; i++)
	{
		m_matrix[i] *= value;
	}
}

void Matrix::operator=(const Matrix& matrix)
{
#ifdef _DEBUG
	std::cout << "Matrix is copied\n";
#endif 
	m_Rows = matrix.m_Rows;
	m_Cols = matrix.m_Cols;
	m_matrix = new double[m_Rows * m_Cols];

	uint32_t size = m_Rows * m_Cols;
	for (uint32_t i = 0; i < size; i++)
	{
		m_matrix[i] = matrix.m_matrix[i];
	}
}

void math::Matrix::Copy(Matrix * matrix)
{
#ifdef _DEBUG
	std::cout << "Matrix is copied\n";
#endif 
	m_Rows = matrix->m_Rows;
	m_Cols = matrix->m_Cols;
	m_matrix = new double[m_Rows * m_Cols];

	uint32_t size = m_Rows * m_Cols;
	for (uint32_t i = 0; i < size; i++)
	{
		m_matrix[i] = matrix->m_matrix[i];
	}
}

Matrix math::Matrix::FastMul(const Matrix & matrix)
{
	if (m_Rows == matrix.m_Cols) {
		Matrix C(m_Rows, matrix.m_Cols);
		for (uint32_t i = 0; i < m_Rows; ++i)
		{
			double* c = C.m_matrix + i * m_Cols;
			for (uint32_t j = 0; j < m_Cols; ++j)
				c[j] = 0;
			for (uint32_t k = 0; k < m_Rows; ++k)
			{
				const double * b = matrix.m_matrix + k * m_Cols;
				double a = m_matrix[i*m_Rows + k];
				for (uint32_t j = 0; j < m_Cols; ++j)
					c[j] += a * b[j];
			}
		}
		return C;
	}
	return Matrix(1, 1);
}

Matrix Matrix::operator+(const Matrix & matrix)
{
	Matrix c(m_Rows, m_Cols);
	uint32_t size = m_Rows * m_Cols;
	for (uint32_t i = 0; i < size; i++)
	{
		c.m_matrix[i] = m_matrix[i] + matrix.m_matrix[i];
	}
	return c;
}

Matrix Matrix::operator-(const Matrix & matrix)
{
	Matrix c(m_Rows, m_Cols);
	uint32_t size = m_Rows * m_Cols;
	for (uint32_t i = 0; i < size; i++)
	{
		c.m_matrix[i] = m_matrix[i] - matrix.m_matrix[i];
	}
	return c;
}

Matrix Matrix::operator*(double value)
{
	Matrix c(m_Rows, m_Cols);
	uint32_t size = m_Rows * m_Cols;
	for (uint32_t i = 0; i < size; i++)
	{
		c.m_matrix[i] = m_matrix[i] * value;
	}
	return c;
}

Vector Matrix::operator*(const Vector& value)
{
	if (m_Rows == value.m_size) {
		Vector c(m_Rows);
		for (uint32_t i = 0; i < m_Rows; i++)
		{
			for (uint32_t j = 0; j < m_Cols; j++)
			{
				c.m_array[i] += operator()(i, j) * value.m_array[j];
			}
		}
		return c;
	}
}

//@Deprecated!
//#include "ThreadPool.h"
//Matrix Matrix::operator*(Matrix& matrix)
//{
//	std::unique_lock<std::mutex> lk(cv_m);	
//	Timer timer;
//	if (m == matrix.n) {
//		std::shared_ptr<Matrix> c(new Matrix(m, matrix.n));
//		std::shared_ptr<Matrix> b(new Matrix(matrix));
//		std::shared_ptr<Matrix> a(new Matrix(*this));
//		uint32_t th = ThreadPool::Instance()->num_Threads;
//		uint32_t counter = 0;
//		uint32_t offsetM = m / th;
//		uint32_t newM = 0;
//		if (offsetM == 0) { offsetM = 1; th = m; }
//		complete = new bool[th];
//		for (uint32_t i = 0; i < th; i++)
//		{
//			ThreadPool::Instance()->enqueue([=] {
//				for (uint32_t i = newM; i < newM + offsetM; i++) {
//					for (uint32_t j = 0; j < n; j++) {
//						c->m_matrix[i * m + j] = 0;
//						for (uint32_t k = 0; k < m; k++) {
//							c->m_matrix[i * m + j] += a->m_matrix[i * m + k] * b->m_matrix[k * m + j];
//						}
//					}
//				}
//				complete[i] = true;
//				cv.notify_all();
//			});
//			newM += offsetM;
//		}
//		cv.wait(lk, [=] {
//			bool check = false;
//			for (uint32_t i = 0; i < th; i++)
//			{
//				check = complete[i];
//			}
//			return check;
//		});
//		for (uint32_t i = 0; i < th; i++)
//		{
//			complete[i] = false;
//		}
//		return *c;
//	}
//}

Matrix Matrix::operator*(const Matrix& matrix)
{
	Matrix C(m_Rows, matrix.m_Cols);
	int K = m_Cols > matrix.m_Rows ? m_Cols : matrix.m_Rows;
	for (int i = 0; i < m_Rows; ++i)
	{
		for (int j = 0; j < C.m_Cols; ++j)
		{
			C.m_matrix[i*C.m_Cols + j] = 0;
			for (int k = 0; k < K; ++k)
				C.m_matrix[i*C.m_Cols + j] += m_matrix[i*K + k] * matrix.m_matrix[k*C.m_Cols + j];
		}
	}
	return C;
}

void Matrix::AssignCol(const Vector& vec, int col)
{
	if (vec.m_size != m_Rows) {
		return;
	}
	for (uint32_t i = 0; i < m_Rows; i++)
	{
		operator()(i, col) = vec.m_array[i];
	}
}

Vector Matrix::GetCol(uint32_t col)
{
	if (col < m_Cols) {
		Vector temp(m_Rows);
		for (uint32_t i = 0; i < m_Rows; i++)
		{
			temp[i] = operator()(i, col);
		}
		return temp;
	}
}
