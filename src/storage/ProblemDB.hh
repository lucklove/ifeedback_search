#pragma once
#include <vector>
#include "cppjieba/Jieba.hpp"
#include "DBConnectionPool.hh"

struct ProblemDB
{
private:
    DBConnectionPool& connection_pool_;

public:
    ProblemDB(DBConnectionPool& pool) : connection_pool_{pool} {}

    std::vector<int> getIds(const std::string& key) const
    {
        auto connection = connection_pool_.get(true);
        soci::rowset<int> ids = (connection->session.prepare << 
            "select id from tb_question_keyword where keyword=:KEY", soci::use(key));
        return std::vector<int>(ids.begin(), ids.end());
    }

    std::string getContent(int id) const
    {
        std::string content;
        auto connection = connection_pool_.get(true);
        connection->session << "select content from tb_question where id=:ID", soci::into(content), soci::use(id);
        return content;
    }

    void set(int id, const std::string& content) const
    {
        auto connection = connection_pool_.get(true);

        /** 保证冥等性 */
        connection->session << "delete from tb_question where id=:ID", soci::use(id);
        connection->session << "delete from tb_question_keyword where id=:ID", soci::use(id);

        std::vector<std::string> words;
        static const cppjieba::Jieba jieba("dict/jieba.dict.utf8", "dict/hmm_model.utf8", "");
        jieba.CutForSearch(content, words);
        for(const std::string& key : words)
            connection->session << "insert into tb_question_keyword(id, keyword) values(:ID, :KEY)", soci::use(id), soci::use(key);
        connection->session << "insert into tb_question(id, content) values(:ID, :CONTENT)", soci::use(id), soci::use(content);
    } 
};
