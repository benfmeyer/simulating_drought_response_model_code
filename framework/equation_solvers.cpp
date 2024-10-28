#include "equation_solvers.h"
#include <valarray>
#include <cmath>



IEquationSolver::IEquationSolver(int max_steps, double eps) :max_steps(max_steps), eps(eps)
{
}

IEquationSolver::~IEquationSolver()
{
}

SecantSolver::SecantSolver(int max_steps, double eps) : IEquationSolver(max_steps, eps)
{
}

double SecantSolver::FindRoot(double a, double b)
{

	c = 0;
	xn = a;
	xn1 = b;


	do
	{
		fn = f(xn);
		fn1 = f(xn1);

		xn2 = xn1 - (xn1 - xn) / (fn1 - fn) * fn1;
		xn = xn1;
		xn1 = xn2;

		c++;
	} while (fabs(fn1 - fn) > eps || c < max_steps);


	return xn2;
}

NewtonSolver::NewtonSolver(int max_steps, double eps) : IEquationSolver(max_steps, eps)
{
}

double NewtonSolver::FindRoot(double x0)
{
	c = 0;
	xn = x0;

	// Initialize with a value to get at least one time into the loop
	xn1 = x0 + 2 * eps;

	while (fabs(xn1 - xn) > eps && c < max_steps)
	{
		xn = xn1;
		xn1 = xn - f(xn) / df(xn);
		c++;
	}

	return xn1;
}


BisectionSolver::BisectionSolver(int max_steps, double eps) : IEquationSolver(max_steps, eps)
{
}

double BisectionSolver::FindRoot(double a, double b)
{
	c = 0;
	double mid = (b - a) / 2.0;
	xn = a;
	xn1 = b;

	while (fabs(xn1 - xn) > eps && c < max_steps)
	{
		// Find middle point
		mid = (xn + xn1) / 2;

		if (f(mid) == 0.0)
			xn1 = xn;

		else if (f(mid) * f(xn) < 0.0)
			xn1 = mid;
		else
			xn = mid;

		c++;
	}

	return mid;
}


IODESolver::IODESolver() : n(0), h(0.0)
{
}

double IODESolver::ODESolve(const double x0, const double y0, const double x, const double h)
{
	n = static_cast<int>((x - x0) / h);
	this->h = h;
	return Iterate(x0, y0);
}

double IODESolver::ODESolve(const double x0, const double y0, const double x, const size_t n)
{
	this->n = n;
	h = (x - x0) / static_cast<double>(n);
	return Iterate(x0, y0);
}

double IODESolver::ODESolve(const double x0, const double y0, const double x, const int n)
{
	this->n = n;
	h = (x - x0) / static_cast<double>(n);
	return Iterate(x0, y0);
}

IODESolver::~IODESolver()
{
}

EulerExplicit::EulerExplicit()
{
}

EulerExplicit::~EulerExplicit()
{
}

double EulerExplicit::Iterate(const double x0, const double y0)
{
	double y = y0;
	double x_r = x0;

	for (int i = 1; i < n + 1; ++i)
	{
		y += h * dydx(x_r, y);
		x_r += h;
	}

	return  y;
}

RungeKutta4Explicit::RungeKutta4Explicit() :k1(0.0), k2(0.0), k3(0.0), k4(0.0), k5(0.0)
{
}

RungeKutta4Explicit::~RungeKutta4Explicit()
{
}

double RungeKutta4Explicit::Iterate(const double x0, const double y0)
{
	double y = y0;
	double x_r = x0;

	for (int i = 1; i < n + 1; ++i)
	{
		// Apply Runge Kutta Formulas to find
		// next value of y
		k1 = h * dydx(x_r, y);
		k2 = h * dydx(x_r + 0.5 * h, y + 0.5 * k1);
		k3 = h * dydx(x_r + 0.5 * h, y + 0.5 * k2);
		k4 = h * dydx(x_r + h, y + k3);

		// Update next value of y
		y += (1.0 / 6.0) * (k1 + 2 * k2 + 2 * k3 + k4);

		// Update next value of x
		x_r += h;
	}
	return y;
}

AdamsBashforth2Explicit::AdamsBashforth2Explicit() : IODESolver()
{
}

AdamsBashforth2Explicit::~AdamsBashforth2Explicit()
{
}

double AdamsBashforth2Explicit::Iterate(const double x0, const double y0)
{
	double x_0 = x0;
	double y_0 = y0;

	double x_1 = x0 + h;
	double y_1 = y_0 + h * dydx(x_0, y_0);

	double x_2 = x_1 + h;
	double y_2 = y_1 + 1.5 * h * dydx(x_1, y_1) - 0.5 * h * dydx(x_0, y_0);

	for (int i = 1; i < n; ++i)
	{
		y_2 = y_2 + 1.5 * h * dydx(x_2, y_2) - 0.5 * h * dydx(x_1, y_1);
		y_1 = y_2;
		x_1 = x_2;
		x_2 += h;
	}

	return y_2;
}

MidpointImplicit::MidpointImplicit(int solver_steps, double solver_prec) :
	IODESolver(),
	NewtonSolver(solver_steps, solver_prec)
{
}


MidpointImplicit::~MidpointImplicit()
{
}

double MidpointImplicit::Iterate(const double x0, const double y0)
{
	xn = x0;
	yn = y0;

	for (int i = 1; i < n + 1; ++i)
	{
		yn = FindRoot(yn);
		xn += h;
	}

	return yn;
}

double MidpointImplicit::f(const double y)
{
	return yn - y + h * dydx(xn + 0.5 * h, 0.5 * (yn + y));
}

double MidpointImplicit::df(double y)
{
	return -1.0 + 0.5 * h * d2ydx2(xn + 0.5 * h, 0.5 * (yn + y));
}

