//
// IntegralSolver.h
//
#include <concepts>
#include <cmath>
#include "List.h"
#include "Pair.h"

template<typename FunctionType>
concept MathFunc = requires(FunctionType function, double x)
{
	{ function(x) } -> std::same_as<double>;
};

template<typename FunctionType>
class IntegralSolver
{
public:
	class integralException : std::exception
	{
		std::wstring message;

	public:
		integralException(std::wstring errMessage) : message(errMessage) {}

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
	double epsilon_;

public:
	// function(double x) -> double
	template<typename FunctionT = FunctionType>
	requires MathFunc<FunctionT>
	IntegralSolver(
		FunctionT function,
		double epsilon = 1e-6
	) :
		func_(function),
		epsilon_(epsilon)
	{}

	void SetEpsilon(double epsilon) { epsilon_ = epsilon; }

	// function(double x) -> double
	template<typename FunctionT = FunctionType>
	requires MathFunc<FunctionT>
	void SetFunction(FunctionT &&function)
	{
		func_ = std::forward<FunctionT>(function);
	}

	//double SolveGauss3(double limStart, double limEnd, int partitionCount)
	//{
	//	if (partitionCount <= 0)
	//	{
	//		throw integralException(L"Количество разбиений должно быть больше нуля.");
	//	}

	//	double result = 0.0;
	//	double step = (limEnd - limStart) / partitionCount;

	//	for (int i = 0; i < partitionCount; ++i)
	//	{
	//		double xStart = limStart + i * step;
	//		double xCenter = xStart + step / 2.0;

	//		double shift = (step / 2.0) * std::sqrt(3.0 / 5.0);

	//		double x1 = xCenter - shift;
	//		double x2 = xCenter;
	//		double x3 = xCenter + shift;

	//		result += (step / 18.0) * (5.0 * func_(x1) + 8.0 * func_(x2) + 5.0 * func_(x3));
	//	}

	//	return result;
	//}
	//double SolveGauss3(double limStart, double limEnd, int partitionCount)
	//{
	//	if (partitionCount <= 0)
	//	{
	//		throw integralException(L"Количество разбиений должно быть больше нуля.");
	//	}
	//	double result = 0.0;
	//	double step = (limEnd - limStart) / partitionCount;

	//	double halfStep = step / 2;
	//	double coefficient = halfStep * 0.7745966692;
	//	double K1 = 5. / 9, K2 = 8. / 9;

	//	for (double x = limStart + halfStep; x < limEnd; x += step)
	//	{
	//		result += K1 * func_(x - coefficient) + K2 * func_(x) + K1 * func_(x + coefficient);
	//	}

	//	return halfStep * result;
	//}
	double SolveGauss3(double limStart, double limEnd, int partitionCount)
	{
		if (partitionCount <= 0)
		{
			throw integralException(L"Количество разбиений должно быть больше нуля.");
		}

		double result = 0.0;
		const double step = (limEnd - limStart) / partitionCount;
		const double halfStep = step / 2;
		const double shift = halfStep * std::sqrt(3.0 / 5.0);
		const double outerCoeff = 5.0 / 18.0 * step;
		const double innerCoeff = 8.0 / 18.0 * step;

		for (int i = 0; i < partitionCount; ++i)
		{
			double center = limStart + i * step + halfStep;
			result += outerCoeff * func_(center - shift) + innerCoeff * func_(center) + outerCoeff * func_(center + shift);
		}

		return result;
	}

	Pair<double, int> SolveGauss3Auto(double limStart, double limEnd, int initialPartitions = 2, int iterationThreshold = 25)
	{
		int m = initialPartitions;
		double s1 = SolveGauss3(limStart, limEnd, m);
		double s2{};

		for (int i = 0; i < iterationThreshold; ++i)
		{
			m *= 2;
			s2 = SolveGauss3(limStart, limEnd, m);

			if (std::abs(s1 - s2) < epsilon_)
			{
				return {s2, m};
			}

			s1 = s2;
		}

		throw integralException(L"Интеграл не сошелся.\r\nПревышено количество итераций.");
	}
};