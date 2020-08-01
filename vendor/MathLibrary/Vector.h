#ifndef VECTOR_H_
#define VECTOR_H_
#include "MathLibrary.h"

namespace math {

	class Matrix;

	class MATH_API Vector {
	private:
		double* m_array;
		uint32_t m_size = 0;
	public:
		Vector();
		Vector(uint32_t size);
		Vector(uint32_t size, double value);
		Vector(uint32_t size, double* value);
		Vector(const Vector& vec);
		~Vector();

		Vector& operator = (const Vector& vec);
		void operator +=(const Vector& vec);
		void operator -=(const Vector& vec);
		void operator *=(double value);

		double operator *(const Vector& vec);
		Vector operator *(Matrix& matrix);
		Vector operator *(double value);
		Vector operator +(const Vector& vec);
		Vector operator -(const Vector& vec);

		double Length() const;

		double& operator[](uint32_t index);

		bool operator>(const Vector& vec);
		bool operator<(const Vector& vec);
		bool operator>=(const Vector& vec);
		bool operator<=(const Vector& vec);
		bool operator==(const Vector& vec);
		bool operator!=(const Vector& vec);

		friend class Matrix;
		friend void Print(const Vector & vec);
		friend Vector Normalize(Vector& vec);
	};

}

#endif