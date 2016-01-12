#include "UnitTest.hh"
#include "storage/DBConnectionPool.hh"

TEST_CASE(connection_pool)
{
    DBConnectionPool p{2, "db=db_ifeedback_search user=root pass=lucklove"};
    {
        auto p1 = p.get();
        TEST_REQUIRE(p1);
        int count = 0;
        p1->session << "select count(*) from tb_question", soci::into(count);
        TEST_CHECK(count != 0);        

        auto p2 = p.get();
        TEST_REQUIRE(p2);
        count = 0;
        p2->session << "select count(*) from tb_question", soci::into(count);
        TEST_CHECK(count != 0);        

        auto p3 = p.get();
        TEST_CHECK(!p3);
    }
    auto p4 = p.get();
    TEST_REQUIRE(p4);
    int count = 0;
    p4->session << "select count(*) from tb_question", soci::into(count);
    TEST_CHECK(count != 0);        
}
