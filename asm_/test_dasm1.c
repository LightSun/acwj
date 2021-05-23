
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **args){
    //2 + 3 * 5 - 8 / 3
    int a = 2;
    int b = 3;
    int c = 5;
    int d = 8;
    int e = 3;

    b = b * c;
    a = 2 + b;
    b = d / e;
    a = a - b;
    printf("%s %d\n", "result: ", a);
    return 0;
}