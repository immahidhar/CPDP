#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char *temp;
    temp = getenv("PWD");
    if (temp == NULL) temp = "";
    printf("%s\n", temp);
}
