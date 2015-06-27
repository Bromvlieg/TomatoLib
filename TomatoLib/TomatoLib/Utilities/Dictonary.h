#ifndef BROMMC_DICTONARY
#define BROMMC_DICTONARY

#include "List.h"

#ifndef null
#define null 0
#endif

template<class KeyType, class ValueType>
class Dictonary {
public:
	List<KeyType> Keys;
	List<ValueType> Values;

	Dictonary() : Count(0) { }
	Dictonary(const Dictonary<KeyType, ValueType>& copy): Count(copy.Count), Keys(copy.Keys), Values(copy.Values) { }

	int Count;
	void Clear() {
		this->Keys.Clear();
		this->Values.Clear();
		this->Count = 0;
	}

	inline bool ContainsKey(const KeyType& key) {
		return this->IndexOfKey(key) > -1;
	}

	inline bool ContainsValue(const ValueType& value) {
		return this->IndexOfValue(value) > -1;
	}

	inline int IndexOfKey(const KeyType& key) {
		for (int i = 0; i < this->Count; i++) {
			if (this->Keys[i] == key) {
				return i;
			}
		}

		return -1;
	}

	inline bool IndexOfValue(const ValueType& value) {
		for (int i = 0; i < this->Count; i++) {
			if (this->Values[i] == value) {
				return i;
			}
		}

		return -1;
	}

	inline ValueType& operator[](const KeyType& key) {
		for (int i = 0; i < this->Count; i++) {
			if (this->Keys[i] == key) {
				return this->Values[i];
			}
		}

		this->Count++;
		this->Keys.Add(KeyType(key));
		this->Values.Add(ValueType());
		return this->Values[this->Count - 1];
	}

	inline void Insert(int index, const KeyType& key, const ValueType& object) {
		this->Keys.insert(this->Keys.begin() + index, key);
		this->Values.insert(this->Values.begin() + index, object);
		this->Count++;
	}

	inline int Add(const KeyType& key, const ValueType& object) {
		this->Keys.push_back(key);
		this->Values.push_back(object);
		return this->Count++;
	}

	inline ValueType Remove(const KeyType& key) {
		for (int i = 0; i < this->Count; i++) {
			if (this->Keys[i] == key) {
				this->Count--;

				this->Keys.RemoveAt(i);
				return this->Values.RemoveAt(i);
			}
		}

		throw;
	}

	inline ValueType RemoveAt(int index) {
		ValueType object = this->Values[index];

		this->Keys.erase(this->Keys.begin() + index);
		this->Values.erase(this->Values.begin() + index);
		this->Count--;

		return object;
	}
};

#endif