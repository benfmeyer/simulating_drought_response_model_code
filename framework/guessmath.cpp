#include "guessmath.h"
#include "guess.h"

double powf(double a, double b) {
	// calculate approximation with fraction of the exponent
	int e = (int)b;
	union {
		double d;
		int x[2];
	} u = { a };
	u.x[1] = (int)((b - e) * (u.x[1] - 1072632447) + 1072632447);
	u.x[0] = 0;

	// exponentiation by squaring with the exponent's integer part
	// double r = u.d makes everything much slower, not sure why
	double r = 1.0;
	while (e) {
		if (e & 1) {
			r *= a;
		}
		a *= a;
		e >>= 1;
	}

	return r * u.d;
}

HillEquation::HillEquation(const double slope, const double p50, bool use_powQ) :
	slope(slope),
	p50(p50),
	use_powQ(use_powQ)
{
}

HillEquation::~HillEquation()
{
}

double HillEquation::Get(const double px)
{
	return 1.0 / (std::pow(px / p50, slope) + 1.0);
}

double HillEquation::GetD(const double px)
{
	double tau = std::pow(px / p50, slope);
	return -slope * tau / (px * (1 + tau) * (1 + tau));
}

double HillEquation::GetStd(const double px)
{
	return 1.0 / (std::pow(px / p50, slope) + 1.0);
}


double WeibullCDF::Get(double x)
{
	return 1.0 - exp(-std::pow(x / shape, scale));
}

double incbeta(double a, double b, double x) {
	if (x < 0.0 || x > 1.0) return 0.0;

	const double stop = 1.0e-8;
	const double tiny = 1.0e-30;

	/*The continued fraction converges nicely for x < (a+1)/(a+b+2)*/
	if (x > (a + 1.0) / (a + b + 2.0)) {
		return (1.0 - incbeta(b, a, 1.0 - x)); /*Use the fact that beta is symmetrical.*/
	}

	/*Find the first part before the continued fraction.*/
	const double lbeta_ab = lgamma(a) + lgamma(b) - lgamma(a + b);
	const double front = exp(log(x) * a + log(1.0 - x) * b - lbeta_ab) / a;

	/*Use Lentz's algorithm to evaluate the continued fraction.*/
	double f = 1.0, c = 1.0, d = 0.0;

	int i, m;
	for (i = 0; i <= 200; ++i) {
		m = i / 2;

		double numerator;
		if (i == 0) {
			numerator = 1.0; /*First numerator is 1.0.*/
		}
		else if (i % 2 == 0) {
			numerator = (m * (b - m) * x) / ((a + 2.0 * m - 1.0) * (a + 2.0 * m)); /*Even term.*/
		}
		else {
			numerator = -((a + m) * (a + b + m) * x) / ((a + 2.0 * m) * (a + 2.0 * m + 1)); /*Odd term.*/
		}

		/*Do an iteration of Lentz's algorithm.*/
		d = 1.0 + numerator * d;
		if (fabs(d) < tiny) d = tiny;
		d = 1.0 / d;

		c = 1.0 + numerator / c;
		if (fabs(c) < tiny) c = tiny;

		const double cd = c * d;
		f *= cd;

		/*Check for stop.*/
		if (fabs(1.0 - cd) < stop) {
			return front * (f - 1.0);
		}
	}

	return 0.0; /*Needed more loops, did not converge.*/
}


MCWD_Calculator::MCWD_Calculator(Climate& climate) : climate_(climate), cwd_daily_3(0.0), mcwd_daily_3(0.0),
cwd_monthly_100(0.0), mcwd_monthly_100(0.0), prec_month(0.0)
{
}

MCWD_Calculator::~MCWD_Calculator()
{
}

void MCWD_Calculator::UpdateDaily()
{
	CWD_Daily();

	CWD_Monthly();

	// Update climate's cwd values
	climate_.cwd_monthly_100 = cwd_monthly_100;
	climate_.cwd_daily_3 = cwd_daily_3;

	// This part is very cruicial
	// The actual climate MCWD variables should only be written at the last of the drought period
	// Problematic is here, that our period goes from Oct to Sep, however all files in LPJ-GUESS are ususally 
	// written at the end of the year and set to zero at the beginning of this year
	// To avoid this kind of behavior we only set the actually output variables (as member of class climate
	// only at the end of the year.
	// MCWD-procedure has been tested and produces identical results to the routine we used for calculating
	// for our MCWD paper
	if (date.month == 8 && date.islastday)
	{
		climate_.mcwd_monthly_100 = mcwd_monthly_100;
		climate_.mcwd_daily_3 = mcwd_daily_3;

		cwd_daily_3 = 0.0;
		cwd_monthly_100 = 0.0;
		mcwd_daily_3 = 0.0;
		mcwd_monthly_100 = 0.0;
	}

}

void MCWD_Calculator::CWD_Daily()
{
	// Daily CWD calculation
	const double d_ET = 100.0 / 30.0;
	const double daily_water_deficit = cwd_daily_3 + climate_.prec - d_ET;

	if (daily_water_deficit < 0)
		cwd_daily_3 = daily_water_deficit;
	else
		cwd_daily_3 = 0.0;

	if (mcwd_daily_3 > cwd_daily_3)
		mcwd_daily_3 = cwd_daily_3;
}

void MCWD_Calculator::CWD_Monthly()
{
	// Monthly CWD calculation
	// Set monthly precipitation to zero at the beginning of each month
	if (date.dayofmonth == 0)
		prec_month = 0.0;

	prec_month += climate_.prec;

	if (date.islastday)
	{
		const double m_ET = 100.0;
		const double monthly_water_deficit = cwd_monthly_100 + prec_month - m_ET;

		if (monthly_water_deficit < 0)
			cwd_monthly_100 = monthly_water_deficit;
		else
			cwd_monthly_100 = 0.0;
	}

	if (mcwd_monthly_100 > cwd_monthly_100)
		mcwd_monthly_100 = cwd_monthly_100;
}

