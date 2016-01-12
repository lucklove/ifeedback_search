#include "UnitTest.hh"
#include "search/Searcher.hh"
#include "storage/Storage.hh"

TEST_CASE(search)
{
    /**
     * \assume: 
     *      1: to be or not to be.
     *      2: be good, not be bad.
     *      3: this will be a problem.
     *      4: I yearned to be a movie actor.
     */
    struct MockProblemStorage
    {
        std::string getContent(int id)
        {
            TEST_REQUIRE(id > 0 && id < 5);
            std::unordered_map<int, std::string> m =
            {
                {1, "to be or not to be."},
                {2, "be good, not be bad."},
                {3, "this will be a problem."},
                {4, "I yearned to be a movie actor."}
            };
            return m[id];
        }

        std::vector<int> getIds(const std::string& key)
        {
            std::unordered_map<std::string, std::vector<int>> m =
            {
                {".", {1, 2, 3, 4}},
                {",", {2}},
                {"to", {1, 4}},
                {"be", {1, 2, 3, 4}},
                {"or", {1}},
                {"not", {1, 2}},
                {"good", {2}},
                {"bad", {2}},
                {"this", {3}},
                {"will", {3}},
                {"a", {3, 4}},
                {"problem", {3}},
                {"I", {4}},
                {"yearned", {4}},
                {"movie", {4}},
                {"actor", {4}}
            };
            return m[key];
        }
    } ps;

    struct MockWeightStorage
    {
        double get(const std::string& key)
        {
            std::unordered_map<std::string, double> m =
            {
                {".", 0.857},
                {",", 0.964},
                {"to", 0.928},
                {"be", 0.857},
                {"or", 0.928},
                {"not", 0.928},
                {"good", 0.964},
                {"bad", 0.964},
                {"this", 0.964},
                {"will", 0.964},
                {"a", 0.928},
                {"problem", 0.964},
                {"I", 0.964},
                {"yearned", 0.964},
                {"movie", 0.964},
                {"actor", 0.964}
            };
            return m[key];
        }
    } ws;

    auto searcher = make_searcher(ps, ws);

    std::vector<std::pair<int, double>> result_set = searcher.search("这句不存在");
    TEST_CHECK(result_set.size() == 0);

    result_set = searcher.search(".");
    TEST_CHECK(result_set.size() == 4);

    result_set = searcher.search("you are good");
    TEST_REQUIRE(result_set.size() == 1);
    TEST_CHECK(result_set[0].first == 2, "expect 2, got:", result_set[0].first);

    result_set = searcher.search("not be good");
    TEST_REQUIRE(result_set.size() == 4);
    TEST_CHECK(result_set[0].first == 2, "expect 2, got:", result_set[0].first);
    TEST_CHECK(result_set[1].first == 1, "expect 1, got:", result_set[1].first);
    TEST_CHECK(result_set[2].first == 4, "expect 4, got:", result_set[2].first);
    TEST_CHECK(result_set[3].first == 3, "expect 3, got:", result_set[3].first);

    result_set = searcher.search("I yearned to be a good boy, not a bad one.");
    TEST_REQUIRE(result_set.size() == 4);
    TEST_CHECK(result_set[0].first == 4, "expect 4, got:", result_set[0].first);
    TEST_CHECK(result_set[1].first == 2, "expect 2, got:", result_set[1].first);
    TEST_CHECK(result_set[2].first == 3, "expect 3, got:", result_set[2].first);
    TEST_CHECK(result_set[3].first == 1, "expect 1, got:", result_set[3].first);
}
