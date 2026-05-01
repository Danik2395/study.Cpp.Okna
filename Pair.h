//
// Pair.h
//

#include <type_traits>
#pragma once


template<typename FirstType, typename SecondType>
class Pair
{
	static constexpr bool isFirstCString =
		std::is_same<FirstType, char*>::value ||
		std::is_same<FirstType, const char*>::value ||
		std::is_same<FirstType, wchar_t*>::value ||
		std::is_same<FirstType, const wchar_t*>::value;

	static constexpr bool isSecondCString =
		std::is_same<SecondType, char*>::value ||
		std::is_same<SecondType, const char*>::value ||
		std::is_same<SecondType, wchar_t*>::value ||
		std::is_same<SecondType, const wchar_t*>::value;

	template<typename U>
	static U getCopyCString(const U &str)
	{
		if (str == nullptr) return nullptr;

		int strCount{0};
		while (str[strCount++] != '\0');
		
		using baseU = std::remove_pointer_t<U>;
		using nonConstBaseU = std::remove_const_t<baseU>;
		nonConstBaseU* copyStr = new nonConstBaseU[strCount]; // Need non constant base type to make new

		while (strCount-- != 0) copyStr[strCount] = str[strCount];

		return reinterpret_cast<U>(copyStr);
	}

	template<typename U>
	static int compareCString(const U &firstStr, const U &secondStr)
	{
		if (firstStr == secondStr) return 0;
		if (firstStr == nullptr) return -1;
		if (secondStr == nullptr) return 1;

		int i{ 0 };
		while (firstStr[i] != '\0')
		{
			if (firstStr[i] != secondStr[i])
			{
				return firstStr[i] < secondStr[i] ? -1 : 1;
			}
			++i;
		}

		if (secondStr[i] != '\0') return -1;
		
		return 0;
	}

public:
	FirstType first;
	SecondType second;

	Pair() : first{}, second{} {}

	Pair(const FirstType &First, const SecondType &Second)
	{
		if constexpr (isFirstCString)
		{
			first = getCopyCString(First);
		}
		else
		{
			first = First;
		}

		if constexpr (isSecondCString)
		{
			second = getCopyCString(Second);
		}
		else
		{
			second = Second;
		}
	}

	Pair(const Pair &secondPair)
	{
		if constexpr (isFirstCString)
		{
			first = getCopyCString(secondPair.first);
		}
		else
		{
			first = secondPair.first;
		}

		if constexpr (isSecondCString)
		{
			second = getCopyCString(secondPair.second);
		}
		else
		{
			second = secondPair.second;
		}
	}

	~Pair()
	{
		if constexpr (isFirstCString)
		{
			delete[] first;
		}

		if constexpr (isSecondCString)
		{
			delete[] second;
		}
	}

	// -1 - this greater
	//  1 - toCompare greater
	//  0 - equal
	int compareFirst(const FirstType &toCompare) const
	{
		if constexpr (isFirstCString)
		{
			return compareCString(first, toCompare);
		}
		else
		{
			if (first < toCompare) return -1;
			if (first > toCompare) return 1;
			return 0;
		}
	}

	// -1 - this greater
	//  1 - toCompare greater
	//  0 - equal
	int compareSecond(const SecondType &toCompare) const
	{
		if constexpr (isSecondCString)
		{
			return compareCString(second, toCompare);
		}
		else
		{
			if (second < toCompare) return -1;
			if (second > toCompare) return 1;
			return 0;
		}
	}

	Pair& operator=(const Pair &secondPair)
	{
		if (this == &secondPair) return *this;

		// Deleting old values first if strings
		if constexpr (isFirstCString)
		{
			delete[] first;
		}
		if constexpr (isSecondCString)
		{
			delete[] second;
		}

		// Copying new values
		if constexpr (isFirstCString)
		{
			first = getCopyCString(secondPair.first);
		}
		else
		{
			first = secondPair.first;
		}

		if constexpr (isSecondCString)
		{
			second = getCopyCString(secondPair.second);
		}
		else
		{
			second = secondPair.second;
		}

		return *this;
	}

	bool operator==(const Pair &secondPair) const
	{
		bool isFirstEqual{};
		if constexpr (isFirstCString)
		{
			isFirstEqual = compareCString(first, secondPair.first) == 0;
		}
		else
		{
			isFirstEqual = first == secondPair.first;
		}

		if (!isFirstEqual) return false;

		bool isSecondEqual{};
		if constexpr (isSecondCString)
		{
			isSecondEqual = compareCString(second, secondPair.second) == 0;
		}
		else
		{
			isSecondEqual = second == secondPair.second;
		}

		return isSecondEqual;
	}

	bool operator!=(const Pair &secondPair) const
	{
		return !(*this == secondPair);
	}
};
