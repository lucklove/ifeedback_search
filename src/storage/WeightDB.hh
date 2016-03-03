#pragma once
#include <vector>
#include <cmath>
#include <redox.hpp>

struct WeightDB
{
    std::unordered_map<std::string, double> get(const std::vector<std::string>& keys, int subject_id = 0, int type_id = 0) const
    {
        redox::Redox rdx;
        if(!rdx.connect("localhost", 6379))
            throw std::runtime_error{"can not connect to redis server"};

        std::unordered_map<std::string, double> words_map;
        for(const std::string& key : keys)
        {
            std::stringstream iss;
            iss << key << "#" << subject_id << "#" << type_id;
            std::string s;
            try { s = rdx.get(iss.str()); } catch(std::runtime_error&) {}
            std::stringstream oss{s};
            words_map[key] = 0.0;
            oss >> words_map[key];
        }
        
        return words_map;    
    }
};
