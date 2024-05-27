//
// enumerator.h
//
// Copyright (c) 2024 Izadori
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#ifndef __IZADORI_ENUMERATOR_H__
#define __IZADORI_ENUMERATOR_H__

#include <tuple>
#include <utility>

#include "zipper.h"

//-----------------------------------------------------------------------------------------
// Python風のenumerator()関数の実装（C++17対応のコンパイラが必要）
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// Enumeratorコンテナクラス
//-----------------------------------------------------------------------------------------
template <class Container>
class Enumerator final
{
public:
	Enumerator() = delete;
	Enumerator(Container & container, size_t initial_index = 0, size_t step = 1)
		: ref_(container), initial_index_(initial_index), step_(step) {}

	class Iterator final
	{
	public:
		Iterator() : index_(0), step_(1){};

		bool operator==(const Iterator & it) const
		{
			return this->iter_ == it.iter_;
		}

		bool operator!=(const Iterator & it) const
		{
			return this->iter_ != it.iter_;
		}

		Iterator & operator++()
		{
			index_ += step_;
			++iter_;
			return *this;
		}

		std::tuple<size_t, typename Container::reference> operator*()
		{
			return {index_, *iter_};
		}

	private:
		size_t index_;
		size_t step_;
		typename Container::iterator iter_;

		static Iterator Begin(Container & container, size_t initial_index, size_t step)
		{
			Iterator it;
			it.iter_ = std::begin(container);
			it.index_ = initial_index;
			it.step_ = step;
			return it;
		}

		static Iterator End(Container & container, size_t step)
		{
			Iterator it;
			it.iter_ = std::end(container);
			it.index_ = container.size();
			it.step_ = step;
			return it;
		}

		friend Enumerator;
	};

	using iterator = Iterator;

	Iterator begin()
	{
		return Iterator::Begin(ref_, initial_index_, step_);
	}

	Iterator end()
	{
		return Iterator::End(ref_, step_);
	}

private:
	Container & ref_;
	size_t initial_index_;
	size_t step_;
};

//-----------------------------------------------------------------------------------------
// Enumeratorクラス（Zipper<> 特殊化）
//-----------------------------------------------------------------------------------------
template <typename... Containers>
class Enumerator<Zipper<Containers...>> final
{
public:
	Enumerator() = delete;
	Enumerator(Zipper<Containers...> & zipper, size_t initial_index = 0, size_t step = 1)
		: zipper_(zipper), initial_index_(initial_index), step_(step) {}

	class Iterator final
	{
	public:
		Iterator() : index_(0), step_(1){};

		bool operator==(const Iterator & it) const
		{
			return this->iter_ == it.iter_;
		}

		bool operator!=(const Iterator & it) const
		{
			return this->iter_ != it.iter_;
		}

		Iterator & operator++()
		{
			index_ += step_;
			++iter_;
			return *this;
		}

		auto operator*()
		{
			return std::make_tuple(index_, *iter_);
		};

	private:
		size_t index_;
		size_t step_;
		typename Zipper<Containers...>::iterator iter_;

		static Iterator Begin(Zipper<Containers...> & zipper, size_t initial_index, size_t step)
		{
			Iterator it;
			it.iter_ = zipper.begin();
			it.index_ = initial_index;
			it.step_ = step;
			return it;
		}

		static Iterator End(Zipper<Containers...> & zipper, size_t step)
		{
			Iterator it;
			it.iter_ = zipper.end();
			it.index_ = zipper.size();
			it.step_ = step;
			return it;
		}

		friend Enumerator;
	};

	using iterator = Iterator;

	Iterator begin()
	{
		return Iterator::Begin(zipper_, initial_index_, step_);
	}

	Iterator end()
	{
		return Iterator::End(zipper_, step_);
	}

private:
	Zipper<Containers...> zipper_;
	size_t initial_index_;
	size_t step_;
};

//-----------------------------------------------------------------------------------------
// Enumernate()関数（右辺値参照版）
//-----------------------------------------------------------------------------------------
template <class Container>
Enumerator<Container> Enumerate(Container && container, size_t initial_index = 0, size_t step = 1)
{
	return Enumerator(container, initial_index, step);
}

//-----------------------------------------------------------------------------------------
// Enumernate()関数（左辺値参照版）
//-----------------------------------------------------------------------------------------
template <class Container>
Enumerator<Container> Enumerate(Container & container, size_t initial_index = 0, size_t step = 1)
{
	return Enumerator(container, initial_index, step);
}

#endif // __IZADORI_ENUMERATOR_H__
