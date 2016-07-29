#pragma once
#include <string>
#include <cassert>
#include <libpq-fe.h>
#include "pq_result.hpp"

namespace smartdb
{
	class postgresql final
	{
	public:
		postgresql() : conn_(nullptr)
		{

		}

		~postgresql()
		{
			close();
		}

		bool connect(const std::string& conn_str)
		{
			conn_ = PQconnectdb(conn_str.c_str());
			if (PQstatus(conn_) != CONNECTION_OK)
				return false;

			return true;
		}

		//because this mode is single row mode, so you can't get the total count, you can get count by sql.
		template<typename... Args>
		pq_result execute(const std::string& strsql, Args&&... args)
		{
			if (pgresult_)
			{
				clear();
			}				

			assert(conn_ != nullptr);

			int success = PQsendQuery(conn_, strsql.c_str());
			if (!success)
			{
				auto str = std::string(PQerrorMessage(conn_));
				std::cout << str << std::endl;
				throw std::exception();
			}
			
			// Switch to the single row mode to avoid loading the all result in memory.
			success = PQsetSingleRowMode(conn_);
			assert(success);
			pgresult_ = PQgetResult(conn_);
			auto status = PQresultStatus(pgresult_);
			return{ pgresult_, conn_, status };
		}

		void clear()
		{
			auto status = PQresultStatus(pgresult_);
			do {
				PQclear(pgresult_);
				pgresult_ = PQgetResult(conn_);
				if (pgresult_ == nullptr) {
					status = PGRES_EMPTY_QUERY;
				}
				else {
					status = PQresultStatus(pgresult_);
					switch (status) {
					case PGRES_COMMAND_OK:
						break;

					case PGRES_BAD_RESPONSE:
					case PGRES_FATAL_ERROR:
						throw std::exception();
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
			} while (status != PGRES_EMPTY_QUERY);
		}

		template<typename Tuple, typename... Args>
		pq_result execute(const std::string& strsql, const Tuple& tp)
		{
			return{};
		}

		bool prepare(const std::string& strsql)
		{
			return false;
		}

		//transaction: begin, end, commit, rollback
		//todo

		//range-based for
		//todo

		void close()
		{
			if (conn_ != nullptr)
			{
				PQfinish(conn_);
				conn_ = nullptr;
			}				
		}

	private:
		PGconn* conn_;
		PGresult* pgresult_ = nullptr;
	};
}