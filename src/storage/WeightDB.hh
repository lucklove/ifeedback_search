#pragma once
#include <vector>
#include <cmath>
#include "DBConnectionPool.hh"

struct WeightDB
{
private:
    DBConnectionPool& connection_pool_;

public:
    WeightDB(DBConnectionPool& pool) : connection_pool_{pool} {}

    double get(const std::string& key) const
    {
        auto connection = connection_pool_.get(true);
        int sum;
        int key_count;
        connection->session << "select count(*) from tb_question_keyword", soci::into(sum);
        connection->session << "select count(*) from tb_question_keyword where keyword=:KEY", soci::into(key_count), soci::use(key);
        return pow((sum - key_count) / static_cast<double>(sum), key_count);
    }
};
