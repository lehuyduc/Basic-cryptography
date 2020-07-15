#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "aes.h"

int main()
{
    int res = 0;
    for (int i=0; i<=255; i++) res = res^i;
    printf("%d\n",res);
}
