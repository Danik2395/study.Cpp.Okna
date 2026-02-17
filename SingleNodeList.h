//
// SingleNodeList.h
//

#pragma once

template<typename T>
class SingleNodeList
{
	struct Node
	{
		T value;
		Node* next;
	};

	Node* head_;
	Node* tail_;

public:
	SingleNodeList() : head_(nullptr), tail_(nullptr) {}

	~SingleNodeList() { clear(); }

	class Iterator
	{
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

		bool operator==(const Iterator &iter) { return ptr_ == iter.ptr_; } // Not a pointer because of &
		bool operator!=(const Iterator &iter) { return ptr_ != iter.ptr_; } // Comparing iterator only with other iterator
	};

	auto begin() const { return Iterator(head_); }   // Create and return Iterator type variable that points on the head_
	auto end()   const { return Iterator(nullptr); } // After the tail_

	void push_front(const T &val)
	{
		Node* newN = new Node{ val, head_ };
		head_ = newN;
		if (tail_ == nullptr) tail_ = head_;
		// nn -> h
		// h
		// t
		// nn -> t
		//  h -> t
		// nn -> h -> t
		//  h -> n -> t
	}

	void pop_front()
	{
		if (!head_) return;

		Node * newHead = head_->next;
		delete head_;
		head_ = newHead;
	}

	T &front() const { return head_->value; }
	T &back() const { return tail_->value; }
	 

	bool empty() const { return head_ == nullptr; }

	void clear()
	{
		Node* cur = head_;
		while (cur) {
			Node* next = cur->next;
			delete cur;
			cur = next;
		}
		head_ = nullptr;
		tail_ = nullptr;
	}

	void sort()
	{
		if (!head_ || !head_->next) return;

		Node* endN = nullptr;         // Optimization for bubble sort
		Node preHead;
		preHead.next = head_;

		bool swapped = false;
		do                            // If any sort accrued continue
		{
			swapped = false;
			Node* prev = &preHead;
			Node* cur = preHead.next;

			while (cur->next != endN) // While not nullptr, then move the limiter (endN) closer to start of the list
			{
				Node* nextN = cur->next;

				if (cur->value < nextN->value)
				{
					swapped = true;
					cur->next = nextN->next;
					nextN->next = cur;
					prev->next = nextN;

					prev = nextN;
					// d -> e -> f -> g  *
					// 
					// p -> c -> n -> g
					// p -> c    n -> g
					//      \________/
					// p -> c <- n    g
					//      \_________/
					// p -> n -> c -> g
					// 
					// d -> f -> e -> g  *
					//
					// d -> p -> c -> n
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
	}

	// Non optimal
	void sort2()
	{
		if (!head_ || !head_->next) return;

		Node* endN = nullptr;         // Optimization for bubble sort

		bool swapped = false;
		do
		{
			swapped = false;
			Node* cur = head_;

			while (cur->next != endN)
			{
				if (cur->value < cur->next->value)
				{
					swapped = true;
					T tempVal = cur->value;
					cur->value = cur->next->value;
					cur->next->value = tempVal;
				}

				cur = cur->next;
			}

			endN = cur;
		} while (swapped);
	}

	SingleNodeList& operator=(SingleNodeList &secList)
	{
		if (&secList == this) return *this;
		Node* cur = secList.head_;                           // Not a pointer because of &
		clear();

		while (cur)
		{
			Node* newNode = new Node{ cur->value, nullptr }; // Creating new tail node with value as in list to copy

			if (head_ != nullptr)
			{
				tail_->next = newNode;                       // Then inserting new nodes after the tail and moving it to that node
				tail_ = newNode;
			}
			else                                             // On the first iter both the head and the tail are pointing on the created node
			{
				head_ = newNode;
				tail_ = newNode;
			}

			cur = cur->next;
		}

		return *this;
	}
};