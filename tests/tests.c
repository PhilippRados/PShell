#include "../include/criterion.h"
#include "../src/main.h"
#include <stdio.h>

Test(Utility_Functions, split_string_at_delimeter){
    char** result = splitString("this.should.split",'.');

    cr_expect(strcmp(result[0],"this") == 0);
    cr_expect(strcmp(result[1],"should") == 0);
    cr_expect(strcmp(result[2],"split") == 0);
}

Test(Parsind_directory,get_last_two_dirs){
  char* last_two_dirs = getLastTwoDirs("/Users/philipprados/Documents/coding/c/pshell\n");

  cr_expect(strcmp(last_two_dirs,"/c/pshell") == 0);
}
