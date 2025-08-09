//
// Created by mBlueberry on 09.08.2025.
//

#include "BedrockBlock.h"


BedrockBlock::BedrockBlock() : Block() {
    this->solid = true;
    this->type_name = "bedrock";
    this->texture = TEXTURE_BEDROCK;
}

BedrockBlock BedrockBlock::INSTANCE;