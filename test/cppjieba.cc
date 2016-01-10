#include "UnitTest.hh"
#include "cppjieba/Jieba.hpp"

TEST_CASE(cppjieba_cut_for_search)
{
    cppjieba::Jieba jieba("dict/jieba.dict.utf8", "dict/hmm_model.utf8", "");
    std::string s = "工信处女干事每月经过下属科室都要亲口交代24口交换机等技术性器件的安装工作";
    std::vector<std::string> words;
    jieba.CutForSearch(s, words);

    TEST_REQUIRE(words.size() == 19);
    TEST_CHECK(words[0] == "工信处");
    TEST_CHECK(words[3] == "经过");
    TEST_CHECK(words[9] == "交代");
    TEST_CHECK(words[12] == "交换机");
    TEST_CHECK(words[15] == "器件");
}
