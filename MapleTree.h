//
// MapleTree.h
//

#include <type_traits>
#include <concepts>
#include "Stack.h"
#include "Pair.h"
#include "List.h"
#pragma once

template<typename Lambda, typename LambdaKeyT, typename LambdaValueT, typename... Extras>
concept LambdaRecurrent = requires(
	Lambda lambdaF,
	LambdaKeyT key,
	LambdaValueT value,
	Extras&&... args
	)
{
	{ lambdaF(key, value, args...) } -> std::same_as<void>;
};

template<typename Lambda, typename LambdaKeyT, typename LambdaValueT, typename... Extras>
concept LambdaStructural = requires(
	Lambda lambdaF,
	LambdaKeyT key,
	LambdaValueT value,
	const LambdaKeyT* parentKey,
	Extras&&... args
	)
{
	{ lambdaF(key, value, parentKey, args...) } -> std::same_as<void>;
};

template<typename KeyType, typename ValueType>
class MapleTree
{
	struct Node
	{
		Pair<KeyType, ValueType> data;
		Node* left;
		Node* right;
		int height;
	};

	Node* root_;

	size_t size_;

	int height(Node* node) { return node ? node->height : 0; }

	void updateHeight(Node* node)
	{
		int leftNodeHeight = height(node->left);
		int rightNodeHeight = height(node->right);
		int maxHeight = leftNodeHeight > rightNodeHeight ? leftNodeHeight : rightNodeHeight;
		node->height = 1 + maxHeight;
	}

	Node* rotateRight(Node* node)
	{
		// Right turn (left is heavier)                                                                                
		//                                                                                                             
		//               node (to make turn on)                leftNode                                                
		//              /                                      /     \                                                 
		//          leftNode                      ->        newNode  node (greater than leftNode)                      
		//           /   \                                           /                                                 
		//     newNode  leftRightNode (or nullptr)                leftRightNode (less than leftNode, greater than node)
		//                                                                                                             
		Node* leftNode = node->left;
		Node* leftRightNode = leftNode->right;

		leftNode->right = node;
		node->left = leftRightNode;

		updateHeight(node);
		updateHeight(leftNode);

		return leftNode;
	}

	Node* rotateLeft(Node* node)
	{
		// Left turn (right is heavier)                                                                             
		//                                                                                                          
		//               node (to make turn on)            rightNode                                                
		//                  \                               /     \                                                 
		//               rightNode                ->      node   newNode                                            
		//                /    \                             \                                                      
		//      rightLeftNode   newNode                     rightLeftNode (greater than node, less than rightNode)  
		//                                                                                                          
		Node* rightNode = node->right;
		Node* rightLeftNode = rightNode->left;

		rightNode->left = node;
		node->right = rightLeftNode;

		updateHeight(node);
		updateHeight(rightNode);

		return rightNode;
	}

	Node* balance(Node* node)
	{
		updateHeight(node);
		int bf = height(node->left) - height(node->right);

		if (bf > 1)                                  // Right is heavier
		{
			                                         // Left-right zigzag
			if (height(node->left->right) > height(node->left->left))
			{
				node->left = rotateLeft(node->left); // Child first
			}
			return rotateRight(node);                // Regular case
		}
		else if (bf < -1)                            // Left is heavier
		{
			                                         // Right-left zigzag
			if (height(node->right->left) > height(node->right->right))
			{
				node->right = rotateRight(node->right);
			}
			return rotateLeft(node);
		}

		return node;                                 // In balance
	}

	void balancePath(Stack<Node*> &path)
	{
		while (!path.empty())
		{
			Node* node = path.top();
			path.pop();

			Node* balancedNode = balance(node);

			if (path.empty())
			{
				root_ = balancedNode;                                  // Root node at the end of path
			}
			else
			{
				Node* parent = path.top();                             // Linking node to the parent
				if (parent->left == node) parent->left = balancedNode; // Comparing the pointers
				else parent->right = balancedNode;
			}
		}
	}

	template<typename Lambda, typename... Extras>
	void traverseRecursive(Node* node, Lambda &&callback, Extras&&... args)
	{
		if (!node) return; // On the nullptr end (leaf)

		traverseRecursive(node->left, callback, std::forward<Extras>(args)...);
		callback(node->data.first, node->data.second, std::forward<Extras>(args)...);
		traverseRecursive(node->right, callback, std::forward<Extras>(args)...);
	}

	template<typename Lambda, typename... Extras>
	void crawlDownwards(Node* node, Lambda&& callback, const KeyType* parentKey, Extras&&... args)
	{
		if (!node) return;
		callback(node->data.first, node->data.second, parentKey, std::forward<Extras>(args)...);
		crawlDownwards(node->left, callback, &node->data.first, std::forward<Extras>(args)...);
		crawlDownwards(node->right, callback, &node->data.first, std::forward<Extras>(args)...);
	}

public:
	MapleTree() : root_(nullptr), size_(0) {}

	MapleTree(const MapleTree &secondMapleTree) : root_(nullptr), size_(0)
	{
		*this = secondMapleTree;
	}

	~MapleTree() { clear(); }

	class Iterator
	{
		friend class MapleTree;

		Stack<Node*, List<Node*>> nodeStack_;

		static Iterator reconstructIter(Node* root, const KeyType& key)
		{
			Iterator reconstructedIter(nullptr);
			Node* cur = root;

			while (cur != nullptr)
			{
				int keyCmp = cur->data.compareFirst(key);
				if (keyCmp < 0)
				{
					reconstructedIter.nodeStack_.push(cur);
					cur = cur->left;
				}
				else if (keyCmp > 0)
				{
					cur = cur->right; // Same logic as DFS iteration
				}
				else
				{
					reconstructedIter.nodeStack_.push(cur);
					return reconstructedIter;
				}
			}

			return Iterator(nullptr);
		}

	public:
		// How to iterate no the tree (DFS [Deep-First Search]):
		// 1. If current node is in the stack and has pointer to the left (branch with decreasing values) node
		//    - Move all the way to the bottom of left branch pushing all nodes to the stack
		//    - Remove stack.top() node
		//    - Return current node wanted data
		// 2. If current node has pointer to the right (branch with increasing values) node
		//    - Push right node to the stack
		//    - Move to the right node
		//    - Move to the step 1
		// 3. Move to the stack.top() node
		//    - Remove stack.top() node
		//    - Return current node wanted data

		Iterator(Node* root = nullptr)
		{
			if (!root) return;
			while (root)
			{
				nodeStack_.push(root);
				root = root->left;
			}
		}

		auto& operator*() const { return nodeStack_.top()->data; }
		auto* operator->() const { return &(nodeStack_.top()->data); }

		Iterator& operator++()   // ++iter
		{
			Node* currentNode_ = nodeStack_.top();
			nodeStack_.pop();
			if (currentNode_->right != nullptr)
			{
				currentNode_ = currentNode_->right;
				while (currentNode_ != nullptr)
				{
					nodeStack_.push(currentNode_);
					currentNode_ = currentNode_->left;
				}
			}
			return *this;
		}

		Iterator operator++(int) // iter++
		{
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

		Iterator& operator--()   // --iter
		{		
			Node* currentNode_ = nodeStack_.top();
			nodeStack_.pop();
			if (currentNode_->left != nullptr)
			{
				currentNode_ = currentNode_->left;
				while (currentNode_ != nullptr)
				{
					nodeStack_.push(currentNode_);
					currentNode_ = currentNode_->right;
				}
			}
			return *this;
		}

		Iterator operator--(int) // iter--
		{
			Iterator tmp = *this;
			--(*this);
			return tmp;
		}

		// No copy overload needed

		bool operator==(const Iterator &iter) const
		{
			bool is_this_empty = nodeStack_.empty();
			bool is_second_empty = iter.nodeStack_.empty();
			if (is_this_empty && is_second_empty) return true;
			if (is_this_empty || is_second_empty) return false;
			return nodeStack_.top() == iter.nodeStack_.top();
		}
		bool operator!=(const Iterator &iter) const { return !(*this == iter); }
	};

	auto begin() const { return Iterator(root_); }
	auto end() const { return Iterator(nullptr); }

	// callback(KeyType, ValueType, Extras)
	template<typename Lambda, typename... Extras>
	requires LambdaRecurrent<Lambda, KeyType, ValueType, Extras...>
	void ForEach(Lambda &&callback, Extras&&... args)
	{
		traverseRecursive(root_, std::forward<Lambda>(callback), std::forward<Extras>(args)...);
	}

	// callback(KeyType, ValueType, int Depth, const KeyType* parentKey)
	template<typename Lambda, typename... Extras>
	requires LambdaStructural<Lambda, KeyType, ValueType, Extras...>
	void ForEachStructural(Lambda&& callback, Extras&&... args)
	{
		crawlDownwards(root_, std::forward<Lambda>(callback), nullptr, std::forward<Extras>(args)...);
	}

	Iterator find(const KeyType &key)
	{
		return Iterator::reconstructIter(root_, key);
	}

	auto insert(const Pair<KeyType, ValueType> &data)
	{
		return insert(data.first, data.second);
	}

	// bool - true if inserted
	// bool - false if not inserted
	Pair<Iterator, bool> insert(const KeyType &key, const ValueType &value)
	{
		if (root_ == nullptr)
		{
			root_ = new Node{ {key, value}, nullptr, nullptr, 0 };
			++size_;
			return Pair<Iterator, bool>(Iterator(root_), true);
		}

		Stack<Node*> path;
		Node* cur = root_;

		bool isInserted = false;
		while (cur != nullptr)
		{
			path.push(cur);

			if (cur->data.compareFirst(key) < 0)      // key less than node -> go left
			{
				if (cur->left == nullptr)             // No node on the left -> create new node
				{
					cur->left = new Node{ {key, value}, nullptr, nullptr, 0 };
					++size_;
					cur = cur->left;
					isInserted = true;
					break;
				}
				cur = cur->left;                      // Go further
			}
			else if (cur->data.compareFirst(key) > 0) // key greater than node -> go right
			{
				if (cur->right == nullptr)            // No node on the left -> create new node
				{
					cur->right = new Node{ {key, value}, nullptr, nullptr, 0 };
					++size_;
					cur = cur->right;
					isInserted = true;
					break;
				}
				cur = cur->right;                     // Go further
			}
			else break;
		}

		if (isInserted)
		{
			balancePath(path);
		}
			
		Iterator iter = find(cur->data.first);
		return Pair<Iterator, bool>(iter, isInserted);
	}

	bool  empty() const { return root_ == nullptr; }

	size_t size() const { return size_; }

	// Deletes element with a specified key
	void erase(const KeyType& key)
	{
		// How to erase element from AVL tree:
		// Go to desired node and write down a path to it.
		//
		// Delete node based on its type:
		//    1. Leaf node
		//        - Just delete the node
		//    2. Single child node
		//        - Link desired node child to the desired node parent
		//        - Delete desired node
		//    3. Double child node
		//        - Look at height of left and right subtree
		//            - If left is higher find max value node of the left subtree
		//                - From the root of left subtree all the way to the right
		//            - If right is higher find min value node of the right subtree
		//                - From the root of right subtree all the way to the left
		//        - Copy data from found node to the desired node
		//        - Delete found node
		// Go back following the created path whilst balancing every node.

		if (root_ == nullptr) return;

		Stack<Node*> path;
		Node* cur = root_;

		while (cur != nullptr)
		{
			if (cur->data.compareFirst(key) < 0)      // Go left
			{
				path.push(cur);
				cur = cur->left;
			}
			else if (cur->data.compareFirst(key) > 0) // Go right
			{
				path.push(cur);
				cur = cur->right;
			}
			else                                      // Found
			{
				break;
			}
		}

		if (cur == nullptr) return;                   // Key not found

		                                              // Double child node logic
		if (cur->left != nullptr && cur->right != nullptr)
		{
			path.push(cur);                           // Push because in stack last is current node parent
			Node* transferNode = nullptr;

			                                          // Look at height of left and right subtree
			if (height(cur->left) > height(cur->right))
			{
				                                      // Left higher
				transferNode = cur->left;
				while (transferNode->right != nullptr)
				{
					path.push(transferNode);
					transferNode = transferNode->right;
				}
			}
			else
			{
				                                      // Right higher
				transferNode = cur->right;
				while (transferNode->left != nullptr)
				{
					path.push(transferNode);
					transferNode = transferNode->left;
				}
			}

			cur->data = transferNode->data;           // Copy data to desired node
			
			cur = transferNode;                       // Now need to delete the found node
		}
		                                              // Leaf node or Single child node (only left or only right)
		Node* child = cur->left != nullptr ? cur->left : cur->right;

		if (path.empty())                             // Only in case of deleting root node
		{
			root_ = child;
		}
		else
		{
			Node* parent = path.top();                // No matter what direction child will go to the desired or transfer node (parents child)
			if (parent->left == cur) parent->left = child;
			else parent->right = child;
		}

		delete cur;
		--size_;

		balancePath(path);
	}

	// Deletes element on the specified position or range of elements
	Iterator erase(Iterator Where)
	{
		if (Where == end()) return end();

		Iterator nextIter = Where;
		++nextIter;                             // Determine the next iterator

		bool hasNext = (nextIter != end());
		KeyType nextKey;
		if (hasNext) nextKey = nextIter->first; // Remember key because it won't change after erase.
		                                        // In the stack would be this node because it is the node after the deleted node

		erase(Where->first);

		if (!hasNext) return end();

		return Iterator::reconstructIter(root_, nextKey);
	}

	// Deletes element on the specified position or range of elements
	Iterator erase(Iterator first, Iterator afterLast)
	{
		while (first != afterLast)
		{
			first = erase(first);
		}
		return afterLast;
	}

	void clear()
	{
		if (!root_) return;
		Iterator iter = this->begin();
		while (iter != this->end())
		{
			iter = erase(iter);
		}
		root_ = nullptr;
		size_ = 0;
	}

	ValueType& operator[](const KeyType& key)
	{
		return insert(key, {}).first->second;
	}

	MapleTree& operator=(const MapleTree& secondMapleTree)
	{
		// BFS based copy. To not to rebalance tree (if iterator copy)
		if (&secondMapleTree == this) return *this;
		clear();
		if (!secondMapleTree.root_)
		{
			root_ = nullptr;
			size_ = 0;
			return *this;
		}

		struct CopyPair
		{
			const Node* source;
			Node* target;
		};
		List<CopyPair> copyList;

		root_ = new Node{
			secondMapleTree.root_->data,
			nullptr,
			nullptr,
			secondMapleTree.root_->height
		};
		copyList.push_back({ secondMapleTree.root_, root_ });

		while (!copyList.empty())
		{
			CopyPair current = copyList.front();
			copyList.pop_front();

			if (current.source->left)
			{
				current.target->left = new Node{
					current.source->left->data,
					nullptr,
					nullptr,
					current.source->left->height
				};
				copyList.push_back({ current.source->left, current.target->left });
			}

			if (current.source->right)
			{
				current.target->right = new Node{
					current.source->right->data,
					nullptr,
					nullptr,
					current.source->right->height
				};
				copyList.push_back({ current.source->right, current.target->right });
			}
		}
		size_ = secondMapleTree.size_;
		return *this;
	}
};
