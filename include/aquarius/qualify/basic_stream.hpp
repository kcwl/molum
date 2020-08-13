#pragma once
#include <memory>
#include <array>
#include "algorithm.hpp"
#include "type_traits.hpp"
#include "streambuf.hpp"

namespace aquarius
{
	namespace qualify
	{
		template<class T,class Alloc>
		class basic_stream
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
			using streambuf_t = detail::streambuf<T, alloc_t>;

			inline constexpr static std::size_t buffer_capacity = 4* 100 * 1024;

		public:
			basic_stream()
				: buffer_ptr_(std::make_unique<streambuf_t>(buffer_capacity)) { }

			basic_stream(std::size_t size)
				: buffer_ptr_(std::make_unique<streambuf_t>(size)) { }

			basic_stream(const char* buf, size_type length)
				: buffer_ptr_(std::make_unique<streambuf_t>(buf,length)) { }

			virtual ~basic_stream() = default;

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
			basic_stream& operator <<(const _Ty& val)
			{
				buffer_ptr_->write(val);

				return *this;
			}

			template<class _Ty/*, class = std::enable_if_t<std::is_aggregate_v<_Ty>>*/>
			basic_stream& operator >>(_Ty& val)
			{
				val = buffer_ptr_->read<_Ty>();

				return *this;
			}

			pointer data() 
			{
				return buffer_ptr_->data();
			}

			template<class T>
			std::string to_json_value(std::enable_if_t<!is_container_v<T>>* = 0)
			{
				std::wstringstream ss;

				if constexpr(std::is_class_v<std::remove_cvref_t<T>>)
				{
					ss << "{";

					constexpr std::size_t N = reflect::tuple_size<std::remove_cvref_t<T>>::value;

					auto f = [&](auto args)
					{
						ss << to_utf8(args) << ",";
					};

					read_string<T>(f, std::make_index_sequence<N>{});

					auto result = ss.str();
					if(result.size() > 1)
						result.erase(result.size() - 1);

					ss.clear();
					ss.str(L"");
					
					ss << result;

					ss << "}";
				}
				else 
				{
					auto tmp = read<T>();
					
					if constexpr(is_byte_v<std::remove_cvref_t<T>>)
						ss << static_cast<int>(tmp);
					else
						ss << tmp;
				}

				return detail::to_gbk(ss.str());
			}



			template<class T>
			std::string to_json_value(std::enable_if_t<is_container_v<T>>* = 0)
			{
				std::stringstream ss;

				if constexpr (!is_string<T>::value)
					ss << "[";

				using ele_type = container_ele_t<T>;

				auto count = read<std::size_t>();
				for(int i = 0; i < count; i++)
				{
					ss << to_json_value<ele_type>();

					if((i != count - 1) && !is_string<std::remove_cvref_t<T>>::value)
						ss << ",";
				}

				if constexpr(!is_string<T>::value)
					ss << "]";

				return ss.str();
			}

			template<class T>
			auto from_json_value(std::size_t count = 0, std::enable_if_t<!is_container_v<T>>* = 0) -> std::tuple<T, std::size_t>
			{
				count == 0 ? count = buffer_ptr_->read<std::size_t>() : 0;

				constexpr auto N = reflect::tuple_size<T>::value;

				std::string ele{};
				std::array<std::string, N> result{};

				int pos{};
				while(pos != N && count > 0)
				{
					auto c = buffer_ptr_->read<char>();
					
					switch(c)
					{
						case '{':
						case '[':
							break;
						case ']':
						case '}':
						case ',':
							{
								if(ele.empty())
									break;

								result[pos++] = ele;
								ele.clear();
							}
							break;
						default:
							ele.push_back(c);
							break;
					}

					count--;
				}

				return {detail::construct<T, N>(result),count};
			}

			template<class T>
			auto from_json_value(std::size_t count = 0, std::enable_if_t<is_container_v<T>>* = 0) ->T
			{
				count == 0 ? count = buffer_ptr_->read<std::size_t>() : 0;

				using ele_t = container_ele_t<T>;

				T result{};

				std::string ele{};

				while(count > 1)
				{
					auto [ele, length] = from_json_value<ele_t>(count);

					result.push_back(ele);

					count = length;
				}
				
				return result;
			}

		private:
			auto get_next_byte()
			{
				return buffer_ptr_->at(buffer_ptr_->size());
			}

			template<class T, class Func, std::size_t... I>
			auto read_string(Func&& f, std::index_sequence<I...>)
			{
				return (std::forward<Func>(f)(to_json_value<std::remove_cvref_t<typename reflect::tuple_element_type<I, T>::type>>()), ...);
			}

		private:
			std::unique_ptr<streambuf_t> buffer_ptr_;
		};
	}
}

template <class _Traits>
std::basic_ostream<char, _Traits>& operator<<(std::basic_ostream<char, _Traits>& os, std::byte val)
{ 
	return os << static_cast<char>(val);
}