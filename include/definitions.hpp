#pragma once


using Weight = std::uint32_t;
using VId = std::uint32_t;

struct WEdge {
    VId src_;
    VId dst_;
    Weight weight_;

    WEdge() = default;

    WEdge(VId src, VId dst, Weight weight)
            : src_{src}, dst_{dst}, weight_{weight} {}

    Weight get_weight() const { return weight_; }

    void set_weight(Weight w) { weight_ = w; }

    VId get_src() const { return src_; }

    void set_src(VId src) { src_ = src; }

    VId get_dst() const { return dst_; }

    void set_dst(VId dst) { dst_ = dst; }

    friend std::ostream &operator<<(std::ostream &out, const WEdge &edge) {
        return out << "(" << edge.src_ << ", " << edge.dst_ << ", " << edge.weight_
                   << ")";
    }
};

using WEdgeList = std::vector<WEdge>;
