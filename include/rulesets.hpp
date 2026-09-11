#pragma once

#include "grid.hpp"
#include <cstdint>
#include <iostream>

// Here's a couple of presets. This is just an enum, the implementation for rulesets will just be done via a single interface based on the preset type


/*
    ,_______,\--------\
    \_\   | | \ _____\ \
          | |  \ \    \ \
    ,_____|_|   \ \    \ \
    | |_____    _\ \____\ \
    \_______|   \_________/
*/
enum class RULE_PRESETS : uint8_t{
    CUSTOM,
    CONWAY,
    HIGHLIFE,
    SEEDS
};

class Rulesets2D{
public:
    virtual ~Rulesets2D() = default;
    virtual uint8_t EvaluateState(uint8_t curr_state, int active_neighbour_count) = 0;
    virtual std::string GetName() const = 0;
};

class ConwayRuleset : public Rulesets2D{
public:

private:

};

class HighLifeRuleset : public Rulesets2D{
public:

private:

};

class SeedsRuleset : public Rulesets2D{
public:

private:

};


/*  
    /-------\     \--------\
    \_____   \     \ _____\ \
        __\   \     \ \    \ \
        |__.   \     \ \    \ \
      ______\   \    _\ \____\ \
      \_________/    \_________/
*/
// ********************************************************************************************+
// I can do this. I can do this. I can do this. I can do this. I can do this. I can do this.   |
// There was a time I met a girl of a different kind                                           |
// We ruled the world, I thought I'd never lose her out of sight                               |
// We were so young, I think of her now and then                                               |
// I still hear the songs remindin' me of a friend                                             |
// ********************************************************************************************+
// Up on that hill across the blue lake                                                        |
// That's where I had my first heart break                                                     |
// I still remember how it all changed                                                         |
// ********************************************************************************************+
// My father said                                                                              |
// "Don't you worry, don't you worry, child"                                                   |
// "See, Heaven's got a plan for you"                                                          |
// "Don't you worry, don't you worry now"                                                      |
// ********************************************************************************************+

