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

    std::vector<int> getIds(const std::string& key, int subject_id = 0, int type_id = 0) const
    {
        auto connection = connection_pool_.get(true);
        if(subject_id && type_id)
        {
            soci::rowset<int> ids = (connection->session.prepare << 
                "select id from tb_question_keyword where keyword=:KEY and subject=:SUB and type=:TYPE", 
                soci::use(key), soci::use(subject_id), soci::use(type_id));
            return std::vector<int>(ids.begin(), ids.end());
        }
        else if(subject_id)
        {
            soci::rowset<int> ids = (connection->session.prepare << 
                "select id from tb_question_keyword where keyword=:KEY and subject=:SUB", 
                soci::use(key), soci::use(subject_id));
            return std::vector<int>(ids.begin(), ids.end());
        }
        else if(type_id)
        {
            soci::rowset<int> ids = (connection->session.prepare << 
                "select id from tb_question_keyword where keyword=:KEY and type=:TYPE", 
                soci::use(key), soci::use(type_id));
            return std::vector<int>(ids.begin(), ids.end());
        }
        else
        {
            soci::rowset<int> ids = (connection->session.prepare << 
                "select id from tb_question_keyword where keyword=:KEY", soci::use(key));
            return std::vector<int>(ids.begin(), ids.end());
        }
    }

    std::string getContent(int id) const
    {
        std::string content;
        auto connection = connection_pool_.get(true);
        connection->session << "select content from tb_question where id=:ID", soci::into(content), soci::use(id);
        return content;
    }

    void set(int id, const std::string& content, int subject_id = 0, int type_id = 0) const
    {
        assert(type_id);
        auto connection = connection_pool_.get(true);

        /** 保证冥等性 */
        connection->session << "delete from tb_question where id=:ID", soci::use(id);
        connection->session << "delete from tb_question_keyword where id=:ID", soci::use(id);

        std::vector<std::string> words;
        static const cppjieba::Jieba jieba("dict/jieba.dict.utf8", "dict/hmm_model.utf8", "");
        jieba.CutForSearch(content, words);
        for(const std::string& key : words)
        {
            connection->session 
                << "insert into tb_question_keyword(id, keyword, subject, type) values(:ID, :KEY, :SUB, :TYPE)", 
                soci::use(id), soci::use(key), soci::use(subject_id), soci::use(type_id);
        }
        connection->session 
            << "insert into tb_question(id, content, subject, type) values(:ID, :CONTENT, :SUB, :TYPE)", 
            soci::use(id), soci::use(content), soci::use(subject_id), soci::use(type_id);
    } 

    void del(int id)
    {
        auto connection = connection_pool_.get(true);

        connection->session << "delete from tb_question where id=:ID", soci::use(id);
        connection->session << "delete from tb_question_keyword where id=:ID", soci::use(id);
    }
};
