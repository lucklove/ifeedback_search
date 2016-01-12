#include "crow/crow.h"
#include "search/Searcher.hh"
#include "storage/ProblemDB.hh"
#include "storage/WeightDB.hh"
#include "storage/DBConnectionPool.hh"

int main()
{
    DBConnectionPool connections{std::thread::hardware_concurrency(), "db=db_ifeedback_search user=root pass=lucklove"};
    ProblemDB pdb{connections};
    WeightDB wdb{connections};

    crow::SimpleApp app;

    CROW_ROUTE(app, "/add/<int>").methods("POST"_method)([&pdb](const crow::request& req, int id)
    {
        auto x = crow::json::load(req.body);
        if(!x) return crow::response(400);
        crow::json::wvalue res;

        pdb.set(id, x["content"].s());

        res["result"] = "ok";
        return crow::response{res}; 
    });

    CROW_ROUTE(app, "/search").methods("POST"_method)([&pdb, &wdb](const crow::request& req)
    {
        auto x = crow::json::load(req.body);
        if(!x) return crow::response(400);
        crow::json::wvalue res;
        
        static const auto searcher = make_searcher(pdb, wdb);
        auto victors = searcher.search(x["keyword"].s());
  
        res["result"] = "ok";
        res["victors"] = std::vector<int>{};        /**< make sure victor will be a list even if it's empty */
        for(size_t i = 0; i < victors.size(); ++i)
        {
            res["victors"][i]["id"] = victors[i].first;
            res["victors"][i]["score"] = victors[i].second;
            res["victors"][i]["content"] = pdb.getContent(victors[i].first);
        }
        return crow::response{res}; 
    });

    app.port(8080).multithreaded().run();
}
