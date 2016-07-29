#pragma once

#include "pq_row.hpp"

namespace smartdb
{
//	template<typename T>
//	class iterator {
//	public:
//		iterator(T *ptr) : ptr_(ptr) {}
//		iterator operator ++()         /**< Next row in the resultset **/
//		{
//			ptr_->result_.next();
//			if (ptr_->result_.status_ != PGRES_SINGLE_TUPLE) {
//				// We've reached the end
//				assert(ptr_->result_.status_ == PGRES_TUPLES_OK);
////				ptr_ = &ptr_->result_.end_;
//			}
//			return iterator(ptr_);
//		}
//		bool operator != (const iterator &other) { return ptr_ != other.ptr_; }
//		T &operator *() { return *ptr_; }
//
//	private:
//		T *ptr_;
//	};

	class pq_result
	{
	public:
		pq_result() : pgresult_(nullptr), conn_(nullptr), begin_(nullptr, 0){}
		pq_result(PGresult *result, PGconn* conn) : pgresult_(result), conn_(conn), begin_(result, 0){}

		uint64_t count() const noexcept
		{
			assert(pgresult_);
			const char *count = PQcmdTuples(pgresult_);
			char *end;
			return std::strtoull(count, &end, 10);
		}

		//iterator<pq_row> begin()
		//{
		//	return{nullptr};
		//}

		//iterator<pq_row> end()
		//{
		//	return{ nullptr };
		//}

	private:
		friend class pq_row;
		PGresult *pgresult_;
		PGconn* conn_;
		ExecStatusType status_ = PGRES_EMPTY_QUERY;
		pq_row begin_;
	//	pq_row begin_, end_;     /**< Virtual begin and end of the result. **/
		int num_;             /**< Current row number. */

		pq_result(const pq_result&) = delete;
		pq_result(const pq_result&&) = delete;
		pq_result& operator = (const pq_result&) = delete;
		pq_result& operator = (const pq_result&&) = delete;

	};
}