#include "scanner.h"
#include "expre.h"

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
static void parseContent(ContentDelegate* cd){
    scanner_init();
    cd->start(cd->param);
    struct ASTnode* ast = expre_parseAST(cd);
    cd->end(cd->param);
    int result = expre_evaluateAST(ast);
    printf("parseContent: >> evaluate result = %d", result);
}

int main(int argc, char** args){
    ContentDelegate *cd;
    if(argc == 1){
        const char *buffer = "2 + 3 * 5 - 8 / 3";
        cd = content_newDelegate(CONTENT_TYPE_TEXT, (void*)buffer);
    }else{
        cd = content_newDelegate(CONTENT_TYPE_FILE, args[1]);
    }
    //scanContent(cd);
    parseContent(cd);
    content_deleteDelegate(cd);
    return 0;
}