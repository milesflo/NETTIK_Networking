//-------------------------------------------------
// NETTIK Networking
// Copyright (c) 2015 - 2016 Jak Brierley
//
// See attached license inside "LICENSE".
//-------------------------------------------------
#pragma once
#include <vector>
#include <mutex>

template <typename T>
class LockableVector
{
private:
	using my_vec = std::vector<T>;
	using lock = std::lock_guard<std::recursive_mutex>;

	my_vec m_contents;
	std::recursive_mutex m_mutex;

	typedef typename std::vector<T>::size_type size_type;

public:

	void safe_lock();

	void safe_unlock();

	my_vec* get();

	void push_back(T v);

	T back();

	void pop_back(T v);

	T at(size_type index);
};

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
	lock(m_mutex);
	m_contents.push_back(v);
}

template <typename T>
inline T LockableVector<T>::back()
{
	lock(m_mutex);
	return m_contents.back();
}

template <typename T>
inline void LockableVector<T>::pop_back(T v)
{
	lock(m_mutex);
	m_contents.pop_back();
}

template <typename T>
inline T LockableVector<T>::at(size_type index)
{
	lock(m_mutex);
	return m_contents.at(index);
}
