#include <iostream>
#include "Vector.h"
#include "Matrix.h"
#include "MathFunctions.h"

using namespace math;

Vector::Vector()
{
#ifdef _DEBUG
	std::cout << "Vector is created\n";
#endif 
	m_array = nullptr;
	m_size = 0;
}

Vector::Vector(uint32_t size) : m_size(size)
{
#ifdef _DEBUG
	std::cout << "Vector is created\n";
#endif 
	m_array = new double[m_size];
	for (uint32_t i = 0; i < m_size; i++)
	{
		m_array[i] = 0;
	}
}

Vector::Vector(uint32_t size, double value) : m_size(size)
{
#ifdef _DEBUG
	std::cout << "Vector is created\n";
#endif 
	m_array = new double[m_size];
	for (uint32_t i = 0; i < m_size; i++)
	{
		m_array[i] = value;
	}
}

Vector::Vector(uint32_t size, double * value) : m_size(size)
{
#ifdef _DEBUG
	std::cout << "Vector is created\n";
#endif 
	m_array = new double[m_size];
	for (uint32_t i = 0; i < m_size; i++)
	{
		m_array[i] = value[i];
	}
}

Vector::Vector(const Vector& vec) : m_size(vec.m_size)
{
#ifdef _DEBUG
	std::cout << "Vector is copied\n";
#endif 
	m_array = new double[m_size];
	for (uint32_t i = 0; i < m_size; i++)
	{
		m_array[i] = vec.m_array[i];
	}
}

Vector::~Vector()
{
#ifdef _DEBUG
	std::cout << "Vector is destroyed " << m_array << std::endl;
#endif 
	delete[] m_array;
	m_array = nullptr;
}

Vector& Vector::operator=(const Vector & vec)
{
#ifdef _DEBUG
	std::cout << "Vector is copied\n";
#endif 
	delete[] m_array;
	m_size = vec.m_size;
	m_array = new double[m_size];
	for (uint32_t i = 0; i < m_size; i++)
	{
		m_array[i] = vec.m_array[i];
	}

	return *this;
}

void Vector::operator+=(const Vector & vec)
{
	if (m_size != vec.m_size)
		return;
	for (uint32_t i = 0; i < m_size; i++)
	{
		m_array[i] += vec.m_array[i];
	}
}

void Vector::operator-=(const Vector & vec)
{
	if (m_size != vec.m_size)
		return;
	for (uint32_t i = 0; i < m_size; i++)
	{
		m_array[i] -= vec.m_array[i];
	}
}

void Vector::operator*=(double value)
{
	for (uint32_t i = 0; i < m_size; i++)
	{
		m_array[i] *= value;
	}
}

double Vector::operator*(const Vector & vec)
{
	if (m_size != vec.m_size)
		return NULL;
	double dotProduct = 0;
	for (uint32_t i = 0; i < m_size; i++)
	{
		dotProduct += m_array[i] * vec.m_array[i];
	}
	return dotProduct;
}

Vector Vector::operator*(Matrix & matrix)
{
	if (matrix.m_Rows == m_size) {
		Vector c(matrix.m_Rows);
		for (uint32_t i = 0; i < matrix.m_Rows; i++)
		{
			for (uint32_t j = 0; j < matrix.m_Cols; j++)
			{
				c.m_array[i] += matrix.operator()(i, j) * m_array[j];
			}
		}
		return c;
	}
}

Vector Vector::operator*(double value)
{
	Vector v(m_size);
	for (uint32_t i = 0; i < m_size; i++)
	{
		v.m_array[i] = m_array[i] * value;
	}
	return v;
}

Vector Vector::operator+(const Vector & vec)
{
	Vector v(m_size);
	for (uint32_t i = 0; i < m_size; i++)
	{
		v.m_array[i] = m_array[i] + vec.m_array[i];
	}
	return v;
}

Vector Vector::operator-(const Vector & vec)
{
	Vector v(m_size);
	for (uint32_t i = 0; i < m_size; i++)
	{
		v.m_array[i] = m_array[i] - vec.m_array[i];
	}
	return v;
}

double Vector::Length() const
{
	double length = 0;
	for (uint32_t i = 0; i < m_size; i++)
	{
		length += m_array[i] * m_array[i];
	}
	return sqrtl(length);
}

double& Vector::operator[](uint32_t index)
{
	return m_array[index];
}

bool Vector::operator>(const Vector & vec)
{
	return (Length() > vec.Length());
}

bool Vector::operator<(const Vector & vec)
{
	return (Length() < vec.Length());
}

bool Vector::operator>=(const Vector & vec)
{
	return (Length() >= vec.Length());
}

bool Vector::operator<=(const Vector & vec)
{
	return (Length() <= vec.Length());
}

bool Vector::operator==(const Vector & vec)
{
	return (Length() == vec.Length());
}

bool Vector::operator!=(const Vector & vec)
{
	return (Length() != vec.Length());
}