//
// m_random.h
//
#include <type_traits>
#include <windows.h>
#pragma once

template<typename T>
class m_random
{
	static constexpr bool isUnsigned = std::is_unsigned<T>::value; // For cutting branches in compile time
	static constexpr bool isIntegral = std::is_integral<T>::value;

	bool hasLimits;

	ULONGLONG seed;
	T tick;
	T minusChance;

	T maxV;
	T minV;

	void checkLimits()
	{
		if (minV > maxV)
		{
			T temp = minV;
			minV = maxV;
			maxV = temp;
		}
	}

	// XORshift64
	ULONGLONG randNum()
	{
		// While shifting bytes in direction opposite to shift direction becomes zeros
		// then XORing original value with this value
		seed ^= seed << 13;
		seed ^= seed >> 7;
		seed ^= seed << 17;
		return seed;
	}

	void compileRandNum()
	{
		ULONGLONG raw = randNum();

		if (hasLimits)
		{
			if constexpr (isIntegral)
			{
				ULONGLONG range = static_cast<ULONGLONG>(maxV) - static_cast<ULONGLONG>(minV);

				tick = minV + static_cast<T>(raw % (range + 1));
			}
			else
			{
				double ratio = static_cast<double>(raw) / static_cast<double>(UINT64_MAX); // Percent filled on UINT64 range

				double range = static_cast<double>(maxV) - static_cast<double>(minV); 

				tick = static_cast<T>(static_cast<double>(minV) + ratio * range);
				                // Percent filled on custom range ^^^^^^^^^^^^^
			}
		}
		else
		{
			if constexpr (isIntegral)
			{
				tick = static_cast<T>(raw);
			}
			else
			{
				tick = static_cast<T>(static_cast<double>(raw) / static_cast<double>(UINT64_MAX));
			}
		}

		if constexpr (!isUnsigned)
		{
			if (minV < 0)
			{
				tick = randNum() % 100 <= minusChance ? -tick : tick;
			}
			
		}
	}

public:
	m_random() : seed(GetTickCount64()), tick(0), minusChance(0), hasLimits(false) {}
	m_random(ULONGLONG init_seed) : seed(init_seed), tick(0), minusChance(0), hasLimits(false) {}

	m_random(T minVal, T maxVal) : seed(GetTickCount64()), tick(0), minusChance(0), minV(minVal), maxV(maxVal)
	{
		checkLimits();
		hasLimits = true;
	}
	m_random(ULONGLONG init_seed, T minVal, T maxVal) : seed(init_seed), tick(0), minusChance(0), minV(minVal), maxV(maxVal)
	{
		checkLimits();
		hasLimits = true;
	}

	m_random& genSeed()
	{
		seed = GetTickCount64();
		return *this;
	}

	m_random& setSeed(ULONGLONG userSeed)
	{
		seed = userSeed;
		return *this;
	}

	m_random& setLimits(T minVal, T maxVal)
	{
		minV = minVal;
		maxV = maxVal;
		checkLimits();
		hasLimits = true;
		return *this;
	}

	m_random& setMinusChance(T chance)
	{
		if (chance < 0) chance = 0;
		if (chance > 100) chance = 100;
		minusChance = chance;
		return *this;
	}

	template <typename Y>
	operator Y()
	{
		compileRandNum();
		if constexpr (std::is_same<T, Y>::value) return tick;

		else return static_cast<Y>(tick);
	}

	T operator()()
	{
		compileRandNum();
		return tick;
	}
};