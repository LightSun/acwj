#include "register_common.h"

// Reset the position of new local variables when parsing a new function
void register_common_cgresetlocals(REGISTER_CONTEXT_PARAM){
    ctx->localOffset = 0;
}