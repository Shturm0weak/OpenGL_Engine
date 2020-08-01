#ifndef MATRIX_H_
#define MATRIX_H_
#include "MathLibrary.h"
#include <mutex>

namespace math {

	class Vector;

	class MATH_API Matrix {
	private:
		std::condition_variable cv;
		std::mutex cv_m;
		uint32_t m = 1;
		uint32_t n = 1;
		double* m_matrix = nullptr;
		bool* complete = nullptr;
	public:
		Matrix(uint32_t m, uint32_t n);
		Matrix(uint32_t m, uint32_t n, double matrix[]);
		Matrix(const Matrix& matrix);
		~Matrix();

		double& operator()(uint32_t i, uint32_t j);

		void operator+=(Matrix& matrix);
		void operator-=(Matrix& matrix);
		void operator*=(double value);
		void operator=(Matrix& matrix);

		Matrix FastMul(Matrix& matrix);
		Matrix operator+(Matrix& matrix);
		Matrix operator-(Matrix& matrix);
		Matrix operator*(double value);
		Matrix operator*(Matrix& matrix);
		Vector operator*(Vector& value);
		//static Matrix& Inverse(Matrix& matrix);

		void AssignCol(Vector vec, int col);
		Vector GetCol(uint32_t col);

		friend class Vector;
		friend double Magnitude(Matrix& matrix);
		friend Matrix Transpose(Matrix& matrix);
		friend void Input(Matrix & matrix);
		friend void Print(const Matrix & matrix);
	};

}

#endif