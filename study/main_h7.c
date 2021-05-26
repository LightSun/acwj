#include "expre.h"
#include "gen.h"
#include "scanner.h"
#include "writer.h"
#include "statement.h"
#include "decl.h"

static void parseStatement(Content *cd, Writer *w){
    struct Token token;
    struct ASTnode *tree;

    scanner_init(cd);
    cd->start(&cd->context);
    w->start(w->context, 0);

    scanner_scan(cd, &token);
    gen_preamble(w);
    while(1){
        tree = decl_function(cd, w, &token);
        gen_genAST(tree, w, NOREG, 0);
        if(token.token == T_EOF){
            break;
        }
    }
/*     tree = statement_parse(cd, w, &token);
    gen_genAST(tree, w, NOREG, 0);
    gen_postamble(w); */

    cd->end(&cd->context);
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
        //const char *buf = "E:/study/github/mine/acwj";
        const char *buf = "E:/study/github/mine_clone/acwj";
    #else
        char buf[80];
        getcwd(buf, sizeof(buf));
        printf("work dir: %s, len =%d \n", buf, sizeof(buf));
    #endif
    return getFilePath(buf, rPath);
}

int main(int argc, char **args)
{
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
        char* outFile = getCurrentFilePath("/study/res/input08"); //func1
        cd = content_new(CONTENT_TYPE_FILE, (void *)outFile);
        free(outFile);

        outFile = getCurrentFilePath("/study/note/out_11.s");
        w = writer_new(WRITER_TYPE_FILE, outFile);
        free(outFile);
    }
    else
    {
        if (argc == 2)
        {
            cd = content_new(CONTENT_TYPE_FILE, args[1]);
            char* outFile = getCurrentFilePath("/study/note/out_11.s");
            w = writer_new(WRITER_TYPE_FILE, outFile);
            free(outFile);
        }
        else
        {
            cd = content_new(CONTENT_TYPE_FILE, args[1]);
            w = writer_new(WRITER_TYPE_FILE, args[2]);
        }
    }
    //scanContent(cd);
    //parseContent(cd, w);
    parseStatement(cd, w);
    content_delete(cd);
    writer_delete(w);
    return 0;
}