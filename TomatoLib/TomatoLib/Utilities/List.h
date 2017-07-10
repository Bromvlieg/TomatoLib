#ifndef TL_U_LIST
#define TL_U_LIST

#include <vector>

template<class Type>
class List : public std::vector < Type > {
public:
	List(void) : Count(0), std::vector<Type>() {}
	List(const List<Type>& copy) : std::vector<Type>(copy), Count(copy.Count) {}
	List(const std::vector<Type>& copy) : std::vector<Type>(copy), Count((int)copy.size()) {}

	int Count;

	inline void Clear() {
		this->clear();
		this->Count = 0;
	}

	inline void Insert(int index, const Type& object) {
		this->insert(this->begin() + index, object);
		this->Count++;
	}

	inline int Add(const Type& object) {
		this->push_back(object);
		return this->Count++;
	}

	inline Type& Push() {
		this->push_back(Type());
		return (*this)[this->Count++];
	}

	inline Type RemoveAt(int index) {
		Type object = (*this)[index];

		this->erase(this->begin() + index);
		this->Count--;

		return object;
	}

	inline void Set(int index, const Type& object) { (*this)[index] = object; }
	inline Type& Get(int index) { return (*this)[index]; }
	inline Type* Buffer() { return this->Count == 0 ? nullptr : &(*this)[0]; }
	inline void Reserve(int num) { this->reserve(num); }
	inline bool Contains(const Type& object) const {
		for (int i = 0; i < this->Count; i++) {
			if ((*this)[i] == object) {
				return true;
			}
		}

		return false;
	}
};

#endif