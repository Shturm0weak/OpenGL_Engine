#include <iostream>
#include "Vector.h"
#include "Matrix.h"
#include "MathFunctions.h"

using namespace math;

Vector::Vector(size_t size, double value) : m_Size(size)
{
//#ifdef _DEBUG
//	std::cout << "Vector is created\n";
//#endif 

	m_Array = new double[m_Size];
	for (size_t i = 0; i < m_Size; i++)
	{
		m_Array[i] = value;
	}
}

Vector::Vector(size_t size, double* value) : m_Size(size)
{
//#ifdef _DEBUG
//	std::cout << "Vector is created\n";
//#endif 

	m_Array = new double[m_Size];
	memcpy(m_Array, value, m_Size * 8);
}

Vector::Vector(const Vector& vector) : m_Size(vector.m_Size)
{
//#ifdef _DEBUG
//	std::cout << "Vector is copied\n";
//#endif 

	m_Array = new double[m_Size];
	memcpy(m_Array, vector.m_Array, m_Size * 8);
}

Vector::~Vector()
{
//#ifdef _DEBUG
//	std::cout << "Vector is destroyed " << m_array << std::endl;
//#endif 

	Clear();
}

void Vector::operator=(const Vector& vector)
{
//#ifdef _DEBUG
//	std::cout << "Vector is copied\n";
//#endif 

	delete[] m_Array;
	m_Size = vector.m_Size;
	m_Array = new double[m_Size];
	for (size_t i = 0; i < m_Size; i++)
	{
		memcpy(m_Array, vector.m_Array, m_Size * 8);
	}
}

void math::Vector::operator=(Vector&& vector) noexcept
{
	if (&vector == this) return;
	m_Size = vector.m_Size;
	delete[] m_Array;
	m_Array = vector.m_Array;
	vector.m_Array = nullptr;
	vector.m_Size = 0;
}

void Vector::operator+=(const Vector & vector)
{
	if (m_Size != vector.m_Size) throw std::string("Vectors have to have equal sizes!");
	for (size_t i = 0; i < m_Size; i++)
	{
		m_Array[i] += vector.m_Array[i];
	}
}

void Vector::operator-=(const Vector & vector)
{
	if (m_Size != vector.m_Size) throw std::string("Vectors have to have equal sizes!");
	for (size_t i = 0; i < m_Size; i++)
	{
		m_Array[i] -= vector.m_Array[i];
	}
}

void Vector::operator*=(double value)
{
	for (size_t i = 0; i < m_Size; i++)
	{
		m_Array[i] *= value;
	}
}

double Vector::operator*(const Vector& vector)
{
	if (m_Size != vector.m_Size) throw std::string("Vectors have to have equal sizes!");
	double scalar = 0.0;
	for (size_t i = 0; i < m_Size; i++)
	{
		scalar += m_Array[i] * vector.m_Array[i];
	}
	return scalar;
}

Vector Vector::operator*(const Matrix& matrix)
{
	if (matrix.m_Cols != m_Size) throw std::string("Matrix and Vector have to have equal amount of rows and columns!");
	Vector result (matrix.m_Rows);
	for (size_t i = 0; i < matrix.m_Rows; i++)
	{
		for (size_t j = 0; j < matrix.m_Cols; j++)
		{
			result.m_Array[i] += matrix.m_Matrix[i * matrix.m_Cols + j] * m_Array[j];
		}
	}
	return result;
}

Vector Vector::operator*(double value)
{
	Vector result(m_Size);
	for (size_t i = 0; i < m_Size; i++)
	{
		result.m_Array[i] = m_Array[i] * value;
	}
	return result;
}

Vector Vector::operator+(const Vector& vector)
{
	if (m_Size != vector.m_Size) throw std::string("Vectors have to have equal sizes!");
	Vector result(m_Size);
	for (size_t i = 0; i < m_Size; i++)
	{
		result.m_Array[i] = m_Array[i] + vector.m_Array[i];
	}
	return result;
}

Vector Vector::operator-(const Vector& vector)
{
	if (m_Size != vector.m_Size) throw std::string("Vectors have to have equal sizes!");
	Vector vec(m_Size);
	for (size_t i = 0; i < m_Size; i++)
	{
		vec.m_Array[i] = m_Array[i] - vec.m_Array[i];
	}
	return vec;
}

double Vector::Length() const
{
	double length = 0.0;
	for (size_t i = 0; i < m_Size; i++)
	{
		length += (m_Array[i]);
	}
	return length;
}

double& Vector::operator[](size_t index)
{
	return m_Array[index];
}

bool Vector::operator<(const Vector& vector)
{
	return (Length() < vector.Length());
}

void math::Vector::Clear()
{
	m_Size = 0;
	delete[] m_Array;
	m_Array = nullptr;
}

void math::Vector::Erase(size_t index)
{
	Vector temp;
	temp = std::move(*this);
	m_Size = temp.m_Size;
	m_Array = new double[m_Size - 1];
	for (size_t i = 0; i < index; i++)
	{
		memcpy(m_Array, temp.m_Array, index * 8);
	}
	for (size_t i = index + 1; i < m_Size; i++)
	{
		m_Array[i - 1] = temp[i];
	}
	m_Size--;
}

void math::Vector::Join(Vector& vector, size_t index)
{
	Vector temp;
	temp = std::move(*this);
	m_Size = temp.m_Size;
	m_Array = new double[m_Size + vector.m_Size];
	for (size_t i = index; i < index + vector.m_Size; i++)
	{
		m_Array[i] = vector[i - index];
	}
	for (size_t i = 0; i < index; i++)
	{
		memcpy(m_Array, temp.m_Array, index * 8);
	}
	for (size_t i = index + vector.m_Size; i < m_Size + vector.m_Size; i++)
	{
		m_Array[i] = temp[i - vector.m_Size];
	}
	m_Size += vector.m_Size;
}