#include <stdio.h>

extern void say_nihao();




#include "lib1.h"
int main(int argc, char *argv[])
{
    printf("hello world!\n");
    say_nihao();


    say_hi();
    return 0;
}