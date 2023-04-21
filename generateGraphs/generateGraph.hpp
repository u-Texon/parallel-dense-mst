#pragma once

#include <vector>
#include <bits/stdc++.h>
#include "../include/definitions.hpp"

namespace generate {


    inline std::vector<WEdge> randomGraph(int edgeCount, int vertexCount, int maxWeight) {

        assert(edgeCount <= vertexCount * (vertexCount - 1) / 2);   //check for parallel edges

        std::vector<WEdge> vec;
        int i, j, edge[edgeCount][2], count;
        i = 0;

        while (i < edgeCount) {
            edge[i][0] = rand() % vertexCount;
            edge[i][1] = rand() % vertexCount;

            if (edge[i][0] == edge[i][1])
                continue;
            else {
                for (j = 0; j < i; j++) {
                    if ((edge[i][0] == edge[j][0]
                         && edge[i][1] == edge[j][1])
                        || (edge[i][0] == edge[j][1]
                            && edge[i][1] == edge[j][0]))
                        i--;
                }
            }
            i++;
        }
        for (i = 0; i < vertexCount; i++) {
            for (j = 0; j < edgeCount; j++) {
                if (edge[j][0] == i) {
                    WEdge e{};
                    e.set_src(i);
                    e.set_dst(edge[j][1]);
                    e.set_weight(rand() % (maxWeight + 1));
                    vec.push_back(e);
                }
            }
        }
        return vec;
    }

}