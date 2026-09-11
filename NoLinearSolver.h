//
// NoLinearSolver.h
//
#include <concepts>
#include "ssstl/List.h"
#include "ssstl/Pair.h"

template<typename FunctionType>
concept LinearFunc = requires(FunctionType function, double x)
{
	{ function(x) } -> std::same_as<double>;
};

template<typename FunctionType>
class NoLinearSolver
{
public:
	class noLinearException : std::exception
	{
		std::wstring message;

	public:
		noLinearException(std::wstring errMessage) : message(errMessage) {}

		const char* what() const noexcept
		{
			return "Error";
		}

		const wchar_t* wwhat() const
		{
			return message.c_str();
		}
	};

private:
	FunctionType func_;
	FunctionType dFunc_;

	double epsilon_;

public:
	// function(double x) -> double
	template<typename FunctionT = FunctionType>
	requires LinearFunc<FunctionT>
	NoLinearSolver(
		FunctionT function,
		FunctionT dFunction,
		double epsilon = 1e-6
	) :
		func_(function),
		dFunc_(dFunction),
		epsilon_(epsilon)
	{}

	void SetEpsilon(double epsilon) { epsilon_ = epsilon; }

	// function(double x) -> double
	template<typename FunctionT = FunctionType>
	requires LinearFunc<FunctionT>
	void SetFunctions(FunctionT &&function, FunctionT&& dFunction)
	{
		func_ = std::forward<FunctionT>(function);
		dFunc_ = std::forward<FunctionT>(dFunction);
	}

	ssstl::List<ssstl::Pair<double, double>> FindIntervals(double limStart, double limEnd, double step)
	{
		ssstl::List<ssstl::Pair<double, double>> intervals;
		for (double x = limStart; x <= limEnd; x += step)
		{
			if (func_(x) * func_(x + step) < 0)
			{
				intervals.push_back({x, x + step});
			}
		}
		return intervals;
	}

	double SolveNewton(double initialXGuess, int iterationThreshold = 1000)
	{
		double xPrev = initialXGuess;
		double xNext{};

		for (int i = 0; i < iterationThreshold; ++i)
		{
			double derivative = dFunc_(xPrev);

			if (abs(derivative) < 1e-12)
			{
				throw noLinearException(L"����������� ����� ����. ���������� ��������� ����� �������.");
			}

			xNext = xPrev - (func_(xPrev) / derivative);

			if (abs(xNext - xPrev) < epsilon_)
			{
				return xNext;
			}

			xPrev = xNext;
		}

		throw noLinearException(L"������ �� ������. ��������� ���������� ��������.");
	}
};