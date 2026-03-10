#pragma once

#include <stdexcept>
#include <cmath>
#include <initializer_list>
#include <vector>
#include <memory>
#include <cstdint>
#include <functional>

namespace adt::tree{
    template<typename T, typename Comparator = std::less<T>>
    class BST{
    protected:
		template<typename value_t> struct BST_Node{
			value_t value;
			uint32_t count = 1; // for duplicates
			std::unique_ptr<BST_Node<value_t>> left = nullptr;
			std::unique_ptr<BST_Node<value_t>> right = nullptr;
			
			BST_Node(const value_t& a_value)
			:	value(a_value)
			{}
		};
		
		std::unique_ptr<BST_Node<T>> root = nullptr;
		Comparator comp;
		uint32_t totalCount = 0; // includes duplicates
		uint32_t uniqueCount = 0; // excludes duplicates
		
		int8_t compare(const T& a, const T& b) const; // wrapper for comparator
		
		bool isLeaf(const std::unique_ptr<BST_Node<T>>& node) const;
		int32_t height(const std::unique_ptr<BST_Node<T>>& node) const;
		bool isBalanced(const std::unique_ptr<BST_Node<T>>& node) const;
		
		std::unique_ptr<BST_Node<T>>& findMinimum(std::unique_ptr<BST_Node<T>>& node) const;
		
		const T& min(const std::unique_ptr<BST_Node<T>>& node) const;
		const T& max(const std::unique_ptr<BST_Node<T>>& node) const;
		
		void insert(std::unique_ptr<BST_Node<T>>& node, const T& a_value);
		uint32_t search(const std::unique_ptr<BST_Node<T>>& node, const T& a_value) const;
		bool remove(std::unique_ptr<BST_Node<T>>& node, const T& a_value);
		
		void preorder(const std::unique_ptr<BST_Node<T>>& node, std::vector<T>& traversal) const;
		void inorder(const std::unique_ptr<BST_Node<T>>& node, std::vector<T>& traversal) const;
		void postorder(const std::unique_ptr<BST_Node<T>>& node, std::vector<T>& traversal) const;
        
    public:
        BST(const Comparator& c = Comparator());
        BST(std::initializer_list<T> in, const Comparator& c = Comparator());
        BST(const std::vector<T>& in, const Comparator& c = Comparator());
        BST(std::vector<T>&& in, const Comparator& c = Comparator());
		
		// disabled copying
		BST(const BST&) = delete;
		BST& operator=(const BST&) = delete;
		~BST() = default;
		
		bool isLeaf() const;
		bool isEmpty() const; // check for null node
		uint32_t size() const; // size with duplicates
		uint32_t uniqueSize() const; // size without duplicates
		int32_t height() const;
		bool isBalanced() const;
		
		// minimum/maximum of tree
		const T& min() const;
		const T& max() const;
		
		void insert(const T& a_value);
		uint32_t search(const T& a_value) const;
		bool contains(const T& a_value) const;
		bool remove(const T& a_value);
		void clear(); // empties tree and sets root to null node
		
		std::vector<T> preorder() const;
		std::vector<T> inorder() const;
		std::vector<T> postorder() const;
	};

	
	template<typename T, typename Comparator>
	BST<T, Comparator>::BST(const Comparator& c)
	: comp(c)
	{}
	
	template<typename T, typename Comparator>
	BST<T, Comparator>::BST(std::initializer_list<T> in, const Comparator& c)
	:	comp(c)
	{
		for(const auto& value : in)
			insert(value);
	}
	
	template<typename T, typename Comparator>
	BST<T, Comparator>::BST(const std::vector<T>& in, const Comparator& c)
	:	comp(c)
	{
		for(const auto& value : in)
			insert(value);
	}
	
	template<typename T, typename Comparator>
	BST<T, Comparator>::BST(std::vector<T>&& in, const Comparator& c)
	:	comp(c)
	{
		for(const auto& value : in)
			insert(value);
	}
	
	template<typename T, typename Comparator> // returns -1 or 0 or 1
	int8_t BST<T, Comparator>::compare(const T& a, const T& b) const{
		if(comp(a, b)) return -1;
		if(comp(b, a)) return 1;
		return 0;
	}
	
	template<typename T, typename Comparator>
	inline uint32_t BST<T, Comparator>::size() const{
		return totalCount;
	}
	
	template<typename T, typename Comparator>
	inline uint32_t BST<T, Comparator>::uniqueSize() const{
		return uniqueCount;
	}
	
	template<typename T, typename Comparator>
	inline bool BST<T, Comparator>::isEmpty() const{
		return !root;
	}
	
	template<typename T, typename Comparator>
	inline bool BST<T, Comparator>::isLeaf() const{
		return isLeaf(root);
	}
	template<typename T, typename Comparator>
	inline bool BST<T, Comparator>::isLeaf(const std::unique_ptr<BST_Node<T>>& node) const{
		if(!node)
    		return false;
		return !(node->left) && !(node->right);
	}
	
	template<typename T, typename Comparator>
	inline int32_t BST<T, Comparator>::height() const{
		return height(root);
	}
	template<typename T, typename Comparator>
	inline int32_t BST<T, Comparator>::height(const std::unique_ptr<BST_Node<T>>& node) const{
		if(!node)
        	return -1;
			
		// gets maximum of subtree heights and adds 1
		return 1 + std::max(height(node->left), height(node->right));
	}
	
	template<typename T, typename Comparator>
	inline bool BST<T, Comparator>::isBalanced(const std::unique_ptr<BST_Node<T>>& node) const{
		if(!node)
			return true;
		
		int32_t leftHeight = height(node->left);
		int32_t rightHeight = height(node->right);

		if(std::abs(leftHeight - rightHeight) > 1)
			return false;
		
		// recursively checks if subtrees are balanced
		return isBalanced(node->left) && isBalanced(node->right);
	}
	template<typename T, typename Comparator>
	inline bool BST<T, Comparator>::isBalanced() const{
		return isBalanced(root);
	}
	
	template<typename T, typename Comparator>
	inline std::unique_ptr<typename BST<T, Comparator>::BST_Node<T>>& 
	 BST<T, Comparator>::findMinimum(std::unique_ptr<BST_Node<T>>& node) const{
		if(!node->left)
			return node;
		return findMinimum(node->left);
	}
	
	template<typename T, typename Comparator>
	inline const T& BST<T, Comparator>::min(const std::unique_ptr<BST_Node<T>>& node) const{
		if(node->left)
			return min(node->left);
		return node->value;
	}
	template<typename T, typename Comparator>
	inline const T& BST<T, Comparator>::min() const{
		if(isEmpty())
			throw std::runtime_error("min() called on empty tree");
		
		return min(root);
	}
	
	template<typename T, typename Comparator>
	inline const T& BST<T, Comparator>::max(const std::unique_ptr<BST_Node<T>>& node) const{
		if(node->right)
			return max(node->right);
		return node->value;
	}
	template<typename T, typename Comparator>
	inline const T& BST<T, Comparator>::max() const{
		if(isEmpty())
			throw std::runtime_error("max() called on empty tree");
		
		return max(root);
	}
	
	template<typename T, typename Comparator>
	inline void BST<T, Comparator>::insert(const T& a_value){
		insert(root, a_value);
	}
	template<typename T, typename Comparator>
	void BST<T, Comparator>::insert(std::unique_ptr<BST_Node<T>>& node, const T& a_value){
		if(!node){
			node = std::make_unique<BST_Node<T>>(a_value);
			totalCount++;
			uniqueCount++;
		}	
		else{
			int8_t comparison = compare(a_value, node->value);
			
			if(comparison == 0){  // if(a_value == node->value)
				node->count++; // counts as dupliate
				totalCount++;
			}
				
			else if(comparison == -1) // if(a_value < node->value)
				insert(node->left, a_value);
				
			else // if(comparison == 1) AKA if(a_value > node->value)
				insert(node->right, a_value);
		}
	}
	
	template<typename T, typename Comparator>
	inline uint32_t BST<T, Comparator>::search(const T& a_value) const{
		return search(root, a_value);
	}
	template<typename T, typename Comparator>
	uint32_t BST<T, Comparator>::search
	(const std::unique_ptr<BST_Node<T>>& node, const T& a_value) const{
		if(!node)
			return 0;
		
		int8_t comparison = compare(a_value, node->value);
		
		if(comparison == 0) // a_value == node->value
			return node->count;
		
		if(comparison == -1) // a_value < node->value
			return search(node->left, a_value);
		
		// if(comparison == 1), AKA a_value > node->value
		return search(node->right, a_value);
	}
	
	template<typename T, typename Comparator>
	inline bool BST<T, Comparator>::contains(const T& a_value) const{
		return search(root, a_value) > 0;
	}
	
	template<typename T, typename Comparator>
	bool BST<T, Comparator>::remove(const T& a_value){
		return remove(root, a_value);
	}
	template<typename T, typename Comparator>
	bool BST<T, Comparator>::remove(std::unique_ptr<BST_Node<T>>& node, const T& a_value){
		if(!node)
			return false;
		
		int8_t comparison = compare(a_value, node->value);
		
		if(comparison == 0){ // a_value == node->value
			if(node->count > 1){ // if duplicate
				node->count--; // don't delete node, just "remove a duplicate"
				totalCount--;
				return true;
			}
				
			else if(isLeaf(node)){ // trivial deletion
				node.reset(); // OR node = nullptr;
				totalCount--;
				uniqueCount--;
				return true;
			}
			
			else if(node->left && node->right){
				// find the smallest (leftmost) value in the right subtree
				auto& oldNode = findMinimum(node->right);
				
				node->value = oldNode->value;
				node->count = oldNode->count;
				// deletes oldNode and replaces it with its right child (if it exists)
				// no memory leak, unique_ptr takes care of deletion
				oldNode = std::move(oldNode->right);
				
				totalCount--;
				uniqueCount--;
				return true;
			}
			
			else if(node->left){
				// no memory leak, unique_ptr takes care of deletion
				node = std::move(node->left);
				totalCount--;
				uniqueCount--;
				return true;
			}
			
			else if(node->right){
				// no memory leak, unique_ptr takes care of deletion
				node = std::move(node->right);
				totalCount--;
				uniqueCount--;
				return true;
			}
				
			else
				throw std::runtime_error("Edge case when removing node.");
		}
		
		if(comparison == -1) // a_value < node->value
			return remove(node->left, a_value);
		
		//  then comparison == 1 AKA a_value > node->value
		return remove(node->right, a_value);
	}
	
	template<typename T, typename Comparator>
	void BST<T, Comparator>::clear(){
		root.reset(); // automatically deletes children 
		totalCount = 0;
		uniqueCount = 0;
	}
	
	template<typename T, typename Comparator>
	std::vector<T> BST<T, Comparator>::preorder() const{
		if(isEmpty())
			return {}; // empty vector
		
		std::vector<T> traversal;
		preorder(root, traversal);
		return traversal;
	}
	template<typename T, typename Comparator>
	void BST<T, Comparator>::preorder
	(const std::unique_ptr<BST_Node<T>>& node, std::vector<T>& traversal) const{
		traversal.push_back(node->value);
		
		if(node->left)
			preorder(node->left, traversal);
		
		if(node->right)
			preorder(node->right, traversal);
	}
	
	template<typename T, typename Comparator>
	std::vector<T> BST<T, Comparator>::inorder() const{
		if(isEmpty())
			return {}; // empty vector
		
		std::vector<T> traversal;
		inorder(root, traversal);
		return traversal;
	}
	template<typename T, typename Comparator>
	void BST<T, Comparator>::inorder
	(const std::unique_ptr<BST_Node<T>>& node, std::vector<T>& traversal) const{
		if(node->left)
			inorder(node->left, traversal);
		
		traversal.push_back(node->value);
		
		if(node->right)
			inorder(node->right, traversal);
	}
	
	template<typename T, typename Comparator>
	std::vector<T> BST<T, Comparator>::postorder() const{
		if(isEmpty())
			return {}; // empty vector
		
		std::vector<T> traversal;
		postorder(root, traversal);
		return traversal;
	}
	template<typename T, typename Comparator>
	void BST<T, Comparator>::postorder
	(const std::unique_ptr<BST_Node<T>>& node, std::vector<T>& traversal) const{		
		if(node->left)
			postorder(node->left, traversal);
		
		if(node->right)
			postorder(node->right, traversal);
		
		traversal.push_back(node->value);
	}
}