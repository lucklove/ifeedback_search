#pragma once
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>
#include <vector>
#include <cmath>

struct WeightDB
{
    double get(const std::string& key) const
    {
        soci::session sql(soci::mysql, "db=db_ifeedback_search user=root pass=lucklove");
        int sum;
        int key_count;
        sql << "select count(*) from tb_question_keyword", soci::into(sum);
        sql << "select count(*) from tb_question_keyword where keyword=:KEY", soci::into(key_count), soci::use(key);
        return pow((sum - key_count) / static_cast<double>(sum), key_count);
    }
};
