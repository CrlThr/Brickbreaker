#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include "Component.h"
#include <unordered_map>
#include <string>

class Entity {
public:
    std::unordered_map<std::string, void*> components;

    template<typename T>
    void addComponent(T component) {
        components[typeid(T).name()] = new T(component);
    }

    template<typename T>
    T* getComponent() {
        return static_cast<T*>(components[typeid(T).name()]);
    }

    template<typename T>
    bool hasComponent() const {
        return components.count(typeid(T).name()) > 0;
    }
};

#endif