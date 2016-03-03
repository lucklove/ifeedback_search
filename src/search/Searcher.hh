#pragma once
#include <vector>
#include "cppjieba/Jieba.hpp"
#include "../utils/WikiSort.hh"

template <typename ProblemStorageT, typename WeightStorageT>
class Searcher
{
public:
    Searcher(ProblemStorageT& p_storage, WeightStorageT& w_storage) 
        : problem_storage_{p_storage}, weight_storage_{w_storage}
    {
    }

    auto search(const std::string& s, int subject_id = 0, int type_id = 0, size_t max_num = 30) const
    {
        std::vector<std::string> words;
        static const cppjieba::Jieba jieba("dict/jieba.dict.utf8", "dict/hmm_model.utf8", "");
        jieba.CutForSearch(s, words);

        std::unordered_map<std::string, double> words_map = weight_storage_.get(words, subject_id, type_id);

        std::vector<std::string> keys;
        for(const auto& p : words_map)
            keys.push_back(p.first);
        
        std::unordered_map<int, std::pair<std::string, double>> matched_problems = problem_storage_.get(keys, subject_id, type_id);

        /** 按匹配度排序 */
        std::vector<std::pair<int, std::pair<std::string, double>>> result_problems(matched_problems.begin(), matched_problems.end());
        for(auto& p : result_problems)
        {
            assert(p.first > 0);
            p.second.second = 0;
            for(const std::string& word : words)
            {
                if(p.second.first.find(word) != std::string::npos)
                    p.second.second += words_map[word];
            }
        }

        Wiki::Sort(result_problems.begin(), result_problems.end(), [](const auto& l, const auto& r)
        {
            assert(l.first > 0);
            assert(r.first > 0);
            return l.second.second >= r.second.second;
        });

        return result_problems;
    }

private:
    ProblemStorageT& problem_storage_;
    WeightStorageT& weight_storage_;
};

template <typename ProblemStorageT, typename WeightStorageT>
auto make_searcher(ProblemStorageT& ps, WeightStorageT& ws)
{
    return Searcher<ProblemStorageT, WeightStorageT>(ps, ws);
}
