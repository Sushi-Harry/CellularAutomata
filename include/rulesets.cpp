#include "rulesets.hpp"

// /33333\ DDDDDDD\
// 33   33  DD   DD
//    <33<  DD   DD
// 33   33  DD   DD
// \33333/ DDDDDDD/

uint8_t PresetRuleset3D::EvaluateState(uint8_t curr_state, int active_neighbour_count) const{
    switch(_type){
        case RULE_PRESET3D::LIFE_3D:{
            if(curr_state == 1)
                // applying survival rules since the current cell just needs to know if it's allowed to survive
                return (active_neighbour_count >= 5 && active_neighbour_count <= 7) ? 1 : 0;
            
            // THis is the birth condition. Returns 1 (alive state) if the conditions meet
            return (active_neighbour_count >= 5 && active_neighbour_count <= 7) ? 1 : 0;
            break;
        }
        case RULE_PRESET3D::AMOEBA_3D:{
            if(curr_state == 1)
                // applying survival rules since the current cell just needs to know if it's allowed to survive
                return (active_neighbour_count >= 3 && active_neighbour_count <= 5) ? 1 : 0;
            
            // THis is the birth condition. Returns 1 (alive state) if the conditions meet
            return (active_neighbour_count >= 4 && active_neighbour_count <= 5) ? 1 : 0;
            break;
        }
        case RULE_PRESET3D::CRYSTAL_3D:{
            if(curr_state == 1)
                // applying survival rules since the current cell just needs to know if it's allowed to survive
                return 1;
            
            // THis is the birth condition. Returns 1 (alive state) if the conditions meet
            return (active_neighbour_count >= 1 && active_neighbour_count <= 3) ? 1 : 0;
            break;
        }
        case RULE_PRESET3D::BAYS_3D:{
            if(curr_state == 1)
                // applying survival rules since the current cell just needs to know if it's allowed to survive
                return (active_neighbour_count >= 11 && active_neighbour_count <= 19) ? 1 : 0;
            
            // THis is the birth condition. Returns 1 (alive state) if the conditions meet
            return (active_neighbour_count >= 13 && active_neighbour_count <= 14) ? 1 : 0;
            break;
        }
        case RULE_PRESET3D::CUSTOM:{
            // TEMPORARY UNTIL I FIGURE OUT HOW TO IMPLEMENT CUSTOM RULES 
            return curr_state;
            break;
        }
    }

    return curr_state;
}

std::string PresetRuleset3D::GetName() const {
    switch(_type){
        case RULE_PRESET3D::LIFE_3D:{
            return "LIFE_3D";
            break;
        }
        case RULE_PRESET3D::AMOEBA_3D:{
            return "AMOEBA_3D";
            break;
        }
        case RULE_PRESET3D::CRYSTAL_3D:{
            return "CRYSTAL_3D";
            break;
        }
        case RULE_PRESET3D::BAYS_3D:{
            return "BAYS_3D";
            break;
        }
        case RULE_PRESET3D::CUSTOM:{
            // TEMPORARY UNTIL I FIGURE OUT HOW TO IMPLEMENT CUSTOM RULES 
            return "CUSTOM";
            break;
        }
    }

    return "UNKNOWN";
}