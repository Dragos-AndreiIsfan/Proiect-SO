#include <stdio.h>
#include "calculate_score.h"


int main(void){
    set S;
    S.maxSize = 100;
    S.currentSize = 0;
    S = calculate_score("../../phase1/hunts",&S);
    for(int i=0;i<S.currentSize;i++){
    printf("%s|%d\n",S.multime[i].username,S.multime[i].value);
    }
    return 0;
}