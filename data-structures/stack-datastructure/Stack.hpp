#include <vector>
#include <stdexcept>

namespace adt{
	template<typename T> class Stack{
	private:
		std::vector<T> data;
		
	public:
		Stack(size_t initialSize = 0);
		
		~Stack() = default;
		
		Stack(const Stack& other);
		Stack& operator=(const Stack& other);
		
		void push(const T& item);
		T pop();
		const T& peek() const;
		bool isEmpty() const noexcept;
		
		size_t size() const noexcept;
		void reserve(size_t size);
		void clear() noexcept;
	};
	
	template<typename T>
	Stack<T>::Stack(size_t initialSize){
		data.reserve(initialSize);
	}
	
	template<typename T>
	Stack<T>& Stack<T>::operator=(const Stack& other){
		data = other.data;
		return *this;
	}
	
	template<typename T>
	Stack<T>::Stack(const Stack& other){
		*this = other;
	}
	
	template<typename T>
	inline void Stack<T>::push(const T& item){
		data.push_back(item);
	}
	
	template<typename T>
	inline T Stack<T>::pop(){
		if(data.empty())
			throw std::underflow_error("Stack underflow");
		
		T item = data.back();
		data.pop_back();
		return item;
	}
	
	template<typename T>
	inline const T& Stack<T>::peek() const{
		if(data.empty())
			throw std::underflow_error("Stack underflow");
			
		return data.back();
	}
	
	template<typename T>
	inline bool Stack<T>::isEmpty() const noexcept{
		return data.empty();
	}
	
	template<typename T>
	inline size_t Stack<T>::size() const noexcept{
		return data.size();
	}
	
	template<typename T>
	void Stack<T>::reserve(size_t size){
		data.reserve(size);
	}
	
	template<typename T>
	inline void Stack<T>::clear() noexcept{
		data.clear();
	}
}