//
//  main.cpp
//  PolymorphicContainer
//
//  Created by Dave Meehan on 08/04/2014.
//  Copyright (c) 2014 Replicated Solutions Limited. All rights reserved.
//

#include <stdio.h>

#include <vector>
#include <iostream>

class Animal {
    
public:
    virtual void speak() const = 0;
    
};

class Cow : public Animal {
    
public:
    
    virtual void speak() const override {
        std::cout << "Moo" << std::endl;
    }
    
};

class Sheep : public Animal {
    
public:
    
    virtual void speak() const override {
        std::cout << "Baaa" << std::endl;
    }
    
};

class Barn {
    
public:
    using Animals = std::vector<std::shared_ptr<Animal>>;
    
    void add(const std::shared_ptr<Animal> &animal) {
        _animals.emplace_back(animal);
    }
    
    Animals::iterator begin() {
        return _animals.begin();
    }
    
    Animals::iterator end() {
        return _animals.end();
    }
    
private:
    Animals _animals;
    
};

int main(int argc, char *argv[]) {

    Barn barn;
    
    barn.add(std::make_shared<Cow>());
    barn.add(std::make_shared<Sheep>());
    
    for (auto &animal : barn) {
        
        animal->speak();
        
    }
    return 0;
    
}