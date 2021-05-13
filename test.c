#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    char t [4];
    int i = 200;
    memcpy(t,&i,4);

    int rslt = (int) *t;
    int rslt2 = atoi(t);
    printf("%d\n", rslt2);
}