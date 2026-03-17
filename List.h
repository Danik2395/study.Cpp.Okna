//
// List.h
//

#pragma once

template<typename T>
class List
{
	struct Node
	{
		T value;
		Node* next;
		Node* prev;
	};

	Node* head_;
	Node* tail_;
	// prev -> head -> tail -> next

public:
	List() : head_(nullptr), tail_(nullptr) {}

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
		Node* newN = new Node{ val, head_, nullptr };
		if (head_) head_->prev = newN;
		head_ = newN;
		if (tail_ == nullptr) tail_ = head_;
	}

	void push_back(const T &val)
	{
		Node* newN = new Node{ val, nullptr, tail_ };
		if (tail_) tail_->next = newN;
		tail_ = newN;
		if (head_ == nullptr) head_ = tail_;
	}

	void pop_front()
	{
		if (!head_) return;

		Node* newHead = head_->next;
		delete head_;
		head_ = newHead;
		if (head_) head_->prev = nullptr;
		else       tail_ = nullptr; // On the deletion of the last element
	}

	void pop_back()
	{
		if (!tail_) return;

		Node* newTail = tail_->prev;
		delete tail_;
		tail_ = newTail;
		if (tail_) tail_->next = nullptr;
		else       head_ = nullptr; // On the deletion of the last element
	}

	T& front() const { return head_->value; }
	T& back()  const { return tail_->value; }

	bool empty() const { return head_ == nullptr; }

	void clear()
	{
		Node* cur = head_;
		while (cur)
		{
			Node* next = cur->next;
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
		while (tempNode != afterLast.ptr_->next)
		{
			delete tempNode->prev;
			tempNode = tempNode->next;
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

				if (cur->value < nextN->value)
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
		while (temp) // Linking previous nodes correctly
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

	List& operator=(List& secList)
	{
		if (&secList == this) return *this;
		Node* cur = secList.head_;
		clear();

		while (cur)
		{
			Node* newNode = new Node{ cur->value, nullptr, tail_ };

			if (head_ != nullptr)
			{
				tail_->next = newNode;
				tail_ = newNode;
			}
			else
			{
				head_ = newNode;
				tail_ = newNode;
			}

			cur = cur->next;
		}

		return *this;
	}
};