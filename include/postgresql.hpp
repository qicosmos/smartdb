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

		template<typename... Args>
		pq_result excecute(const std::string& strsql, Args&&... args)
		{
			assert(conn_ != nullptr);
//			result_.clear();
			PGresult* res = PQexec(conn_, strsql.c_str());
			if (PQresultStatus(res) != PGRES_TUPLES_OK)
			{
				return{};
			}

			//int success = PQsetSingleRowMode(conn_);
			//assert(success);
			//result_.first();

			return{ res, conn_ };
		}

		template<typename Tuple, typename... Args>
		pq_result excecute(const std::string& strsql, const Tuple& tp)
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
	};
}