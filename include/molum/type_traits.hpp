#pragma once
#include <type_traits>

namespace molum
{
	template<class T, class = std::void_t<>>
	struct has_iterator : std::false_type { };

	template<class T>
	struct has_iterator<T,std::void_t<typename T::iterator>> : std::true_type { };

	template<class T>
	inline static constexpr bool has_iterator_v = has_iterator<std::remove_cvref_t<T>>::value;

	template<class T, class = std::void_t<>>
	struct has_push_back : std::false_type { };

	template<class T>
	struct has_push_back<T, std::void_t<decltype(std::declval<T>().push_back())>> : std::true_type { };

	template<class T>
	inline static constexpr bool has_pushback_v = has_push_back<T>::value;

	template<class T, class = std::void_t<>>
	struct container_element_type
	{
		using type = T;
	};

	template<class T>
	struct container_element_type<T,std::void_t<typename T::value_type>>
	{
		using type = typename T::value_type;
	};

	template<class T>
	using container_ele_t = typename container_element_type<T>::type;


	template<class T, class = std::void_t<>>
	struct is_container : std::false_type
	{
	};

	template<class T>
	struct is_container<T, std::void_t<typename T::iterator>> : std::true_type
	{
	};

	template<class T>
	static constexpr bool is_container_v = is_container<T>::value;
}