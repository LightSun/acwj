
#include "register/register_x64.h"
#include "register/register_arm.h"
#include "register.h"

Register* register_new(int type){
    Register *reg = (Register*)malloc(sizeof(Register));
    reg->context = (RegisterContext*)malloc(sizeof(RegisterContext));
    memset(reg->context, 0, sizeof(RegisterContext));
    reg->context->intlist = (int*) malloc(sizeof(int) * REG_MAXINTS);
    switch (type)
    {
    case REGISTER_TYPE_X64:
        REGISTER_ASSIGN_FUNCS(reg, x64);
        break;

     case REGISTER_TYPE_ARM:
        REGISTER_ASSIGN_FUNCS(reg, arm);
        break;
    default:
        break;
    }
    return reg;
}

void register_delete(Register* r){
    if(r->context){
        free(r->context->intlist);
        free(r->context);
        r->context = NULL;
    }
    free(r);
}