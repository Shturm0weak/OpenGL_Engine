#ifndef MATH_H_
#define MATH_H_

namespace math {

	double Magnitude(Matrix& matrix);

	bool IsCloseEnough(double a, double b);

	void Print(const Matrix& matrix);
	void Print(const Vector& vector);
	void Input(Matrix& matrix);

	Matrix Transpose(const Matrix& matrix);
	Matrix Cofactor(const Matrix& matrix, int64_t p, int64_t q);
	int64_t Determinant(const Matrix& matrix, int64_t n);
	Matrix Adjoint(const Matrix& matrix);
	Matrix Inverse(const Matrix& matrix);
	Vector Solve(const Matrix& matrix, const Vector& vector, double& progress, std::string& progressMessage);
	int64_t ForwardElimination(Matrix& matrix, double& progress);
	Vector BackSubstitutian(Matrix& matrix, double& progress);
	Vector  Normalize(const Vector& vector);
}

#endif