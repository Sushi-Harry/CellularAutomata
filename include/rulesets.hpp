#pragma once

#include "grid.hpp"

static int location2DtoIndex();

// Here's a couple of presets. This is just an enum, the implementation for rulesets will just be done via a single interface based on the preset type

enum class RULE_PRESETS : uint8_t{
    CUSTOM,
    CONWAY,
    HIGHLIFE,
    SEEDS
};

