#include "../include/criterion.h"
#include "../src/main.h"
#include <stdio.h>

Test(Utility_Functions, split_string_at_delimeter){
    char** result = splitString("this.should.split",'.');

    cr_expect(strcmp(result[0],"this\n"));
    cr_expect(strcmp(result[1],"should\n"));
    cr_expect(strcmp(result[2],"split\n"));
}

Test(Utility_Functions,new_test){
}
