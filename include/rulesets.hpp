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
// To Sxm Azd: Write comments above these enum entries to explain the birth and survival conditions like I did in the RULE_PRESET3D enum in this file

class Rulesets2D{
public:
   virtual ~Rulesets2D() = default;
   virtual uint8_t EvaluateState(uint8_t curr_state, int active_neighbour_count) const = 0;
   virtual std::string GetName() const = 0;
};

// Any Life-like rule in B/S notation, e.g. B3/S23.
class CustomRuleset2D : public Rulesets2D {
public:
    CustomRuleset2D(const std::vector<int>& birth, const std::vector<int>& survival);

    uint8_t EvaluateState(uint8_t curr_state, int active_neighbour_count) const override;
    std::string GetName() const override;  // returns e.g. "B36/S23"

private:
    uint16_t _birth = 0;     // bit n set => dead cell with n neighbours is born
    uint16_t _survival = 0;  // bit n set => live cell with n neighbours survives
};

enum class RULE_PRESET2D : uint8_t {
    // B3 / S2-3 for life2d
    LIFE_2D = 0,
    // B3,6 / S2-3 for highlife2d
    HIGHLIFE_2D = 1,
    // B2 / S (NONE) for seeds2d
    SEEDS_2D = 2,
};

class PresetRuleset2D : public CustomRuleset2D {
public:
    explicit PresetRuleset2D(RULE_PRESET2D type);

    std::string GetName() const override;
    RULE_PRESET2D GetType() const { return _type; }

private:
    RULE_PRESET2D _type;
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
// ********************************************************************************************+
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

enum class RULE_PRESET3D : uint8_t{
    // B5-7 / S5-7 for life3d
    LIFE_3D = 0,
    // B4-5 / S3-5 for amoeba3d
    AMOEBA_3D = 1,
    // B1-3 / S (ALL) for crystal3d
    CRYSTAL_3D = 2,
    // B13-14 / S11-19 for bays3d
    BAYS_3D = 3,
    // Custom Birth and Survival Conditions
    CUSTOM = 4,
};

class Rulesets3D {
public:
    virtual ~Rulesets3D() = default;
    virtual uint8_t EvaluateState(uint8_t curr_state, int active_neighbour_count) const = 0;
    virtual std::string GetName() const = 0;
};

class PresetRuleset3D : public Rulesets3D{
public:
    PresetRuleset3D(RULE_PRESET3D type) : _type(type) {}
    uint8_t EvaluateState(uint8_t curr_state, int active_neighbour_count) const override;
    std::string GetName() const override;
private:
    RULE_PRESET3D _type;
};

class CustomRuleset3D : public Rulesets3D{
public:
    CustomRuleset3D(const std::vector<int>& birth, const std::vector<int>& survival);
    uint8_t EvaluateState(uint8_t curr_state, int active_neighbour_count) const override;
    std::string GetName() const override { return "CUSTOM"; };
private:
    bool _birth[27];
    bool _survival[27];
};