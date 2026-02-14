//
// Stack.h
//
#include "SingleNodeList.h"
#include <type_traits>
#pragma once

template<typename T, class CONTAINER = SingleNodeList<T>>
class Stack
{
	static constexpr bool isFList = std::is_same<CONTAINER, SingleNodeList<T>>::value;

	CONTAINER cont;
	size_t size_;

public:
	Stack() : size_(0) {}

	void push(const T &val)
	{
		if constexpr (isFList)
		{
			return cont.push_front(val);
		}
		else
		{
			return cont.push_back(val);
		}
		++size_;
	}

	void pop()
	{
		if constexpr (isFList)
		{
			return cont.pop_front();
		}
		else
		{
			return cont.pop_back();
		}
		--size_;
	}

	T& top()
	{
		if constexpr (isFList)
		{
			return cont.front();
		}
		else
		{
			return cont.back();
		}
	}

	bool empty() { return cont.empty(); }

	size_t size() { return size_; }



	// Only for lab task
	
	auto begin() const { return cont.begin(); }
	auto end()   const { return cont.end(); }

	void sort() { cont.sort(); }
};