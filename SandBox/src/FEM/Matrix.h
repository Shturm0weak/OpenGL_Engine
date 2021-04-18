#ifndef MATRIX_H_
#define MATRIX_H_

namespace math {

	class Vector;

	class Matrix {
	public:

		double* m_Matrix = nullptr;
		size_t m_Rows = 0;
		size_t m_Cols = 0;
		Matrix(size_t rows, size_t cols, double initValue = 0.0);
		Matrix(size_t rows, size_t cols, double matrix[]);
		Matrix(const Matrix& matrix);
		void Copy(const Matrix& matrix);
		~Matrix();

		double& operator()(size_t i, size_t j);

		void operator+=(const Matrix& matrix);
		void operator-=(const Matrix& matrix);
		void operator*=(double value);
		void operator=(const Matrix& matrix);
		void operator=(Matrix&& matrix) noexcept;

		Matrix operator+(const Matrix& matrix);
		Matrix operator-(const Matrix& matrix);
		Matrix operator*(double value);
		Matrix operator*(const Matrix& matrix);
		Vector operator*(const Vector& value);

		void AssignCol(const Vector& vector, size_t col);
		void AssignRow(const Vector& vector, size_t row);
		void AppendCol(const Vector& vector);
		void AppendRow(const Vector& vector);
		void RemoveCol(size_t col);
		void RemoveRow(size_t row);
		Vector GetCol(size_t col);
		Vector GetRow(size_t row);

		void Clear();

		friend class Vector;
		friend double Magnitude(const Matrix& matrix);
		friend Matrix Transpose(const Matrix& matrix);
		friend void Input(Matrix & matrix);
		friend void Print(const Matrix & matrix);
		friend int64_t Determinant(const Matrix& matrix, int64_t n);
		friend Matrix Cofactor(const Matrix& matrix, int64_t p, int64_t q);
		friend Matrix Adjoint(const Matrix& matrix);
		friend Matrix Inverse(const Matrix& matrix);
		friend Vector Solve(const Matrix& matrix, const Vector& vector, double& progress, std::string& progressMessage);
		friend int64_t ForwardElimination(const Matrix& matrix, double& progress);
		friend Vector BackSubstitutian(Matrix& matrix, std::string& progressMessage);
	};

}

#endif