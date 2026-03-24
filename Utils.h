//
// Utils.h
//
#pragma once
#include <string>

namespace UTL
{
	template<typename T>
	inline T GetNumber(std::wstring buffer)
	{
		static constexpr bool isIntegral = std::is_integral<T>::value;
		static constexpr bool isUnsigned = std::is_unsigned<T>::value;
		bool hasSign = false;

		size_t len = buffer.length();

		if constexpr (!isUnsigned)
		{
			if (buffer[0] == L'-')
			{
				buffer.erase(0, 1);
				--len;
				hasSign = true;
			}
		}

		bool hasDot = false;
		auto iter = buffer.begin();
		while (iter != buffer.end())
		{
			
			if (*iter >= L'0' && *iter <= L'9')
			{
				++iter;
				continue;
			}

			if constexpr (!isIntegral)
			{
				if (*iter == L'.')
				{
					if (hasDot)
					{
						buffer.erase(iter);
						--len;
						continue;
					}
					else
					{
						hasDot = true;
						++iter;
						continue;
					}
				}
				else if (*iter == L',')
				{
					if (!hasDot)
					{
						*iter = L'.';
					}
					else
					{
						hasDot = true;
						++iter;
						continue;
					}
				}
			}
			buffer.erase(iter);
			--len;
		}

		if (len == 0) return 0;

		bool isLargeStart = (buffer[0] > L'2');

		if constexpr (std::is_same_v<T, double>)
		{
			if (len > 15) buffer.erase(15);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			if (len > 9) buffer.erase(9);
		}
		else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, long>)
		{
			if (len > 10 && isLargeStart) buffer.erase(10); // 2 000 000 000
		}
		else if constexpr (std::is_same_v<T, short>)
		{
			if (len > 5 && isLargeStart) buffer.erase(5);  // 32 000
		}

		if constexpr (isIntegral)
		{
			return hasSign ? -std::stoi(buffer) : std::stoi(buffer);
		}
		else
		{
			return hasSign ? -std::stod(buffer) : std::stod(buffer);
		}
	}
}
