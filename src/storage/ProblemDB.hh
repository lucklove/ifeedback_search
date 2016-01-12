#pragma once
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>
#include <vector>
#include "cppjieba/Jieba.hpp"

struct ProblemDB
{
    std::vector<int> getIds(const std::string& key) const
    {
        soci::session sql(soci::mysql, "db=db_ifeedback_search user=root pass=lucklove");
        soci::rowset<int> ids = (sql.prepare << "select id from tb_question_keyword where keyword=:KEY", soci::use(key));
        return std::vector<int>(ids.begin(), ids.end());
    }

    std::string getContent(int id) const
    {
        soci::session sql(soci::mysql, "db=db_ifeedback_search user=root pass=lucklove");
        std::string content;
        sql << "select content from tb_question where id=:ID", soci::into(content), soci::use(id);
        return content;
    }

    void set(int id, const std::string& content) const
    {
        soci::session sql(soci::mysql, "db=db_ifeedback_search user=root pass=lucklove");

        /** 保证冥等性 */
        sql << "delete from tb_question where id=:ID", soci::use(id);
        sql << "delete from tb_question_keyword where id=:ID", soci::use(id);

        std::vector<std::string> words;
        static const cppjieba::Jieba jieba("dict/jieba.dict.utf8", "dict/hmm_model.utf8", "");
        jieba.CutForSearch(content, words);
        for(const std::string& key : words)
            sql << "insert into tb_question_keyword(id, keyword) values(:ID, :KEY)", soci::use(id), soci::use(key);
        sql << "insert into tb_question(id, content) values(:ID, :CONTENT)", soci::use(id), soci::use(content);
    } 
};
