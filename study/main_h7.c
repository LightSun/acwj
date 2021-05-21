#include "scanner.h"
#include "expre.h"
#include "gen.h"
#include "writer.h"

// List of printable tokens
char *tokstr[] = { "+", "-", "*", "/", "intlit" };

//only scan
static void scanContent(ContentDelegate* cd) {
    scanner_init();
    struct Token T;

    cd->start(cd->param);
    while (scanner_scan(cd, &T))
    {
        printf("Token %s", tokstr[T.token]);
        if (T.token == T_INTLIT)
            printf(", value %d", T.intvalue);
        printf("\n");
    }
    cd->end(cd->param);
}

//parse and evaluate(no order)
static void parseContent(ContentDelegate* cd, Writer *w){
    scanner_init();
    cd->start(cd->param);
    struct ASTnode* ast = expre_parseAST(cd);
    cd->end(cd->param);
    int result = expre_evaluateAST(ast);
    printf("parseContent: >> evaluate result = %d", result);
    gen_genCode(ast, w);
}

int main(int argc, char** args){
    ContentDelegate *cd;
    Writer *w;
    if(argc == 1){
        const char *buffer = "2 + 3 * 5 - 8 / 3";
        w = writer_new(WRITER_TYPE_TEXT, NULL);
        cd = content_newDelegate(CONTENT_TYPE_TEXT, (void*)buffer);
    }else{
        if(argc == 2){
            cd = content_newDelegate(CONTENT_TYPE_FILE, args[1]);
            w = writer_new(WRITER_TYPE_FILE, "out.s");
        }else{
            cd = content_newDelegate(CONTENT_TYPE_FILE, args[1]);
            w = writer_new(WRITER_TYPE_FILE, args[2]);    
        }
    }
    //scanContent(cd);
    parseContent(cd, w);
    content_deleteDelegate(cd);
    writer_delete(w);
    return 0;
}