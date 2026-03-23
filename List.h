//
// List.h
//

#include <type_traits>
#pragma once


template<typename T>
class List
{
	static constexpr bool isCString =
		std::is_same<T, char*>::value ||
		std::is_same<T, const char*>::value ||
		std::is_same<T, wchar_t*>::value ||
		std::is_same<T, const wchar_t*>::value;

	struct Node
	{
		T value;
		Node* next;
		Node* prev;
	};

	Node* head_;
	Node* tail_;
	// prev -> head -> tail -> next

	size_t size_;

	template<typename U = T> // Template is needed only because enable_if works with templates. And method call would be actual type
	typename std::enable_if<isCString, U>::type
	getCopyCString(const U &str)
	{
		if (str == nullptr) return nullptr;

		int strCount{1}; // Mind the '\n'
		while (str[strCount] != '\0') ++strCount;
		
		using baseU = std::remove_pointer<U>::type;
		using nonConstBaseU = std::remove_const<baseU>::type;
		nonConstBaseU* copyStr = new nonConstBaseU[strCount];

		--strCount;      // To match the index
		while (strCount != -1) copyStr[strCount] = str[strCount--];

		return reinterpret_cast<U>(copyStr);
	}

	template<typename U = T>
	typename std::enable_if<isCString, bool>::type
	compareCString(const U &firstStr, const U &secondStr)
	{
		int i{ 0 };
		while (firstStr[i] != L'\0')
		{
			if (firstStr[i] != secondstr[i]) return false;
			++i;
		}
		return secondStr[i] == L'\0';
	}

public:
	List() : head_(nullptr), tail_(nullptr), size_(0) {}

	List(const List &secondList) : head_(nullptr), tail_(nullptr), size_(0)
	{
		*this = secondList;
	}

	~List() { clear(); }

	class Iterator
	{
		friend class List; // Because of access logic in List
		
		Node* ptr_;

	public:
		Iterator(Node* ptr) : ptr_(ptr) {}

		T& operator*() const { return ptr_->value; }
		T* operator->() const { return &(ptr_->value); }
		// iter->field
		// (iter.operator->())->field
		// pointerOnTValue->field

		Iterator& operator++()   // ++iter
		{
			if (ptr_) ptr_ = ptr_->next;
			return *this;
		}

		Iterator operator++(int) // iter++
		{
			Iterator tmp = *this;
			if (ptr_) ptr_ = ptr_->next;
			return tmp;
		}

		Iterator& operator--()   // --iter
		{
			if (ptr_) ptr_ = ptr_->prev;
			return *this;
		}

		Iterator operator--(int) // iter--
		{
			Iterator tmp = *this;
			if (ptr_) ptr_ = ptr_->prev;
			return tmp;
		}

		bool operator==(const Iterator &iter) { return ptr_ == iter.ptr_; }
		bool operator!=(const Iterator &iter) { return ptr_ != iter.ptr_; }
	};

	auto begin() const { return Iterator(head_); }
	auto end()   const { return Iterator(nullptr); }

	void push_front(const T &val)
	{
		T actualVal = val;
		if constexpr (isCString)
		{
			actualVal = getCopyCString(val);
		}

		Node* newN = new Node{ actualVal, head_, nullptr };
		if (head_) head_->prev = newN;
		head_ = newN;
		if (tail_ == nullptr) tail_ = head_;
		++size_;
	}

	void push_back(const T &val)
	{
		T actualVal = val;
		if constexpr (isCString)
		{
			actualVal = getCopyCString(val);
		}

		Node* newN = new Node{ actualVal, nullptr, tail_ };
		if (tail_) tail_->next = newN;
		tail_ = newN;
		if (head_ == nullptr) head_ = tail_;
		++size_;
	}

	void pop_front()
	{
		if (!head_) return;

		Node* newHead = head_->next;

		if constexpr (isCString)
		{
			delete[] head_->value;
		}
		delete head_;

		head_ = newHead;
		if (head_) head_->prev = nullptr;
		else       tail_ = nullptr; // On the deletion of the last element
		--size_;
	}

	void pop_back()
	{
		if (!tail_) return;

		Node* newTail = tail_->prev;

		if constexpr (isCString)
		{
			delete[] tail_->value;
		}
		delete tail_;

		tail_ = newTail;
		if (tail_) tail_->next = nullptr;
		else       head_ = nullptr; // On the deletion of the last element
		--size_;
	}

	T& front() const { return head_->value; }
	T& back()  const { return tail_->value; }

	bool empty() const { return head_ == nullptr; }

	size_t size() const { return size_; }

	void clear()
	{
		Node* cur = head_;
		while (cur)
		{
			Node* next = cur->next;
			
			if constexpr (isCString)
			{
				delete[] cur->value;
			}
			delete cur;

			cur = next;
		}
		head_ = nullptr;
		tail_ = nullptr;
	}

	// Deletes element on the specified position or range of elements
	Iterator erase(Iterator Where)
	{
		Node* wantedNode = Where.ptr_;     // No exception checking because of O(1) access (because we have Where)

		Node* prevNode = wantedNode->prev; // Remembering extreme nodes
		Node* nextNode = wantedNode->next;

		if (prevNode) prevNode->next = nextNode;
		else head_ = nextNode;             // Deleting head
		
		if (nextNode) nextNode->prev = prevNode;
		else tail_ = prevNode;             // Deleting tail

		if constexpr (isCString)
		{
			delete[] wantedNode->value;
		}
		delete wantedNode;                 // Deleting the wanted node

		return nextNode ? nextNode : end();
	}

	// Deletes element on the specified position or range of elements
	Iterator erase(Iterator first, Iterator afterLast)
	{
		if (first == afterLast) return afterLast;

		if (first.ptr_->prev) first.ptr_->prev->next = afterLast.ptr_; // Separating inner range
		else head_ = afterLast.ptr_;

		if (afterLast.ptr_) afterLast.ptr_->prev = first.ptr_->prev;
		else tail_ = first.ptr_->prev;

		Node* tempNode = first.ptr_->next;
		while (tempNode != afterLast.ptr_)
		{
			if constexpr (isCString)
			{
				delete[] tempNode->prev->value;
			}
			tempNode = tempNode->next;
			delete tempNode->prev;
		}
	}

	void sort()
	{
		if (!head_ || !head_->next) return;

		Node* endN = nullptr;
		Node preHead;
		preHead.next = head_;
		preHead.prev = nullptr;

		bool swapped = false;
		do
		{
			swapped = false;
			Node* prev = &preHead;
			Node* cur = preHead.next;

			while (cur->next != endN)
			{
				Node* nextN = cur->next;

				bool isNextGreater{};
				if constexpr (isCString)
				{
					isNextGreater = compareCString(cur->value, nextN->value);
				}
				else
				{
					isNextGreater = cur->value < nextN->value;
				}

				if (isNextGreater)
				{
					swapped = true;
					cur->next = nextN->next;
					nextN->next = cur;
					prev->next = nextN;

					prev = nextN;
				}
				else
				{
					prev = cur;
					cur = cur->next;
				}
			}

			endN = cur;
		} while (swapped);

		head_ = preHead.next;

		Node* temp = head_;
		while (temp->next) temp = temp->next; // Recalculating tail
		tail_ = temp;

		temp = head_;
		Node* tempPrev = nullptr;
		while (temp)                          // Linking previous nodes correctly
		{
			temp->prev = tempPrev;
			tempPrev = temp;
			temp = temp->next;
		}
	}

	// Changes order in the list on the opposite
	void reverse()
	{
		if (head_ == nullptr) return;

		Node* curNode = head_;

		Node* tempNode;
		while (curNode != nullptr) // Swapping pointers directions till the end of the list
		{
			tempNode = curNode->next;

			curNode->next = curNode->prev;
			curNode->prev = tempNode;

			curNode = tempNode;
		}

		tempNode = head_;          // Then changing head_ and tail_ so they point on the "head" and the "tail"
		head_ = tail_;
		tail_ = tempNode;
	}

	List& operator=(const List& secondList)
	{
		if (&secondList == this) return *this;
		Node* cur = secondList.head_;
		clear();

		while (cur)
		{
			//Node* newNode = new Node{ cur->value, nullptr, tail_ };

			//if (head_ != nullptr)
			//{
			//	tail_->next = newNode;
			//	tail_ = newNode;
			//}
			//else
			//{
			//	head_ = newNode;
			//	tail_ = newNode;
			//}

			push_back(cur->value);
			cur = cur->next;
		}

		return *this;
	}
};

using pcwList = List<const wchar_t*>;
using cwList = List<const wchar_t>;
