#include "crow/crow.h"
#include "search/Searcher.hh"
#include "storage/ProblemDB.hh"
#include "storage/WeightDB.hh"
#include "storage/DBConnectionPool.hh"

int main()
{
    DBConnectionPool connections{std::thread::hardware_concurrency(), "db=db_ifeedback_search user=root pass=lucklove charset=utf8"};
    ProblemDB pdb{connections};
    WeightDB wdb{};

    crow::SimpleApp app;

    CROW_ROUTE(app, "/set/<int>").methods("POST"_method)([&pdb](const crow::request& req, int id)
    {
        auto x = crow::json::load(req.body);
        if(!x) return crow::response(400);
        crow::json::wvalue res;

        int subject_id = 0;
        int type_id = 0;
        if(req.url_params.get("subject")) subject_id = boost::lexical_cast<int>(req.url_params.get("subject"));
        if(req.url_params.get("type")) type_id = boost::lexical_cast<int>(req.url_params.get("type"));

        pdb.set(id, x["content"].s(), subject_id, type_id);

        res["result"] = "ok";
        return crow::response{res}; 
    });

    CROW_ROUTE(app, "/delete/<int>").methods("DELETE"_method)([&pdb](int id)
    {
        pdb.del(id);
        crow::json::wvalue res;
        res["result"] = "ok";
        return crow::response{res}; 
    });

    CROW_ROUTE(app, "/search").methods("POST"_method)([&pdb, &wdb](const crow::request& req)
    {
        auto x = crow::json::load(req.body);
        if(!x) return crow::response(400);
        crow::json::wvalue res;
      
        int subject_id = 0;
        int type_id = 0;
        size_t offset = 0;
        size_t limit = 30;
        if(req.url_params.get("subject")) subject_id = boost::lexical_cast<int>(req.url_params.get("subject"));
        if(req.url_params.get("type")) subject_id = boost::lexical_cast<int>(req.url_params.get("type"));
        if(req.url_params.get("offset")) offset = boost::lexical_cast<size_t>(req.url_params.get("offset"));
        if(req.url_params.get("limit")) limit = boost::lexical_cast<size_t>(req.url_params.get("limit"));

        static const auto searcher = make_searcher(pdb, wdb);
        auto victors = searcher.search(x["keyword"].s(), subject_id, type_id, offset + limit);

        res["result"] = "ok";
        res["victors"] = std::vector<int>{};        /**< make sure victor will be a list even if it's empty */
        for(size_t i = offset; i < std::min(victors.size(), offset + limit ? offset + limit : victors.size()); ++i)
        {
            res["victors"][i-offset]["id"] = victors[i].first;
            res["victors"][i-offset]["score"] = victors[i].second.second;
            res["victors"][i-offset]["content"] = victors[i].second.first;
        }
        return crow::response{res}; 
    });

    app.port(1024).multithreaded().run();
}
