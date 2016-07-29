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

		int column_num() const
		{
			assert(result_ != nullptr);
			return PQnfields(result_);
		}

		template<typename T>
		T get(int column) const;

		template<>
		int8_t get<int8_t>(int column) const
		{
			assert(result_ != nullptr);
			if (PQgetisnull(result_, current_row_no_, column))
				return 0;
			return *reinterpret_cast<int8_t*>(PQgetvalue(result_, current_row_no_, column));
		}

		template<>
		int16_t get<int16_t>(int column) const
		{
			assert(result_ != nullptr);
			if (PQgetisnull(result_, current_row_no_, column))
				return 0;
			return be16toh(*reinterpret_cast<int16_t*>(PQgetvalue(result_, current_row_no_, column)));
		}

		template<>
		int32_t get<int32_t>(int column) const
		{
			assert(result_ != nullptr);
			if (PQgetisnull(result_, current_row_no_, column))
				return 0;
			return be32toh(*reinterpret_cast<int32_t*>(PQgetvalue(result_, current_row_no_, column)));
		}

		template<>
		int64_t get<int64_t>(int column) const
		{
			assert(result_ != nullptr);
			if (PQgetisnull(result_, current_row_no_, column))
				return 0;

			return be64toh(*reinterpret_cast<int64_t*>(PQgetvalue(result_, current_row_no_, column)));
		}

		template<>
		bool get<bool>(int column) const
		{
			assert(result_ != nullptr);
			if (PQgetisnull(result_, current_row_no_, column))
				return false;

			return *reinterpret_cast<bool*>(PQgetvalue(result_, current_row_no_, column));
		}

		template<>
		float get<float>(int column) const
		{
			assert(result_ != nullptr);
			if (PQgetisnull(result_, current_row_no_, column))
				return 0.f;

			return *reinterpret_cast<float*>(be32toh(*reinterpret_cast<int32_t*>(PQgetvalue(result_, current_row_no_, column))));
		}

		template<>
		double get<double>(int column) const
		{
			assert(result_ != nullptr);
			if (PQgetisnull(result_, current_row_no_, column))
				return 0.f;

			return *reinterpret_cast<double*>(be64toh(*reinterpret_cast<int64_t*>(PQgetvalue(result_, current_row_no_, column))));
		}

		template<>
		std::string get<std::string>(int column) const
		{
			assert(result_ != nullptr);
			if (PQgetisnull(result_, current_row_no_, column))
				return{};

			int length = PQgetlength(result_, current_row_no_, column);
			return std::string(PQgetvalue(result_, current_row_no_, column), length);
		}

		template<>
		char get<char>(int column) const
		{
			assert(result_ != nullptr);
			if (PQgetisnull(result_, current_row_no_, column))
				return '\0';

			return *PQgetvalue(result_, current_row_no_, column);
		}

	private:
		friend class pq_result;

		pq_row(PGresult * result, int row_no) : result_(result), current_row_no_(row_no) {}

		pq_row(const pq_row&) = delete;
		pq_row& operator = (const pq_row&) = delete;
		pq_row(const pq_row&&) = delete;
		pq_row& operator = (const pq_row&&) = delete;

		PGresult *result_;
		int current_row_no_;
	};
}