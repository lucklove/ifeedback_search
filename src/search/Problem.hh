#pragma once

struct Problem
{
    int id;
    double match_degree;

    bool operator<(const Problem& p)
    {
        return match_degree < p.match_degree;
    }
};
