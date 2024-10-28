#pragma once
#include <stdexcept>
#include <string>

namespace out_guess
{
	enum class PatchDetailLevel
	{
		Each_Patch,
		Mean_over_all,
		Invalid
	};

	enum class TimeReductionType
	{
		Mean,
		Total,
		Last,
		Invalid,
	};

	enum class PftDetailLevel
	{
		Total,
		Pfts_Total,
		Invalid
	};

	enum class TimeDomain
	{
		Daily,
		Monthly,
		Yearly,
		Daily_Monthly,
		Monthly_Yearly,
		Daily_Monthly_Yearly,
		Invalid
	};

	class OutGuessException : public std::runtime_error
	{
	public:
		OutGuessException(const std::string what) : runtime_error(what) {}
	};
}


