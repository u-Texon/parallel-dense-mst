#pragma once

#include "../definitions.hpp"

class Vertex;

class Vertex {
public:
    Vertex(VId id, VId parent) {
        this->id = id;
        this->parent = parent;
    }
    VId getID() {
        return id;
    }
    VId getParent() {
        return parent;
    }

    void setParent(VId p) {
        parent = p;
    }

private:
    VId id;
    VId parent;
};
