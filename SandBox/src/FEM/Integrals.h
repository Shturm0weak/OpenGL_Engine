#pragma once

#include <functional>

namespace math {

	namespace integrals {

		double RectangleMethod(double(*F)(double), double high, double low, int n);

		double TrapezoidMethod(double(*F)(double), double high, double low, int n);

		double ParabolaMethod(double(*F)(double), double high, double low, int n);

		double doubleIntegral(std::function<double(double, double)> F, float step_x, float step_y, float lower_x, float upper_x, float lower_y, float upper_y);
	}

}