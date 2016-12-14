//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <vector>
#include <mutex>

//------------------------------------------------
// A lockable vector container that controls 
// element access.
// 
// NOTE: This vector isn't recursive safe. Do not
// call safe functions while safe_lock retains
// ownership.
//
// This will cause an assertation.
//------------------------------------------------
template <typename T>
class LockableVector
{
public:
	using lock = std::lock_guard<std::recursive_mutex>;
	using my_vec = std::vector<T>;

	typedef typename std::vector<T>::size_type size_type;

	std::recursive_mutex& mutex();

	//-----------------------------------
	// Locks the vector.
	//-----------------------------------
	void safe_lock();

	//-----------------------------------
	// Unlocks the vector.
	//-----------------------------------
	void safe_unlock();

	//-----------------------------------
	// Gets the underlying contents.
	//-----------------------------------
	my_vec* get();

	//-----------------------------------
	// Pushes the item onto the back of 
	// the list, safely.
	//-----------------------------------
	void push_back(T v);

	//-----------------------------------
	// Gets the back element of the list,
	// safely.
	//-----------------------------------
	T back();

	//-----------------------------------
	// Pops the last item of the list, 
	// safely.
	//-----------------------------------
	void pop_back(T v);

	//-----------------------------------
	// Copies the value at the specified
	// index, safely.
	//-----------------------------------
	T at(size_type index);

	//-----------------------------------
	// Clears the vector, safely.
	//-----------------------------------
	void clear();

	//-----------------------------------
	// Copies vector contents by locking
	// underlying data structure.
	//-----------------------------------
	my_vec get_copy();

private:
	my_vec                m_contents;
	std::recursive_mutex  m_mutex;

};

template <typename T>
inline void LockableVector<T>::clear()
{
	lock guard(m_mutex);
	m_contents.clear();
}

template <typename T>
inline std::recursive_mutex& LockableVector<T>::mutex()
{
	return m_mutex;
}

template <typename T>
inline void LockableVector<T>::safe_lock()
{
	m_mutex.lock();
}

template <typename T>
inline void LockableVector<T>::safe_unlock()
{
	m_mutex.unlock();
}

template <typename T>
inline typename LockableVector<T>::my_vec* LockableVector<T>::get()
{
	return &m_contents;
}

template <typename T>
inline void LockableVector<T>::push_back(T v)
{
	lock guard(m_mutex);
	m_contents.push_back(v);
}

template <typename T>
inline T LockableVector<T>::back()
{
	lock guard(m_mutex);
	return m_contents.back();
}

template <typename T>
inline void LockableVector<T>::pop_back(T v)
{
	lock guard(m_mutex);
	m_contents.pop_back();
}

template <typename T>
inline T LockableVector<T>::at(size_type index)
{
	lock guard(m_mutex);
	return m_contents.at(index);
}

//-----------------------------------
// Copies vector contents by locking
// underlying data structure.
//-----------------------------------
template <typename T>
inline typename LockableVector<T>::my_vec LockableVector<T>::get_copy()
{
	lock guard(m_mutex);
	return m_contents;
}