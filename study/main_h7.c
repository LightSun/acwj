#include "expre.h"
#include "gen.h"
#include "scanner.h"
#include "writer.h"
#include "statement.h"

// List of printable tokens
static char *tokstr[] = {"+", "-", "*", "/", "intlit"};

//only scan
static void scanContent(Content *cd)
{
    scanner_init(cd);
    struct Token T;

    cd->start(cd->context.param);
    while (scanner_scan(cd, &T))
    {
        printf("Token %s", tokstr[T.token]);
        if (T.token == T_INTLIT)
            printf(", value %d", T.intvalue);
        printf("\n");
    }
    cd->end(cd->context.param);
}

//parse and evaluate(no order)
static void parseContent(Content *cd, Writer *w)
{
    scanner_init(cd);
    cd->start(cd->context.param);  
    struct ASTnode *ast = expre_parseAST(cd);
    cd->end(cd->context.param);
    int result = expre_evaluateAST(ast);
    printf("parseContent: >> evaluate result = %d\n", result);
    gen_genCode(ast, w);
}

static void parseStatement(Content *cd, Writer *w){
    struct Token token;
    scanner_init(cd);
    cd->start(&cd->context);

    scanner_scan(cd, &token);
    gen_preamble(w);
    statement_parse(cd, w, &token);
    gen_postamble(w);

    cd->end(&cd->context);
}

static char *getFilePath(const char *dir, const char *rPath)
{
    char *filepath = (char *)malloc(strlen(dir) + strlen(rPath) + 1);
    strcpy(filepath, dir);
    strcpy(filepath + strlen(dir), rPath);
    return filepath;
}

static char *getCurrentFilePath(const char *rPath)
{
    char buf[80];
    getcwd(buf, sizeof(buf));
    printf("work dir: %s, len =%d \n", buf, sizeof(buf));
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
        // char* outFile = getCurrentFilePath("/study/out.s");
        char* outFile = getCurrentFilePath("/study/input01.txt");
        cd = content_new(CONTENT_TYPE_FILE, (void *)outFile);
        free(outFile);

        outFile = getCurrentFilePath("/study/out1.s");
        w = writer_new(WRITER_TYPE_FILE, outFile);
        free(outFile);
    }
    else
    {
        if (argc == 2)
        {
            cd = content_new(CONTENT_TYPE_FILE, args[1]);
            char* outFile = getCurrentFilePath("/study/out1.s");
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