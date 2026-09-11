//
// Utils.h
//
#pragma once
#include <string>
#include <stdexcept>
#include <limits>

namespace UTL
{
	template<typename T>
	inline T GetNumber(std::wstring buffer)
	{
		static constexpr bool isIntegral = std::is_integral<T>::value;
		static constexpr bool isUnsigned = std::is_unsigned<T>::value;
		bool hasSign = false;

		size_t len = buffer.length();

		size_t index = buffer.find_first_not_of(L" \t\r\n");
		if (index == std::wstring::npos) return 0;

		if constexpr (!isUnsigned)
		{
			if (buffer[index] == L'-')
			{
				hasSign = true;
				index++;
			}
			else if (buffer[index] == L'+')
			{
				index++;
			}
		}

		std::wstring cleanNumber;
		cleanNumber.reserve(buffer.length() - index);
		bool hasDot = false;

		for (size_t i = index; i < buffer.length(); ++i)
		{
			wchar_t ch = buffer[i];
			if (ch >= L'0' && ch <= L'9')
			{
				cleanNumber.push_back(ch);
			}
			else if constexpr (!isIntegral)
			{
				if (ch == L'.' || ch == L',')
				{
					if (!hasDot)
					{
						cleanNumber.push_back(L'.');
						hasDot = true;
					}
				}
			}
		}

		if (cleanNumber.empty()) return 0;

		T maxNumberForT{ (std::numeric_limits<T>::max)() };
		T minNumberForT{ (std::numeric_limits<T>::lowest)() };
		try
		{
			if constexpr (isIntegral)
			{
				if constexpr (isUnsigned)
				{
					unsigned long long val = std::stoull(cleanNumber);
					
					if (val > static_cast<unsigned long long>(maxNumberForT)) return maxNumberForT;
					
					return static_cast<T>(val);
				}
				else
				{
					long long val = std::stoll(cleanNumber);
					if (hasSign) val = -val;

					if (val > static_cast<long long>(maxNumberForT))    return maxNumberForT;
					if (val < static_cast<long long>(minNumberForT))    return minNumberForT;

					return static_cast<T>(val);
				}
			}
			else
			{
				double val = std::stod(cleanNumber);
				if (hasSign) val = -val;

				if (val > static_cast<double>(maxNumberForT))    return maxNumberForT;
				if (val < static_cast<double>(minNumberForT))    return minNumberForT;

				return static_cast<T>(val);
			}
		}
		catch (const std::out_of_range&)
		{
			if (hasSign)
			{
				return minNumberForT; 
			}
			else
			{
				return maxNumberForT;
			}
		}
		catch (...)
		{
			return 0;
		}
	}
}
