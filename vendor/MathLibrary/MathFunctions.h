#ifndef MATH_H_
#define MATH_H_
#include "MathLibrary.h"
#include "Matrix.h"
#include "Vector.h"

namespace math {

	double Magnitude(Matrix& matrix);

	void  Print(const Matrix& matrix);
	void  Print(const Vector& vec);
	void  Input(Matrix& matrix);

	Matrix  Transpose(Matrix& matrix);
	Vector  Normalize(Vector& vec);
}

#endif