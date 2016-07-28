#pragma once
#include <cassert>
#include "pq_type.h"
#include "pq_result.hpp"

namespace smartdb
{
	class pq_result;
	class pq_row
	{
	public:
		//test a field for a null value.
		bool is_null(int column) const
		{
			assert(result_.pgresult_ != nullptr);
			return PQgetisnull(result_.pgresult_, 0, column) == 1;
		}

		const char *column_name(int column) const
		{
			assert(result_.pgresult_ != nullptr);
			const char *res = PQfname(result_.pgresult_, column);
			assert(res);
			return res;
		}

		int column_num() const
		{
			assert(result_.pgresult_ != nullptr);
			return PQnfields(result_.pgresult_);
		}

		template<typename T>
		T get(int column) const;

		template<>
		int get<int>(int column) const
		{
			return 0;
		}

	private:
		friend class pq_result;
		friend class iterator<pq_row>;
		pq_row(pq_result& result) : result_(result) {}

		pq_row(const pq_row&) = delete;
		pq_row& operator = (const pq_row&) = delete;
		pq_row(const pq_row&&) = delete;
		pq_row& operator = (const pq_row&&) = delete;

		pq_result &result_;
	};
}