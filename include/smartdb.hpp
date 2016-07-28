#pragma once
#include <string>
#include <tuple>

namespace smartdb
{
	template<typename DBType>
	class smartdb
	{
	public:
		bool connect(const std::string& conn_str)
		{
			return false;
		}

		template<typename... Args>
		auto excecute(const std::string& strsql, Args&&... args)
		{
			return 0;
		}

		template<typename Tuple, typename... Args>
		auto excecute(const std::string& strsql, const Tuple& tp)
		{
			return 0;
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

		}

	private:
		DBType db_;
	};
}
