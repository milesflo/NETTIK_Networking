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

	inline void safe_lock()
	{
		m_mutex.lock();
	}

	inline void safe_unlock()
	{
		m_mutex.unlock();
	}

	inline my_vec* get()
	{
		return &m_contents;
	}

	inline void push_back(T v)
	{
		lock(m_mutex);
		m_contents.push_back(v);
	}

	inline T back()
	{
		lock(m_mutex);
		return m_contents.back();
	}

	inline void pop_back(T v)
	{
		lock(m_mutex);
		m_contents.pop_back();
	}

	inline T at(size_type index)
	{
		lock(m_mutex);
		return m_contents.at(index);
	}
};
