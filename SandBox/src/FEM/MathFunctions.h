#ifndef MATH_H_
#define MATH_H_

namespace math {

	double Magnitude(Matrix& matrix);

	void Print(const Matrix& matrix);
	void Print(const Vector& vec);
	void Input(Matrix& matrix);

	Matrix Transpose(const Matrix& matrix);
	Matrix Cofactor(const Matrix& matrix, int p, int q);
	int Determinant(const Matrix& matrix,int n);
	Matrix Adjoint(const Matrix& matrix);
	Matrix Inverse(const Matrix& matrix);
	Vector  Normalize(const Vector& vec);
}

#endif