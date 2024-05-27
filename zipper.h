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

#include <tuple>
#include <utility>

//-----------------------------------------------------------------------------------------
// Python風のzip()関数の実装（C++17対応のコンパイラが必要）
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// Zipperコンテナクラス
//-----------------------------------------------------------------------------------------
template <class... Containers>
class Zipper final
{
public:
	Zipper() = delete;
	Zipper(Containers &... containers) : tpl_({containers...}) {}

	class Iterator final
	{
	public:
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
			Increment(std::make_index_sequence<sizeof...(Containers)>{});
			return *this;
		}

		auto operator*()
		{
			auto iter_tmp = iter_;
			return std::apply(GetValue<iterator<Containers>...>, iter_tmp);
		}

	private:
		template <class T>
		using iterator = typename T::iterator;
		template <class T>
		using reference = typename T::reference;

		std::tuple<iterator<Containers>...> iter_;

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
		static std::tuple<reference<Containers>...> GetValue(ContainerIterators &... containers)
		{
			return {GetValueHelper<ContainerIterators>(containers)...};
		}

		static Iterator Begin(Containers &... containers)
		{
			Iterator it;
			it.iter_ = std::make_tuple(std::begin(containers)...);
			return it;
		}

		static Iterator End(Containers &... containers)
		{
			Iterator it;
			it.iter_ = std::make_tuple(std::end(containers)...);
			return it;
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

	Iterator end()
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
		return std::min({std::get<N>(tpl_).size()...});
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
