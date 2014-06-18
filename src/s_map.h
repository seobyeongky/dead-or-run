/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	s_map.h

	SMap template container class의 header file입니다.

	SMap은 Static Map의 준말입니다.

	사용법 :

	{
		SMap<int, string> s_map;

		s_map.Insert(38, "ho ho");
		s_map.Insert(3232, "ha ha");

		cout << "38 : " <<  s_map[38] << endl;
	}

=======================================================*/

#pragma once

#include <algorithm>
#include <vector>
#include <list>

template <typename KeyT, typename T>
class SMap {
/*	연관 배열(associative array) container입니다.
	STL의 map은 red-black tree를 사용했지만 이 class는 다른 방식인데
	작은 용량, 드문 삽입, 삭제에 더 적합한 sorted vector방식입니다.
	검색 오버헤드가 상대적으로 작다고 할 수 있겠습니다.
	단, KeyT 타입은 < 연산자가 구현되어있어야합니다. */

private:
	typedef typename std::list<T>			el_list_t;
	typedef typename el_list_t::iterator	iter_t;

public:
	class Pair
	{
		friend class SMap;

	public:
		const KeyT &	GetKey() const { return _key; }
		T &				GetEl() { return *_iter;}

		bool	operator < (const KeyT & key) const { return this->_key < key; }

	private:
		KeyT		_key;
		iter_t		_iter;

		Pair() : _key(), _iter() { }
		Pair(const KeyT & key, const iter_t & iter) : _key(key), _iter(iter) { }
	};

private:
	typedef std::vector<Pair>	pair_vec_t;

public:
	class Iter
	{
	friend class SMap;
	
	private:
		typedef typename pair_vec_t::iterator iter_type_t; 

		iter_type_t _it;

	public:
				 Iter() : _it() { }
				 Iter(const Iter & iter) : _it(iter._it) { }
				 Iter(const iter_type_t & it) : _it(it) { }
		Iter &	 operator = (const Iter & iter) { _it = iter._it; return *this; }
				~Iter() { }

		Pair	 operator * () { return *_it; }
		Pair	 operator -> () { return *_it; }
		bool	 operator == (const Iter & iter) const { return this->_it == iter._it; }
		bool	 operator != (const Iter & iter) const { return this->_it != iter._it; }
		Iter &	 operator ++ () { ++_it; return *this; }
		Iter &	 operator -- () { --_it; return *this; }
		Iter &	 operator += (int n) { _it += n; return *this; }
		Iter &	 operator -= (int n) { _it -= n; return *this; }
		Iter &	 operator + (int n) { Iter new_it(*this); new_it += n; return new_it; }
		Iter &	 operator - (int n) { Iter new_it(*this); new_it -= n; return new_it; }
	};

public:
			 SMap() : _pair_vec(), _el_list() { }
			~SMap() { }

	// 원소를 삽입합니다.
	// key가 이미 있으면 false를 반환하고 없어서 삽입에 성공하면 true를 반환합니다.
	bool	 Insert(const KeyT & key, const T & el);
	bool	 Insert(const Pair & pair) { return Insert(pair.key, *pair.el_ptr); }

	// 원소를 지웁니다.
	// 지운 원소 다음 iterator를 반환합니다.
	// 원소를 지우면 기존 iterator가 불능이 되므로 반환한 iterator로 갱신하십시오.
	Iter	 Erase(const Iter & iter) 
	{
		_el_list.erase(iter._it->_iter);
		return _pair_vec.erase(iter._it);
	}

	void	 Erase(const KeyT & key)
	{
		Iter iter;
		if(Find(key, &iter)) Erase(iter);
	}
	
	// 특정 원소를 찾습니다.
	// 못찾으면 false를 반환합니다.
	// 찾으면 true를 반환하고 인자 it_ptr에 찾은 iterator를 대입합니다.
	bool	 Find(const KeyT & key, Iter * it_ptr = nullptr);

	void	 Clear() { _pair_vec.clear(); _el_list.clear(); }
	bool	 CheckEmpty() const { return _pair_vec.empty(); }

	Iter	 Begin() { return Iter(_pair_vec.begin()); }
	Iter	 End() { return Iter(_pair_vec.end()); }

	T	&	 operator [] (const KeyT & key);

	size_t	 Size() const { return _pair_vec.size(); }

private:
	pair_vec_t	_pair_vec;
	el_list_t	_el_list;
};

template <typename KeyT, typename T>
bool SMap<KeyT, T>::Insert(const KeyT & key, const T & el)
{
	pair_vec_t::iterator it = std::lower_bound(_pair_vec.begin(), _pair_vec.end(), key);
	if(it != _pair_vec.end() && it->_key == key) return false;

	_el_list.push_front(el);
	_pair_vec.insert(it, Pair(key, _el_list.begin()));
	return true;
}

template <typename KeyT, typename T>
bool SMap<KeyT, T>::Find(const KeyT & key, Iter * it_ptr)
{
	if(_pair_vec.empty()) return false;
	pair_vec_t::iterator it = lower_bound(_pair_vec.begin(), _pair_vec.end(), key);
	if(it == _pair_vec.end() || it != _pair_vec.end() && it->_key != key) return false;
	if(it_ptr != nullptr) it_ptr->_it = it;
	return true;
}

template <typename KeyT, typename T>
T & SMap<KeyT, T>::operator [] (const KeyT & key)
{
	Iter iter;
	if(Find(key, &iter)) return (*iter).GetEl();
	else
	{
		// 못 찾았으면 하나 만든다.
		// T 타입은 기본 생성자가 있어야함
		Insert(key, T());
		return _el_list.front();
	}
}