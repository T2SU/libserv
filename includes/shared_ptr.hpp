/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_ptr.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smun <smun@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/28 20:11:55 by smun              #+#    #+#             */
/*   Updated: 2022/03/28 20:56:04 by smun             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP

#include <cstddef>
#include "log.hpp"

template<typename T>
class SharedPtr
{
private:
	T*		_ptr;
	int*	_ref;

public:
	SharedPtr()
		: _ptr(NULL)
		, _ref(NULL)
	{}

	~SharedPtr()
	{
		if (_ref == NULL)
			return;
		if (--(*_ref) == 0)
		{
			Log::V("SharedPtr remaining ref count=0. initiated deletion");
			delete _ref;
			delete _ptr;
		}
		else
			Log::V("SharedPtr decreased ref count. remaining ref count=%d. ", *_ref);
	}

	SharedPtr(T* ptr)
		: _ptr(ptr)
		, _ref(new int(1))
	{
		Log::V("SharedPtr Initiated. ref count=1");
	}

	SharedPtr(const SharedPtr& s)
		: _ptr(s._ptr)
		, _ref(s._ref)
	{
		if (s._ref == NULL)
			return;
		++(*_ref);
		Log::V("SharedPtr copied. remaining ref count=%d", *_ref);
	}

	SharedPtr& operator= (const SharedPtr& s)
	{
		if (this == &s)
			return *this;
		this->~SharedPtr();
		_ptr = s._ptr;
		_ref = s._ref;
		++(*_ref);
		Log::V("SharedPtr assigned. remaining ref count=%d", *_ref);
		return *this;
	}

	T*	Load()
	{
		return _ptr;
	}
};

#endif
