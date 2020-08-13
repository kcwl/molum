#pragma once
#include <locale>
#include <vector>
#include <codecvt>
#include <string>
#include <sstream>
#include <aquarius/reflection.hpp>


namespace aquarius
{
	namespace detail
	{
		std::string to_gbk(std::wstring str)
		{
#ifdef _MSC_VER
			std::locale loc("zh-CN");
#else
			std::locale loc("zh_CN.GB18030");
#endif

			const wchar_t* from_next = nullptr;
			char* to_next = nullptr;

			std::mbstate_t state{};

			std::vector<char> buff(str.size() * 2);

			std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc).out(state, str.data(), str.data() + str.size(), from_next,
				buff.data(), buff.data() + buff.size(), to_next);

			return std::string(buff.data(), to_next);
		}

		std::wstring to_utf8(const std::string& str)
		{
			std::vector<wchar_t> buff(str.size());
#ifdef _MSC_VER
			std::locale loc("zh-CN");
#else
			std::locale loc("zh-CN.GB18030");
#endif
			wchar_t* to_next = nullptr;
			const char* to_from = nullptr;

			std::mbstate_t state{};

			std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc).in(state, str.data(), str.data() + str.size(), to_from, buff.data(), buff.data() + buff.size(), to_next);

			//return std::declval<std::wstring_convert<std::codecvt_utf8<wchar_t>>>().to_bytes(std::wstring(buff.data(), buff.size()));
			return std::wstring(buff.data(),buff.size());
		}

		template<class T>
		auto string_to(const std::string& val)
		{
			std::stringstream ss;
			ss << val;

			T t;
			ss >> t;

			return t;
		}


		template<class T, std::size_t N, std::size_t... I>
		auto construct_impl(std::array<std::string, N> val, std::index_sequence<I...>)
		{
			return T{string_to<typename reflect::tuple_element_type<I,T>::type>(val[I])...};
		}

		template<class T, std::size_t N>
		auto construct(std::array<std::string, N> val) ->T
		{
			return construct_impl<T>(val, std::make_index_sequence<reflect::tuple_size<T>::value>{});
		}
	}

}