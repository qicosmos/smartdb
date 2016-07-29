#pragma once
#include <cassert>
#include "pq_type.h"

#ifdef __linux__
#include <endian.h>
#elif defined (__APPLE__)
#include <libkern/OSByteOrder.h>
#define be16toh(x) OSSwapBigToHostInt16(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#elif defined (_WINDOWS)||defined (WIN32)
#include <winsock2.h>
#define be16toh(x) ntohs(x)
#define be32toh(x) ntohl(x)
#define be64toh(x) ntohll(x)
#endif

namespace smartdb
{
	class pq_row
	{
	public:
		//test a field for a null value.
		bool is_null(int column) const
		{
			assert(result_ != nullptr);
			return PQgetisnull(result_, 0, column) == 1;
		}

		const char *column_name(int column) const
		{
			assert(result_ != nullptr);
			const char *res = PQfname(result_, column);
			assert(res);
			return res;
		}

		int column_index(const char *column_name) const
		{
			assert(result_ != nullptr);
			return PQfnumber(result_, column_name);
		}

		int column_num() const
		{
			assert(result_ != nullptr);
			return PQnfields(result_);
		}

		Oid column_type(int column) const
		{
			assert(result_ != nullptr);
			return PQftype(result_, column);
		}

		template<typename T>
		T get(const char *column_name) const
		{
			const int index = column_index(column_name);
			if (index == -1)
				return{};

			return get<T>(index);
		}

		template<typename T>
		T get(int column) const;

	private:
		friend class pq_result;
		friend class iterator;
		pq_row(PGresult * result) : result_(result) {}

		pq_row(const pq_row&) = delete;
		pq_row& operator = (const pq_row&) = delete;
		pq_row(const pq_row&&) = delete;
		pq_row& operator = (const pq_row&&) = delete;

		PGresult *result_;
	};

	template<>
	int8_t pq_row::get<int8_t>(int column) const
	{
		assert(result_ != nullptr);
		if (PQgetisnull(result_, 0, column))
			return 0;
		return *reinterpret_cast<int8_t*>(PQgetvalue(result_, 0, column));
	}

	template<>
	int16_t pq_row::get<int16_t>(int column) const
	{
		assert(result_ != nullptr);
		if (PQgetisnull(result_, 0, column))
			return 0;
		return be16toh(*reinterpret_cast<int16_t*>(PQgetvalue(result_, 0, column)));
	}

	template<>
	int32_t pq_row::get<int32_t>(int column) const
	{
		assert(result_ != nullptr);
		if (PQgetisnull(result_, 0, column))
			return 0;

		return atoi(PQgetvalue(result_, 0, column));
		//return be32toh(*reinterpret_cast<int32_t*>(PQgetvalue(result_, current_row_no_, column)));
	}

	template<>
	int64_t pq_row::get<int64_t>(int column) const
	{
		assert(result_ != nullptr);
		if (PQgetisnull(result_, 0, column))
			return 0;

		return *reinterpret_cast<int64_t*>(PQgetvalue(result_, 0, column));
	}

	template<>
	bool pq_row::get<bool>(int column) const
	{
		assert(result_ != nullptr);
		if (PQgetisnull(result_, 0, column))
			return false;

		return *reinterpret_cast<bool*>(PQgetvalue(result_, 0, column));
	}

	template<>
	float pq_row::get<float>(int column) const
	{
		assert(result_ != nullptr);
		if (PQgetisnull(result_, 0, column))
			return 0.f;

		return *reinterpret_cast<float*>(*reinterpret_cast<int32_t*>(PQgetvalue(result_, 0, column)));
	}

	template<>
	double pq_row::get<double>(int column) const
	{
		assert(result_ != nullptr);
		if (PQgetisnull(result_, 0, column))
			return 0.f;

		return *reinterpret_cast<double*>(be64toh(*reinterpret_cast<int64_t*>(PQgetvalue(result_, 0, column))));
	}

	template<>
	std::string pq_row::get<std::string>(int column) const
	{
		assert(result_ != nullptr);
		if (PQgetisnull(result_, 0, column))
			return{};

		int length = PQgetlength(result_, 0, column);
		return std::string(PQgetvalue(result_, 0, column), length);
	}

	template<>
	char pq_row::get<char>(int column) const
	{
		assert(result_ != nullptr);
		if (PQgetisnull(result_, 0, column))
			return '\0';

		return *PQgetvalue(result_, 0, column);
	}
}