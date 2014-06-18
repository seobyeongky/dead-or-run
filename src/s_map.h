/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	s_map.h

	SMap template container class�� header file�Դϴ�.

	SMap�� Static Map�� �ظ��Դϴ�.

	���� :

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
/*	���� �迭(associative array) container�Դϴ�.
	STL�� map�� red-black tree�� ��������� �� class�� �ٸ� ����ε�
	���� �뷮, �幮 ����, ������ �� ������ sorted vector����Դϴ�.
	�˻� ������尡 ��������� �۴ٰ� �� �� �ְڽ��ϴ�.
	��, KeyT Ÿ���� < �����ڰ� �����Ǿ��־���մϴ�. */

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

	// ���Ҹ� �����մϴ�.
	// key�� �̹� ������ false�� ��ȯ�ϰ� ��� ���Կ� �����ϸ� true�� ��ȯ�մϴ�.
	bool	 Insert(const KeyT & key, const T & el);
	bool	 Insert(const Pair & pair) { return Insert(pair.key, *pair.el_ptr); }

	// ���Ҹ� ����ϴ�.
	// ���� ���� ���� iterator�� ��ȯ�մϴ�.
	// ���Ҹ� ����� ���� iterator�� �Ҵ��� �ǹǷ� ��ȯ�� iterator�� �����Ͻʽÿ�.
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
	
	// Ư�� ���Ҹ� ã���ϴ�.
	// ��ã���� false�� ��ȯ�մϴ�.
	// ã���� true�� ��ȯ�ϰ� ���� it_ptr�� ã�� iterator�� �����մϴ�.
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
		// �� ã������ �ϳ� �����.
		// T Ÿ���� �⺻ �����ڰ� �־����
		Insert(key, T());
		return _el_list.front();
	}
}