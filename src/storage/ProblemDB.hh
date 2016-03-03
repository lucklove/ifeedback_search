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

    std::unordered_map<int, std::pair<std::string, double>> get(const std::vector<std::string>& keys, int subject_id = 0, int type_id = 0) const
    {
        auto connection = connection_pool_.get(true);
        std::unordered_map<int, std::pair<std::string, double>> problems;
        std::stringstream ss;
        ss << "select id, content from tb_question where ";
        for(const std::string& key : keys)
            ss << "content like '%" << key << "%' or ";
        ss << "false";
        if(subject_id)
            ss << " and subject=" << subject_id;
        if(type_id)
            ss << " and type=" << type_id;
        soci::rowset<> rows = (connection->session.prepare << ss.str());
        for(const auto& row : rows)
        {
            assert(row.get<int>(0) > 0);
            problems[row.get<int>(0)] = {row.get<std::string>(1), 0};
        }

        return problems;
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
