#include <iostream>
#include <postgresql.hpp>

using namespace smartdb;

void test_pq_connect()
{
	postgresql pq;
	auto conninfo = "hostaddr=192.168.3.226  port=5432 dbname=exampledb user=dbuser password=12345";
	bool r = pq.connect(conninfo);
	std::cout << r << std::endl;

	auto& result = pq.excecute("select * from student");
	auto count = result.count();
}

int main()
{
	test_pq_connect();

	return 0;
}