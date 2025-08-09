//
// Created by mBlueberry on 09.08.2025.
//

#include "AirBlock.h"


AirBlock::AirBlock() : Block() {
    this->solid = false;
    this->type_name = "air";
    this->texture = TEXTURE_WOOD;
}

AirBlock AirBlock::INSTANCE;