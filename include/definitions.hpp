#pragma once

#include <vector>
#include "../external/graphs/include/edge_types.hpp"

using Weight = std::uint32_t;
using VId = std::uint32_t;

using WEdge = graphs::WEdge24;

using WEdgeList = std::vector<WEdge>;

struct WEdgeOrigin {
    std::uint32_t src;
    std::uint32_t dst;
    std::uint32_t src_origin;
    std::uint32_t dst_origin;
    std::uint64_t weight;


    std::uint32_t get_src() const { return src; }

    std::uint32_t get_dst() const { return dst; }

    std::uint32_t get_src_origin() const { return src_origin; }

    std::uint32_t get_dst_origin() const { return dst_origin; }

    std::uint64_t get_weight() const { return weight; }

    void set_src(std::uint32_t src) { this->src = src; }

    void set_dst(std::uint32_t dst) { this->dst = dst; }

    void set_weight(std::uint64_t weight) { this->weight = weight; }

    template<typename EdgeType>
    struct WeightOrder {
        bool operator()(const EdgeType &lhs, const EdgeType &rhs) const {
            return lhs.get_weight() < rhs.get_weight();
        }
    };

    WEdge toWEdge() const {
        return {src_origin, dst_origin, weight};
    }

    WEdgeOrigin() = default;

    WEdgeOrigin(std::uint32_t src, std::uint32_t dst, std::uint64_t weight)
            : src{src}, dst{dst}, src_origin{src}, dst_origin{dst}, weight{weight} {}

    WEdgeOrigin(std::uint32_t src, std::uint32_t src_o, std::uint32_t dst, std::uint32_t dst_o, std::uint64_t weight)
            : src{src}, dst{dst}, src_origin{src_o}, dst_origin{dst_o}, weight{weight} {}

    friend std::ostream &operator<<(std::ostream &out, const WEdgeOrigin &edge) {
        return out << "(" << edge.src << ", " << edge.dst << ", " << edge.weight
                   << ")";
    }


};


struct Config {
    std::string graphType = "gnm";
    VId log_n = 10;
    VId log_m = 18;
    VId minWeight = 1;
    VId maxWeight = 254;
    VId treeFactor = 2;
    bool useKruskal = false;
    bool test = false;
    bool help = false;
    bool parseError = false;
    std::string algo = "boruvka";

    friend std::ostream &operator<<(std::ostream &out, const Config &c) {
        return out << "graph " << c.graphType << ", with log_n = " << c.log_n
                   << " and log_m = " << c.log_m << ". Edges have minWeight = " << c.minWeight << " and maxWeight = "
                   << c.maxWeight << ". Selected algorithm is " << c.algo << ", treeFactor (or d) = " << c.treeFactor
                   << ". Run test = " << c.test << ". use kruskal = " << c.useKruskal;
    }
};

bool operator==(const WEdgeOrigin &a, const WEdgeOrigin &b) {
    if (a.get_src() == b.get_src()) {
        return a.get_dst() == b.get_dst();
    } else if (a.get_src() == b.get_dst()) {
        return a.get_dst() == b.get_src();
    }
    return false;
}

using WEdgeOriginList = std::vector<WEdgeOrigin>;
