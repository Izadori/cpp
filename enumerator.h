/**
 * @file misc/enumerator.h
 * @brief Pythonのenumerate()風の動作をする関数の実装
 *
 * @note これらのクラス、関数を使用するためにはC++17以降に対応したコンパイラが必要です。
 */
#ifndef __CPP17_ENUMERATOR_H__
#define __CPP17_ENUMERATOR_H__

#include <tuple>
#include <utility>

#include "zipper.h"

//-----------------------------------------------------------------------------------------
// Python風のenumerator()関数の実装（C++17対応のコンパイラが必要）
//
// 警告なしを確認したコンパイラとコンパイラオプション
// gcc 13.2.0(on Linux)
//   % g++ -Wall -Wextra -O3 -std=c++17 -c
// MinGW gcc 13.2.0(on Windows 10)
//   > g++ -Wall -Wextra -O3 -std=gnu++17 -c
// Microsoft Visual C++ Build Tools 2022(on Windows 10)
//   > cl /c /W4 /EHsc /std:c++17
//-----------------------------------------------------------------------------------------

/**
 * @brief Enumeratorコンテナクラス
 *
 * @tparam Container コンテナの型
 */
template <class Container>
class Enumerator final
{
public:
	// デフォルトコンストラクタは削除
	Enumerator() = delete;

	/**
	 * @brief コンストラクタ
	 *
	 * @param[in] container     コンテナ
	 * @param[in] initial_index 添字の初期値
	 * @param[in] step          添字の増分
	 */
	Enumerator(Container & container, size_t initial_index = 0, size_t step = 1)
		: ref_(container), initial_index_(initial_index), step_(step) {}

	/**
	 * @brief Enumerator::Iteratorクラス
	 */
	class Iterator final
	{
	public:
		/**
		 * @brief コンストラクタ
		 */
		Iterator() : index_(0), step_(1){};

		/**
		 * @brief 比較演算子（等号）
		 */
		bool operator==(const Iterator & it) const
		{
			return this->iter_ == it.iter_;
		}

		/**
		 * @brief 比較演算子（不等号）
		 */
		bool operator!=(const Iterator & it) const
		{
			return this->iter_ != it.iter_;
		}

		/**
		 * @brief 前置インクリメント演算子
		 */
		Iterator & operator++()
		{
			index_ += step_;
			++iter_;
			return *this;
		}

		/**
		 * @brief 現在のイテレータの指し示す値の組をタプルで返す
		 *
		 * @return 現在のイテレータの指し示す値をまとめたタプル
		 */
		std::tuple<size_t, typename Container::reference> operator*()
		{
			return {index_, *iter_};
		}

	private:
		/**
		 * @brief 添字
		 */
		size_t index_;

		/**
		 * @brief 添字の増分
		 */
		size_t step_;

		/**
		 * @brief 実際のイテレータへの参照
		 */
		typename Container::iterator iter_;

		/**
		 * @brief 初期化して先頭イテレータを返す
		 *
		 * @param[in] container     コンテナ
		 * @param[in] initial_index 添字の初期値
		 * @param[in] step          添字の増分
		 * @return 先頭イテレータ
		 */
		static Iterator Begin(Container & container, size_t initial_index, size_t step)
		{
			Iterator it;
			it.iter_ = std::begin(container);
			it.index_ = initial_index;
			it.step_ = step;
			return it;
		}

		/**
		 * @brief 初期化して終端イテレータを返す
		 *
		 * @param[in] container     コンテナ
		 * @param[in] step          添字の増分
		 * @return 終端イテレータ
		 */
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

	/**
	 * @brief 各コンテナの先頭イテレータを返す
	 *
	 * @return 先頭イテレータ
	 */
	Iterator begin()
	{
		return Iterator::Begin(ref_, initial_index_, step_);
	}

	/**
	 * @brief 各コンテナの終端イテレータを返す
	 *
	 * @return 終端イテレータ
	 */
	Iterator end()
	{
		return Iterator::End(ref_, step_);
	}

private:
	/**
	 * @brief コンテナへの参照
	 */
	Container & ref_;

	/**
	 * @brief 添字の初期値
	 */
	size_t initial_index_;

	/**
	 * @brief 添字の増分
	 */
	size_t step_;
};

/**
 * @brief Enumeratorクラス（Zipper<>特殊化）
 *
 * @tparam Containers コンテナの型（可変引数テンプレート）
 */
template <typename... Containers>
class Enumerator<Zipper<Containers...>> final
{
public:
	// デフォルトコンストラクタは削除
	Enumerator() = delete;

	/**
	 * @brief コンストラクタ
	 *
	 * @param[in] container     コンテナ
	 * @param[in] initial_index 添字の初期値
	 * @param[in] step          添字の増分
	 */
	Enumerator(Zipper<Containers...> & zipper, size_t initial_index = 0, size_t step = 1)
		: zipper_(zipper), initial_index_(initial_index), step_(step) {}

	/**
	 * @brief Enumerator::Iteratorクラス
	 */
	class Iterator final
	{
	public:
		/**
		 * @brief コンストラクタ
		 */
		Iterator() : index_(0), step_(1){};

		/**
		 * @brief 比較演算子（等号）
		 */
		bool operator==(const Iterator & it) const
		{
			return this->iter_ == it.iter_;
		}

		/**
		 * @brief 比較演算子（不等号）
		 */
		bool operator!=(const Iterator & it) const
		{
			return this->iter_ != it.iter_;
		}

		/**
		 * @brief 前置インクリメント演算子
		 */
		Iterator & operator++()
		{
			index_ += step_;
			++iter_;
			return *this;
		}

		/**
		 * @brief 現在のイテレータの指し示す値の組をタプルで返す
		 *
		 * @return 現在のイテレータの指し示す値をまとめたタプル
		 */
		auto operator*()
		{
			return std::make_tuple(index_, *iter_);
		};

	private:
		/**
		 * @brief 添字
		 */
		size_t index_;

		/**
		 * @brief 添字の増分
		 */
		size_t step_;

		/**
		 * @brief Zipperクラスのイテレータへの参照
		 */
		typename Zipper<Containers...>::iterator iter_;

		/**
		 * @brief 初期化して先頭イテレータを返す
		 *
		 * @param[in] zipper        コンテナ
		 * @param[in] initial_index 添字の初期値
		 * @param[in] step          添字の増分
		 * @return 先頭イテレータ
		 */
		static Iterator Begin(Zipper<Containers...> & zipper, size_t initial_index, size_t step)
		{
			Iterator it;
			it.iter_ = zipper.begin();
			it.index_ = initial_index;
			it.step_ = step;
			return it;
		}

		/**
		 * @brief 初期化し終端イテレータを返す
		 *
		 * @param[in] zipper コンテナ
		 * @param[in] step   添字の増分
		 * @return 終端イテレータ
		 */
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

	/**
	 * @brief 各コンテナの先頭イテレータを返す
	 *
	 * @return Iterator
	 */
	Iterator begin()
	{
		return Iterator::Begin(zipper_, initial_index_, step_);
	}

	/**
	 * @brief 各コンテナの終端イテレータを返す
	 *
	 * @return Iterator
	 */
	Iterator end()
	{
		return Iterator::End(zipper_, step_);
	}

private:
	/**
	 * @brief Zipperコンテナのコピー
	 */
	Zipper<Containers...> & zipper_;

	/**
	 * @brief 添字の初期値
	 */
	size_t initial_index_;

	/**
	 * @brief 添字の増分
	 */
	size_t step_;
};

/**
 * @brief Pythonのenumerate()関数のように添字と要素をタプルとして返す（右辺値参照版）
 *
 * Pythonのenumerate()関数のように添字と要素をタプルとして返す
 *
 * @param[in] container     添字を付加するコンテナ
 * @param[in] initial_index 添字の初期値
 * @param[in] step          添字の増分
 * @return 生成されたEnumeratorコンテナ
 */
template <class Container>
Enumerator<Container> Enumerate(Container && container, size_t initial_index = 0, size_t step = 1)
{
	return Enumerator(container, initial_index, step);
}

/**
 * @brief Pythonのenumerate()関数のように添字と要素をタプルとして返す（左辺値参照版）
 *
 * Pythonのenumerate()関数のように添字と要素をタプルとして返す
 *
 * @param[in] container     添字を付加するコンテナ
 * @param[in] initial_index 添字の初期値
 * @param[in] step          添字の増分
 * @return 生成されたEnumeratorコンテナ
 */
template <class Container>
Enumerator<Container> Enumerate(Container & container, size_t initial_index = 0, size_t step = 1)
{
	return Enumerator(container, initial_index, step);
}

#endif // __CPP17_ENUMERATOR_H__
