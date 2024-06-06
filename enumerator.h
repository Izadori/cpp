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
	Enumerator(Container & container, int initial_index = 0, int step = 1)
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

		std::tuple<int, GetReference<Container>> operator*()
		{
			return {index_, *iter_};
		}

	private:
		int index_;
		int step_;
		GetIterator<Container> iter_;

		static Iterator Begin(Container & container, int initial_index, int step)
		{
			Iterator it;
			it.iter_ = std::begin(container);
			it.index_ = initial_index;
			it.step_ = step;
			return it;
		}

		static Iterator End(Container & container, int initial_index, int step)
		{
			Iterator it;
			it.iter_ = std::end(container);
			it.index_ = (int)(std::distance(std::begin(container), std::end(container)) - 1) * step + initial_index;
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
		return Iterator::End(ref_, initial_index_, step_);
	}

private:
	Container & ref_;
	int initial_index_;
	int step_;
};

//-----------------------------------------------------------------------------------------
// Enumeratorクラス（Zipper<> 特殊化）
//-----------------------------------------------------------------------------------------
template <typename... Containers>
class Enumerator<Zipper<Containers...>> final
{
public:
	Enumerator() = delete;
	Enumerator(Zipper<Containers...> & zipper, int initial_index = 0, int step = 1)
		: zipper_(zipper), initial_index_(initial_index), step_(step) {}

	class EndIterator final
	{
	public:
		int index_;
		int step_;
		typename Zipper<Containers...>::EndIterator iter_;
	};

	class Iterator final
	{
	public:
		Iterator() : index_(0), step_(1){};

		bool operator==(const Iterator & it) const
		{
			return this->iter_ == it.iter_;
		}

		bool operator==(const EndIterator & it) const
		{
			return this->iter_ == it.iter_;
		}

		bool operator!=(const Iterator & it) const
		{
			return this->iter_ != it.iter_;
		}

		bool operator!=(const EndIterator & it) const
		{
			return !(*this == it);
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
		int index_;
		int step_;
		typename Zipper<Containers...>::iterator iter_;

		static Iterator Begin(Zipper<Containers...> & zipper, int initial_index, int step)
		{
			Iterator it;
			it.iter_ = zipper.begin();
			it.index_ = initial_index;
			it.step_ = step;
			return it;
		}

		static EndIterator End(Zipper<Containers...> & zipper, int initial_index, int step)
		{
			EndIterator it;
			it.iter_ = zipper.end();
			it.index_ = (int)(zipper.size() - 1) * step + initial_index;
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

	EndIterator end()
	{
		return Iterator::End(zipper_, initial_index_, step_);
	}

private:
	Zipper<Containers...> zipper_;
	int initial_index_;
	int step_;
};

//-----------------------------------------------------------------------------------------
// Enumernate()関数
//-----------------------------------------------------------------------------------------
template <class Container>
Enumerator<Container> Enumerate(Container & container, int initial_index = 0, int step = 1)
{
	return Enumerator(container, initial_index, step);
}

//-----------------------------------------------------------------------------------------
// Enumernate()関数（Zipper<>版）
//-----------------------------------------------------------------------------------------
template <class... Containers>
Enumerator<Zipper<Containers...>> Enumerate(Zipper<Containers...> && zipper, int initial_index = 0, int step = 1)
{
	return Enumerator(zipper, initial_index, step);
}

#endif // __IZADORI_ENUMERATOR_H__
