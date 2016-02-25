#pragma once
#include <vector>
#include "cppjieba/Jieba.hpp"

template <typename ProblemStorageT, typename WeightStorageT>
class Searcher
{
public:
    Searcher(ProblemStorageT& p_storage, WeightStorageT& w_storage) 
        : problem_storage_{p_storage}, weight_storage_{w_storage}
    {
    }

    auto search(const std::string& s, int subject_id = 0, int type_id = 0) const
    {
        std::vector<std::string> words;
        std::unordered_map<int, double> unordered_problems;
        static const cppjieba::Jieba jieba("dict/jieba.dict.utf8", "dict/hmm_model.utf8", "");
        jieba.CutForSearch(s, words);

        double sum_score = 0.0;                     /**< 用于之后的规一化 */
        for(const std::string& key : words)
        {
            sum_score += weight_storage_.get(key, subject_id, type_id);

            std::vector<int> matched_problems = problem_storage_.getIds(key, subject_id, type_id);
            for(int id : matched_problems)
                unordered_problems[id] = 0;
        }

        /** 按匹配度排序 */
        std::vector<std::pair<int, double>> result_problems(unordered_problems.begin(), unordered_problems.end());
        for(auto& p : result_problems)
        {
            std::string s = problem_storage_.getContent(p.first);
            std::vector<std::string> doc_words;
            jieba.CutForSearch(s, doc_words);
            p.second = match(doc_words.begin(), doc_words.begin(), doc_words.end(), words.begin(), words.end(), subject_id, type_id) / sum_score;
        }
        std::sort(result_problems.begin(), result_problems.end(), [](const auto& l, const auto& r)
        {
            return l.second > r.second;
        });

        return result_problems;
    }

private:
    template <typename IteratorT>
    double match(IteratorT doc_begin,
        IteratorT doc_middle, 
        IteratorT doc_end,
        IteratorT words_begin, 
        IteratorT words_end,
        int subject_id,
        int type_id) const
    {
        if(words_begin == words_end)
            return 0;

        IteratorT dest = std::find(doc_middle, doc_end, *words_begin);

        if(dest == doc_end)     /**< 若在期望的区间未找到 */
        {
            dest = std::find(doc_begin, doc_middle, *words_begin);

            if(dest == doc_middle)
            {
                return match(doc_begin, doc_middle, doc_end, ++words_begin, words_end, subject_id, type_id);
            }
            else 
            {
                return 0.5 * weight_storage_.get(*words_begin, subject_id, type_id)     /**< 0.5为关键词乱序的惩罚 */
                    + match(doc_begin, ++dest, doc_end, ++words_begin, words_end, subject_id, type_id);
            }
        }
        else
        {
            return weight_storage_.get(*words_begin, subject_id, type_id) 
                + match(doc_begin, ++dest, doc_end, ++words_begin, words_end, subject_id, type_id);
        }
    }

    ProblemStorageT& problem_storage_;
    WeightStorageT& weight_storage_;
};

template <typename ProblemStorageT, typename WeightStorageT>
auto make_searcher(ProblemStorageT& ps, WeightStorageT& ws)
{
    return Searcher<ProblemStorageT, WeightStorageT>(ps, ws);
}
