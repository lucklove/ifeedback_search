#include "UnitTest.hh"
#include "cppjieba/Jieba.hpp"

TEST_CASE(cppjieba_cut_for_search)
{
    cppjieba::Jieba jieba("dict/jieba.dict.utf8", "dict/hmm_model.utf8", "");
    std::string s = "我的滑板鞋，时尚时尚最时尚。";
    std::vector<std::string> words;
    jieba.CutForSearch(s, words);
    TEST_REQUIRE(words.size() == 9);

    TEST_CHECK(words[0] == "我");
    TEST_CHECK(words[1] == "的");
    TEST_CHECK(words[2] == "滑板鞋");
    TEST_CHECK(words[3] == "，");
    TEST_CHECK(words[4] == "时尚");
    TEST_CHECK(words[5] == "时尚");
    TEST_CHECK(words[6] == "最");
    TEST_CHECK(words[7] == "时尚");
    TEST_CHECK(words[8] == "。");
}
