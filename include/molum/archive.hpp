#pragma once
#include <memory>
#include <array>
#include "algorithm.hpp"
#include "type_traits.hpp"
#include "streambuf.hpp"


namespace molum
{
	template<class T, class Alloc>
	class archive
	{
	public:
		using alloc_t = Alloc;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using const_pointer = const T*;
		using const_reference = const T&;
		using iterator = typename std::vector<T>::iterator;
		using const_iterator = const iterator;
		using size_type = std::size_t;
		using streambuf_t = streambuf<T, alloc_t>;

		inline constexpr static std::size_t buffer_capacity = 4 * 100 * 1024;

	public:
		archive()
			: buffer_ptr_(std::make_unique<streambuf_t>(buffer_capacity))
		{
		}

		archive(std::size_t size)
			: buffer_ptr_(std::make_unique<streambuf_t>(size))
		{
		}

		archive(const char* buf, size_type length)
			: buffer_ptr_(std::make_unique<streambuf_t>(buf, length))
		{
		}

		virtual ~archive() = default;

	public:
		iterator begin() noexcept
		{
			return buffer_ptr_->begin();
		}

		iterator end() noexcept
		{
			return buffer_ptr_->end();
		}

		std::size_t size() noexcept
		{
			return buffer_ptr_->size();
		}

		iterator tellr() noexcept
		{
			return buffer_ptr_->tellr();
		}

		iterator tellw() noexcept
		{
			return buffer_ptr_->tellw();
		}

		template<class _Ty>
		archive& operator <<(const _Ty& val)
		{
			buffer_ptr_->write(val);

			return *this;
		}

		template<class _Ty/*, class = std::enable_if_t<std::is_aggregate_v<_Ty>>*/>
		archive& operator >>(_Ty& val)
		{
			val = buffer_ptr_->read<_Ty>();

			return *this;
		}

		pointer data()
		{
			return buffer_ptr_->data();
		}

	private:
		std::unique_ptr<streambuf_t> buffer_ptr_;
	};
}

template <class _Traits>
std::basic_ostream<char, _Traits>& operator<<(std::basic_ostream<char, _Traits>& os, std::byte val)
{
	return os << static_cast<char>(val);
}