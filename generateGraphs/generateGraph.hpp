#pragma once

#include <vector>

namespace generate {

struct Edge
{
    int source;
    int target;
};


    class Generator{
         public: std::vector<Edge> randomGraph(int edgeCount, int VertexCount);
    };

}