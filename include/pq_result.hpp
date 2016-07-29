#pragma once

#include "pq_row.hpp"

namespace smartdb
{
	class pq_result
	{
	public:
		pq_result() : pgresult_(nullptr), conn_(nullptr), begin_(nullptr), end_(nullptr){}
		pq_result(PGresult *result, PGconn* conn, ExecStatusType status) : pgresult_(result), conn_(conn), status_(status), begin_(result), end_(result){}

		ExecStatusType status() const
		{
			return status_;
		}

		uint64_t count() const noexcept {
			assert(pgresult_);
			const char *count = PQcmdTuples(pgresult_);
			char *end;
			return std::strtoull(count, &end, 10);
		}

		class iterator {
		public:

			iterator(pq_result& result, pq_row *ptr) : result_(result), ptr_(ptr) {} /**< Constructor. **/
			iterator operator ++()         /**< Next row in the resultset **/
			{
				result_.next();

				if (result_.status_ != PGRES_SINGLE_TUPLE) {
					// We've reached the end
					assert(result_.status_ == PGRES_TUPLES_OK);
					ptr_ = &result_.end_;
				}
				return iterator(result_, ptr_);
			}

			bool operator != (const iterator &other) { return ptr_ != other.ptr_; }
			pq_row &operator *() { return *ptr_; }

		private:
			pq_row *ptr_;
			pq_result& result_;
		};

		iterator begin()
		{
			return{*this, &begin_ };
		}

		iterator end()
		{
			return{ *this, (status_ == PGRES_SINGLE_TUPLE) ? &end_ : &begin_ };
		}

		operator const PGresult *() const {
			return pgresult_;
		}

	private:
		friend class pq_row;
		friend class postgresql;

		PGresult *pgresult_;
		PGconn* conn_;
		ExecStatusType status_ = PGRES_EMPTY_QUERY;
	    pq_row begin_, end_;     /**< Virtual begin and end of the result. **/
		int num_=0;             /**< Current row number. */

		pq_result(const pq_result&) = delete;
		pq_result(const pq_result&&) = delete;
		pq_result& operator = (const pq_result&) = delete;
		pq_result& operator = (const pq_result&&) = delete;

		void next()
		{
			PQclear(pgresult_);
			pgresult_ = PQgetResult(conn_);
			assert(pgresult_);
			status_ = PQresultStatus(pgresult_);
			if (status_ == PGRES_BAD_RESPONSE || status_ == PGRES_FATAL_ERROR)
				throw std::exception();

			num_++;
		}
	};
}