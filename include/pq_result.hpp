#pragma once
#include "pq_row.hpp"

namespace smartdb
{
	template<typename T>
	class iterator {
	public:
		iterator(T *ptr) : ptr_(ptr) {}
		iterator operator ++()         /**< Next row in the resultset **/
		{
			ptr_->result_.next();
			if (ptr_->result_.status_ != PGRES_SINGLE_TUPLE) {
				// We've reached the end
				assert(ptr_->result_.status_ == PGRES_TUPLES_OK);
//				ptr_ = &ptr_->result_.end_;
			}
			return iterator(ptr_);
		}
		bool operator != (const iterator &other) { return ptr_ != other.ptr_; }
		T &operator *() { return *ptr_; }

	private:
		T *ptr_;
	};

	class pq_row;
	class pq_result
	{
	public:
		pq_result() : pgresult_(nullptr), conn_(nullptr){}
		pq_result(PGresult *result, PGconn* conn) : pgresult_(result), conn_(conn){}

		uint64_t count() const noexcept
		{
			assert(pgresult_);
			const char *count = PQcmdTuples(pgresult_);
			char *end;
			return std::strtoull(count, &end, 10);
		}

		iterator<pq_row> begin()
		{
			return{nullptr};
		}

		iterator<pq_row> end()
		{
			return{ nullptr };
		}

		void first() {
			assert(pgresult_ == nullptr);
			num_ = 0;
			next();
		}

		void clear()
		{
			switch (status_) {
			case PGRES_COMMAND_OK:
				do {
					PQclear(pgresult_);
					pgresult_ = PQgetResult(conn_);
					if (pgresult_ == nullptr) {
						status_ = PGRES_EMPTY_QUERY;
					}
					else {
						status_ = PQresultStatus(pgresult_);
						switch (status_) {
						case PGRES_COMMAND_OK:
							break;

						case PGRES_BAD_RESPONSE:
						case PGRES_FATAL_ERROR:
							//throw ExecutionException(conn_.lastError());
							break;

						case PGRES_SINGLE_TUPLE:
						case PGRES_TUPLES_OK:
							// It is not supported to execute a multi statement sql
							// without fetching the result with the result iterator.
							assert(true);
							break;

						default:
							assert(true);
							break;
						}
					}
				} while (status_ != PGRES_EMPTY_QUERY);
				break;

			case PGRES_BAD_RESPONSE:
			case PGRES_FATAL_ERROR:
			case PGRES_TUPLES_OK:
				PQclear(pgresult_);
				pgresult_ = PQgetResult(conn_);
				assert(pgresult_ == nullptr);
				status_ = PGRES_EMPTY_QUERY;
				break;

			case PGRES_SINGLE_TUPLE:
				next();
				if (status_ == PGRES_SINGLE_TUPLE) {
					// All results of the previous query have not been processed, we
					// need to cancel it.
					char errbuf[256];
					PGcancel *pgcancel = PQgetCancel(conn_);

					int success = PQcancel(pgcancel, errbuf, sizeof(errbuf));
					if (!success) {
						//throw ExecutionException(errbuf);
					}

					PQfreeCancel(pgcancel);
				}
				else if (status_ == PGRES_TUPLES_OK) {
					PQclear(pgresult_);
					pgresult_ = PQgetResult(conn_);
					status_ = PGRES_EMPTY_QUERY;
				}
				assert(pgresult_ == nullptr);
				break;

			case PGRES_EMPTY_QUERY:
				break;

			default:
				assert(true);
			}
		}

	private:
		friend class pq_row;
		PGresult *pgresult_;
		PGconn* conn_;
		ExecStatusType status_ = PGRES_EMPTY_QUERY;
	//	pq_row begin_, end_;     /**< Virtual begin and end of the result. **/
		int num_;             /**< Current row number. */

		pq_result(const pq_result&) = delete;
		pq_result(const pq_result&&) = delete;
		pq_result& operator = (const pq_result&) = delete;
		pq_result& operator = (const pq_result&&) = delete;

		void next()
		{
			if (pgresult_) {
				assert(status_ == PGRES_SINGLE_TUPLE);
				PQclear(pgresult_);
			}

			pgresult_ = PQgetResult(conn_);
			assert(pgresult_);
			status_ = PQresultStatus(pgresult_);
			switch (status_) {
			case PGRES_SINGLE_TUPLE:
				assert(PQntuples(pgresult_) == 1);
				num_++;
				break;

			case PGRES_TUPLES_OK:
				// The SELECT statement did not return any row or after the last row,
				// a zero-row object with status PGRES_TUPLES_OK is returned; this is
				// the signal that no more rows are expected.
				assert(PQntuples(pgresult_) == 0);
				break;

			case PGRES_BAD_RESPONSE:
			case PGRES_FATAL_ERROR:
				//throw ExecutionException(conn_.lastError());
				break;

			case PGRES_COMMAND_OK:
				break;

			default:
				assert(false);
				break;
			}
		}
	};
}