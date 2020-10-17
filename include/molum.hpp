#pragma once
#include "molum/archive.hpp"


namespace molum
{
	using archive_stream = molum::archive<std::byte, std::allocator<std::byte>>;

	static archive_stream archive_;

	template<class T>
	auto serialize(T val)
	{
		archive_ << val;

		return archive_.data();
	}

	template<class R,class T>
	R deserialize(T* value)
	{
		archive_ << value;

		R result;
		archive_ >> result;

		return result;
	}

	template<class R,class T>
	void deserialize(T* value, R& result)
	{
		archive_ << value;

		archive_ >> result;
	}
}