#include "../include/criterion.h"
#include "../src/main.h"
#include <stdio.h>

Test(Split_func, split_string_at_delimeter){
  char** result = splitString("this.should.split",'.');

  cr_expect(strcmp(result[0],"this") == 0);
  cr_expect(strcmp(result[1],"should") == 0);
  cr_expect(strcmp(result[2],"split") == 0);
}

Test(Split_func,single_strings_are_nullterminated){
  char** result = splitString("this.should.split",'.');

  cr_expect(result[0][strlen(result[0])] == '\0',"%c",result[0][strlen(result[0])]);
}

Test(Split_func,quits_on_q){
  char** result = splitString("q",' ');

  cr_expect(strcmp(result[0],"q") == 0);
}

Test(Split_func,end_string_split_with_NULL){
  char** result = splitString("ls -l",' ');

  cr_expect_null(result[3]);
}

Test(Parsing_directory,get_last_two_dirs){
  char* last_two_dirs = getLastTwoDirs("/Users/philipprados/Documents/coding/c/pshell");

  cr_expect(strcmp(last_two_dirs,"c/pshell") == 0);
}
