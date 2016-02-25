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

    double get(const std::string& key, int subject_id = 0, int type_id = 0) const
    {
        auto connection = connection_pool_.get(true);
        int sum;
        int key_count;
        if(subject_id && type_id)
        {
            connection->session << "select count(*) from tb_question_keyword where subject=:SUB and type=:TYPE", 
                soci::into(sum), soci::use(subject_id), soci::use(type_id);
            connection->session << "select count(*) from tb_question_keyword where keyword=:KEY and subject=:SUB and type=:TYPE", 
                soci::into(key_count), soci::use(key), soci::use(subject_id), soci::use(type_id);
        }
        else if(subject_id)
        {
            connection->session << "select count(*) from tb_question_keyword where subject=:SUB", 
                soci::into(sum), soci::use(subject_id);
            connection->session << "select count(*) from tb_question_keyword where keyword=:KEY and subject=:SUB", 
                soci::into(key_count), soci::use(key), soci::use(subject_id);
        }
        else if(type_id)
        {
            connection->session << "select count(*) from tb_question_keyword where type=:TYPE", 
                soci::into(sum), soci::use(type_id);
            connection->session << "select count(*) from tb_question_keyword where keyword=:KEY and type=:TYPE", 
                soci::into(key_count), soci::use(key), soci::use(type_id);
        }
        else
        {
            connection->session << "select count(*) from tb_question_keyword", soci::into(sum);
            connection->session << "select count(*) from tb_question_keyword where keyword=:KEY", soci::into(key_count), soci::use(key);
        }

        return pow((sum - key_count) / static_cast<double>(sum), key_count);
    }
};
