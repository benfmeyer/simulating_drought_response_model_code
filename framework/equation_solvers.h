///////////////////////////////////////////////////////////////////////////////////////
/// \file EquationSolvers.h
/// \brief Auxiliary root finding equation solver class
///
/// \author Phillip Papstefanou
/// $Date: 2020-04-23 15:04:09 +0200 (Wed, 23 Apr 2020) $
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
// for size_t on unix
#include "stddef.h"

class IEquationSolver
{
public:
	IEquationSolver(int max_steps, double eps);
	virtual ~IEquationSolver();

protected:
	virtual double f(const double x) = 0;

	/// Number of iterations
	int max_steps;

	/// Desired precision of root
	double eps;

	/// Running conter
	int c;
};


class SecantSolver : public IEquationSolver
{
public:
	SecantSolver(int max_steps, double eps);

	double FindRoot(double xa, double xb);

private:
	double xn;
	double xn1;
	double xn2;

	double fn;
	double fn1;
};


class NewtonSolver : public IEquationSolver
{
public:
	NewtonSolver(int max_steps, double eps);

	double FindRoot(double x0);

protected:
	virtual double df(double x) = 0;

private:
	double xn;
	double xn1;

	double fn;
	double dfn;
};


class BisectionSolver : public IEquationSolver
{
public:
	BisectionSolver(int max_steps, double eps);

	double FindRoot(double a, double b);

private:
	double xn;
	double xn1;

	double fn;
	double dfn;
};


/**
* \brief Ordinary Differential Equation Solver Interface
* This Interface methods is chosen to avoid member function pointer passing
* which can have much slower performance and is very ugly...
* That's the cause why the differntial dydx has to implemented by a derived function
* IODESolver as an interface cannot be implemented directly: One of it's derived members
* RungeKutta4, ExplicitEuler, ... can only be chosen
*/
class IODESolver
{
public:
	IODESolver();
	/**
	* \brief
	* \param x0 x-coordinate of initial point
	* \param y0 y-coordinate of initial point
	* \param x  x-coordinate of point of interest
	* \param h stepsize
	* \return
	*/
	double ODESolve(const double x0, const double y0, const double x, double h);

	/**
	* \brief
	* \param x0 x-coordinate of initial point
	* \param y0 y-coordinate of initial point
	* \param x  x-coordinate of point of interest
	* \param n number of steps
	* \return
	*/
	double ODESolve(const double x0, const double y0, const double x, const size_t n);

	/**
	* \brief
	* \param x0 x-coordinate of initial point
	* \param y0 y-coordinate of initial point
	* \param x  x-coordinate of point of interest
	* \param n number of steps
	* \return
	*/
	double ODESolve(const double x0, const double y0, const double x, const int n);
	virtual ~IODESolver();

protected:
	/// Iteration function; Needs to be implemtend be the solver itself
	virtual double Iterate(const double x0, const double y0) = 0;

	/// Differential of the ODE, needs to be implemented by the derived class of the
	/// actual solver
	virtual double dydx(const double x, const double y) = 0;

	/// Number of steps
	size_t n;

	/// x-size of step
	double h;
};

class EulerExplicit : public IODESolver
{
public:
	EulerExplicit();
	~EulerExplicit();
private:
	double Iterate(const double x0, const double y0) override;
};

class RungeKutta4Explicit : public IODESolver
{
public:
	RungeKutta4Explicit();
	~RungeKutta4Explicit();

private:
	double Iterate(const double x0, const double y0) override;

	double k1;
	double k2;
	double k3;
	double k4;
	double k5;
};

class AdamsBashforth2Explicit : public IODESolver
{
public:
	AdamsBashforth2Explicit();
	~AdamsBashforth2Explicit();
private:
	double Iterate(const double x0, const double y0) override;
};

class MidpointImplicit : public IODESolver, public NewtonSolver
{
public:
	MidpointImplicit(int solver_steps, double solver_prec);
	~MidpointImplicit();

protected:
	double Iterate(const double x0, const double y0) override;

	virtual double d2ydx2(const double x, const double y) = 0;

	double f(const double x) override;
	double df(double x) override;

private:
	double xn;
	double yn;

};

