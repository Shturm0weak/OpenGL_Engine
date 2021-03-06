#include "Integrals.h"
#include <math.h>


namespace integrals {

	double RectangleMethod(double(*F)(double), double high, double low, int n) {
		double S = 0;
		double step = (high - low) / n;
		double deltaxval = low;
		for (int i = 0; i < n; i++)
		{
			S += step * (F(deltaxval));
			deltaxval += step;
		}
		return fabs(S);
	}

	double TrapezoidMethod(double(*F)(double), double high, double low, int n) {
		double S = 0;
		double step = (high - low) / n;
		double deltaxval = low;
		double a, b;
		for (int i = 0; i < n; i++)
		{
			a = F(deltaxval);
			b = F(deltaxval + step);
			S += ((a + b) / 2.) * step;
			deltaxval += step;
		}
		return fabs(S);
	}

	double ParabolaMethod(double(*F)(double), double high, double low, int n)
	{
		double S = 0;
		double step = (high - low) / n;
		double deltaxval = low;
		double a, b, c;
		for (int i = 0; i < n; i++)
		{
			a = deltaxval;
			b = deltaxval + step;
			c = (a + b) / 2.;
			S += (b - a) / 6. * (F(a) + 4. * F(c) + F(b));
			deltaxval += step;
		}
		return fabs(S);
	}

	double doubleIntegral(std::function<double(double, double)> F, float step_x, float step_y, float lower_x, float upper_x, float lower_y, float upper_y)
	{
		int n1, n2;
		float arr[50][50], arr_2[50], result;
		n1 = (upper_x - lower_x) / step_x + 1;
		n2 = (upper_y - lower_y) / step_y + 1;
		for (int i = 0; i < n1; ++i) {
			for (int j = 0; j < n2; ++j) {
				arr[i][j] = F(lower_x + i * step_x, lower_y + j * step_y);
			}
		}
		for (int i = 0; i < n1; ++i) {
			arr_2[i] = 0;
			for (int j = 0; j < n2; ++j) {
				if (j == 0 || j == n2 - 1)
					arr_2[i] += arr[i][j];
				else if (j % 2 == 0)
					arr_2[i] += 2 * arr[i][j];
				else
					arr_2[i] += 4 * arr[i][j];
			}
			arr_2[i] *= (step_y / 3);
		}
		result = 0;
		for (int i = 0; i < n1; ++i) {
			if (i == 0 || i == n1 - 1)
				result += arr_2[i];
			else if (i % 2 == 0)
				result += 2 * arr_2[i];
			else
				result += 4 * arr_2[i];
		}
		result *= (step_x / 3);
		return result;
	}

}