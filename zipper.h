//
// zipper.h
//
// Copyright (c) 2024 Izadori
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#ifndef __IZADORI_ZIPPER_H__
#define __IZADORI_ZIPPER_H__

#include <algorithm>
#include <tuple>
#include <type_traits>
#include <utility>

//-----------------------------------------------------------------------------------------
// Python風のzip()関数の実装（C++17対応のコンパイラが必要）
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// GetIteratorImplクラス - コンテナの場合はイテレータを、配列の場合にはポインタを返す
//-----------------------------------------------------------------------------------------
template <typename T>
struct GetIteratorImpl {
	using iterator = decltype(std::begin(std::declval<T &>()));
	using reference = typename std::iterator_traits<iterator>::reference;
	using value_type = typename std::iterator_traits<iterator>::value_type;
	using pointer = typename std::iterator_traits<iterator>::pointer;
};

template <typename T>
using GetIterator = typename GetIteratorImpl<T>::iterator;

template <typename T>
using GetReference = typename GetIteratorImpl<T>::reference;

template <typename T>
using GetValueType = typename GetIteratorImpl<T>::value_type;

template <typename T>
using GetPointer = typename GetIteratorImpl<T>::pointer;

//-----------------------------------------------------------------------------------------
// Zipperコンテナクラス
//-----------------------------------------------------------------------------------------
template <class... Containers>
class Zipper final
{
public:
	Zipper() = delete;
	Zipper(Containers &... containers) : tpl_({containers...}) {}

	class EndIterator final
	{
	public:
		std::tuple<GetIterator<Containers>...> iter_;
	};

	class Iterator final
	{
	public:
		bool operator==(const Iterator & it) const
		{
			return this->iter_ == it.iter_;
		}

		bool operator==(const EndIterator & it) const
		{
			return IsEnd(it, std::make_index_sequence<sizeof...(Containers)>{});
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
			Increment(std::make_index_sequence<sizeof...(Containers)>{});
			return *this;
		}

		auto operator*()
		{
			auto iter_tmp = iter_;
			return std::apply(GetValue<GetIterator<Containers>...>, iter_tmp);
		}

	private:
		std::tuple<GetIterator<Containers>...> iter_;

		template <size_t... N>
		void Increment(std::index_sequence<N...>)
		{
			using swallow = std::initializer_list<int>;
			(void)swallow{(void(std::get<N>(iter_)++), 0)...};
		}

		template <typename ContainerIterator>
		static auto & GetValueHelper(ContainerIterator & it)
		{
			return *it;
		}

		template <typename... ContainerIterators>
		static std::tuple<GetReference<Containers>...> GetValue(ContainerIterators &... containers)
		{
			return {GetValueHelper<ContainerIterators>(containers)...};
		}

		static Iterator Begin(Containers &... containers)
		{
			Iterator it;
			it.iter_ = std::make_tuple(std::begin(containers)...);
			return it;
		}

		static EndIterator End(Containers &... containers)
		{
			EndIterator it;
			it.iter_ = std::make_tuple(std::end(containers)...);
			return it;
		}

		template <size_t... N>
		bool IsEnd(const EndIterator & it, std::index_sequence<N...>) const
		{
			bool chk[] = {(std::get<N>(iter_) == std::get<N>(it.iter_))...};

			return std::any_of(std::begin(chk), std::end(chk), [](bool x) { return x; });
		}

		friend Zipper;
	};

	using iterator = Iterator;
	using reference = std::tuple<Containers &...> &;
	using value_type = std::tuple<Containers &...>;

	Iterator begin()
	{
		return std::apply(Iterator::Begin, tpl_);
	}

	EndIterator end()
	{
		return std::apply(Iterator::End, tpl_);
	}

	size_t size()
	{
		return GetSize(std::make_index_sequence<std::tuple_size<decltype(tpl_)>::value>());
	}

private:
	std::tuple<Containers &...> tpl_;

	template <size_t... N>
	size_t GetSize(std::index_sequence<N...>)
	{
		return std::min({(std::distance(std::begin(std::get<N>(tpl_)), std::end(std::get<N>(tpl_))))...});
	}
};

//-----------------------------------------------------------------------------------------
// Zip関数
//-----------------------------------------------------------------------------------------
template <class... Containers>
auto Zip(Containers &... containers)
{
	return Zipper(containers...);
}

#endif // __IZADORI_ZIPPER_H__
