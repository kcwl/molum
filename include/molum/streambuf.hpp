#pragma once
#include <string>
#include <algorithm>
#include "reflect.hpp"


namespace molum
{
	template<class T, class Alloc>
	class streambuf
	{
	public:
		using alloc_t = Alloc;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using const_pointer = const T*;
		using const_reference = const T&;
		using iterator = typename std::basic_string<T, std::char_traits<T>, alloc_t>::iterator;
		using const_iterator = const iterator;
		using size_type = std::size_t;

		inline constexpr static size_type buffer_capacity = 4 * 1024;

	public:
		streambuf()
			: wpos_(0)
			, rpos_(0)
			, buffer_(buffer_capacity, T{})
		{
		}

		streambuf(size_type size)
			: buffer_(size, T{})
			, wpos_()
			, rpos_()
		{
		}

		streambuf(const char* val, size_type length)
			: wpos_(0)
			, rpos_(0)
		{
			std::for_each(std::begin(val), std::end(val), [this](auto iter)
				{
					buffer_.push_back(static_cast<T>(iter));
				});
		}

		virtual ~streambuf() = default;

	public:
		iterator begin() noexcept
		{
			return buffer_.begin();
		}

		iterator end() noexcept
		{
			return buffer_.end();
		}

		reference at(const size_type pos)
		{
			return buffer_.at(pos);
		}

		size_type size() noexcept
		{
			return wpos_;
		}

		iterator tellr() noexcept
		{
			auto iter = begin();
			std::advance(iter, rpos_);

			return iter;
		}

		iterator tellw() noexcept
		{
			auto iter = begin();
			std::advance(iter, wpos_);

			return iter;
		}

		value_type* data() noexcept
		{
			return buffer_.data();
		}

		std::string str() noexcept
		{
			return buffer_;
		}

	public:
		template<class _Ty>
		constexpr auto write(_Ty data)
		{
			if(rpos_ == wpos_)
			{
				std::fill(begin(), end(), T{});

				rpos_ = 0;
				wpos_ = 0;
			}

			return write_impl(data);
		}

		template<class _Ty>
		constexpr auto read() -> _Ty
		{
			return read_impl<_Ty>();
		}

	private:
		template<class _Ty>
		constexpr auto write_impl(_Ty&& val, std::enable_if_t<!is_container_v<std::remove_reference_t<_Ty>>>* = 0)
		{
			constexpr auto N = reflect::template tuple_size<std::remove_reference_t<_Ty>>::value;

			return write_element<N>(std::forward<_Ty>(val));
		}

		template<class _Ty>
		constexpr auto write_impl(_Ty&& val, std::enable_if_t<is_container_v<std::remove_reference_t<_Ty>>>* = 0)
		{
			return write_element_to_buff(std::forward<_Ty>(val));
		}

		template<class _Ty>
		auto write_element_to_buff(_Ty&& val)
		{
			if constexpr(!has_iterator_v<_Ty>)
			{
				_Ty tmp = std::forward<_Ty>(val);
				std::memcpy(&buffer_[wpos_], reinterpret_cast<_Ty*>(&tmp), sizeof(_Ty));

				wpos_ += sizeof(_Ty);
			}
			else
			{
				int32_t size = static_cast<int32_t>(val.size());
				write_impl(size);
				std::for_each(val.begin(), val.end(), [this](auto iter)
					{
						write_impl(iter);
					});
			}
		}

		template<class _Ty, std::size_t... I>
		void write_element(_Ty&& val, std::index_sequence<I...>)
		{
			return (write_element_to_buff(reflect::tuple_element<I>(std::forward<_Ty>(val))), ...);
		}

		template<std::size_t N, class _Ty, class Indices = std::make_index_sequence<N>>
		void write_element(_Ty&& val)
		{
			return write_element(std::forward<_Ty>(val), Indices{});
		}

		template<class _Ty>
		constexpr auto read_impl(std::enable_if_t<!is_container_v<std::remove_cvref_t<_Ty>>>* = 0) -> _Ty
		{
			constexpr auto N = reflect::tuple_size<std::remove_reference_t<_Ty>>::value;

			return read_element<N, _Ty>();
		}

		template<class _Ty>
		constexpr auto read_impl(std::enable_if_t<is_container_v<std::remove_cvref_t<_Ty>>>* = 0) -> _Ty
		{
			_Ty element;

			std::size_t element_size;
			std::memcpy(&element_size, reinterpret_cast<std::size_t*>(&buffer_.at(rpos_)), sizeof(std::size_t));

			rpos_ += sizeof(std::size_t);

			for(std::size_t i = 0; i != element_size; ++i)
			{
				auto ele = read_impl<container_ele_t<_Ty>>();

				element.push_back(ele);
			}

			return element;
		}

		template<std::size_t N, class _Ty, class Indices = std::make_index_sequence<N>>
		constexpr auto read_element()
		{
			return read_element<_Ty>(Indices{});
		}

		template<class _Ty, std::size_t... I>
		constexpr auto read_element(std::index_sequence<I...>)
		{
			return _Ty{read_any_element<I,_Ty>()...};
		}

		template<std::size_t I, class _Ty>
		constexpr auto read_any_element()
		{
			auto element = reflect::tuple_element<I>(_Ty{});

			using element_type = decltype(element);

			if constexpr(!has_iterator_v<element_type>)
			{
				constexpr auto N = sizeof(element_type);

				std::memcpy(&element, reinterpret_cast<element_type*>(&buffer_.at(rpos_)), N);

				rpos_ += N;
			}
			else
			{
				std::size_t element_size;
				std::memcpy(&element_size, reinterpret_cast<std::size_t*>(&buffer_.at(rpos_)), sizeof(std::size_t));

				rpos_ += sizeof(std::size_t);

				for(std::size_t i = 0; i != element_size; ++i)
				{
					auto ele = read_impl<typename container_element_type<element_type>::type>();

					element.push_back(ele);
				}
			}

			return element;
		}

	private:
		std::basic_string<T, std::char_traits<T>, alloc_t> buffer_;

		int32_t wpos_;
		int32_t rpos_;
	};
}