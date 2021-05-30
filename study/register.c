
#include "register/register_x64.h"
#include "register.h"

Register* register_new(int type){
    Register *reg = (Register*)malloc(sizeof(Register));
    reg->context = (RegisterContext*)malloc(sizeof(RegisterContext));
    memset(reg->context, 0, sizeof(RegisterContext));
    switch (type)
    {
    case REGISTER_TYPE_X64:
        REGISTER_ASSIGN_FUNCS(reg, x64);
        break;

     case REGISTER_TYPE_ARM64:
        //TODO REGISTER_ASSIGN_FUNCS(reg, x64);
        break;
    default:
        break;
    }
    return reg;
}

void register_delete(Register* r){
    if(r->context){
        free(r->context);
        r->context = NULL;
    }
    free(r);
}