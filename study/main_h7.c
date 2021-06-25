#include "expre.h"
#include "gen.h"
#include "scanner.h"
#include "writer.h"
#include "statement.h"
#include "decl.h"
#include "sym.h"
#include "register.h"
#include "global_context.h"

static void parseStatement(Content *cd, Writer *w){
    struct Token token;
    struct ASTnode *tree;

    scanner_init(cd);
    cd->start(cd->context);
    w->start(w->context, 0);

    scanner_scan(cd, &token);
    gen_preamble(w);
    decl_global(cd, w, &token);
    gen_postamble(w, 0);

    cd->end(cd->context);
    w->end(w->context);
}

static char *getFilePath(const char *dir, const char *rPath)
{
    char *filepath = (char *)malloc(strlen(dir) + strlen(rPath) + 1);
    strcpy(filepath, dir);
    strcpy(filepath + strlen(dir), rPath);
    return filepath;
}

#define _ABSOLUTE_EXE_PATH
static char *getCurrentFilePath(const char *rPath)
{
    #ifdef _ABSOLUTE_EXE_PATH
        const char *buf = "E:/study/github/mine/acwj";
       // const char *buf = "E:/study/github/mine_clone/acwj";
    #else
        char buf[80];
        getcwd(buf, sizeof(buf));
        printf("work dir: %s, len =%d \n", buf, sizeof(buf));
    #endif
    return getFilePath(buf, rPath);
}
int main(int argc, char **args)
{

    Register* reg = register_new(REGISTER_TYPE_X64);
    struct GlobalState* gs = sym_globalState_new();

    Content *cd;
    Writer *w;
    if (argc == 1)
    {
        // const char *buffer = "2 + 3 * 5 - 8 / 3";
        // cd = content_new(CONTENT_TYPE_TEXT, (void *)buffer);
        // char* outFile = getCurrentFilePath("/study/note/out.s");
       // char* outFile = getCurrentFilePath("/study/res/input01.txt");
      //  char* outFile = getCurrentFilePath("/study/res/input02");
      //  char* outFile = getCurrentFilePath("/study/res/input04");
      //  char* outFile = getCurrentFilePath("/study/res/input05");
      //  char* outFile = getCurrentFilePath("/study/res/input06.txt");
       // char* outFile = getCurrentFilePath("/study/res/input07");
       // char* outFile = getCurrentFilePath("/study/res/input08"); //func1 11
      //  char* outFile = getCurrentFilePath("/study/res/input10"); //12
       // char* outFile = getCurrentFilePath("/study/res/input14"); //13
        char* outFile = getCurrentFilePath("/study/res/input27a.c");
        cd = content_new(CONTENT_TYPE_FILE, (void *)outFile);
        free(outFile);

        outFile = getCurrentFilePath("/study/note/out_27.s");
        w = writer_new(WRITER_TYPE_FILE, outFile);
        free(outFile);
    }
    else
    {
        if (argc == 2)
        {
            cd = content_new(CONTENT_TYPE_FILE, args[1]);
            char* outFile = getCurrentFilePath("/study/note/out_27.s");
            w = writer_new(WRITER_TYPE_FILE, outFile);
            free(outFile);
        }
        else
        {
            cd = content_new(CONTENT_TYPE_FILE, args[1]);
            w = writer_new(WRITER_TYPE_FILE, args[2]);
        }
    }
    //true to dump ast
    cd->context->dumpAST = 1;

    GlobalContext gCtx;
    globalContext_init(&gCtx, gs, reg, cd, w);

     // For now, ensure that void printint() and printchar() are defined
    sym_addglob(gs, "printint", P_INT, S_FUNCTION, 0, 0);
    sym_addglob(gs, "printchar", P_VOID, S_FUNCTION, 0, 0);

    parseStatement(cd, w);

    globalContext_destroy(&gCtx);
    return 0;
}