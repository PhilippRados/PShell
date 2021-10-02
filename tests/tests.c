#include "../include/criterion.h"
#include "../src/main.h"
#include <stdio.h>

// Unit Tests
Test(Split_func, split_string_at_delimeter){
  string_array result = splitString("this.should.split",'.');

  cr_expect(strcmp(result.values[0],"this") == 0);
  cr_expect(strcmp(result.values[1],"should") == 0);
  cr_expect(strcmp(result.values[2],"split") == 0);
}

Test(Split_func,single_strings_are_nullterminated){
  string_array result = splitString("this.should.split",'.');

  cr_expect(result.values[0][strlen(result.values[0])] == '\0',"%c",result.values[0][strlen(result.values[0])]);
}

Test(Split_func,quits_on_q){
  string_array result = splitString("q",' ');

  cr_expect(strcmp(result.values[0],"q") == 0);
}

Test(Split_func,end_string_split_with_NULL){
  string_array result = splitString("ls -l",' ');

  cr_expect_null(result.values[3]);
}

Test(Parsing_directory,get_last_two_dirs){
  char* last_two_dirs = getLastTwoDirs("/Users/philipprados/Documents/coding/c/pshell");

  cr_expect(strcmp(last_two_dirs,"c/pshell") == 0);
}

/* // End-to-end user-journey tests */
/* Test(User_journey,typing_ls){ */
/*   FILE* file = fopen("/Users/philipprados/documents/coding/c/pshell/user_test.txt","r"); */
/*   char* command = NULL; */
/*   size_t size = 0; */
/*  */
/*   getline(&command,&size,file); */
/*   cr_expect(strcmp(command,"ls\n") == 0,"Expected: simones\tReceived:%s",command); */
/*   fclose(file); */
/* } */
/*  */
/* Test(User_journey,last_history_command_adding_a_flag){ */
/*   FILE* file = fopen("/Users/philipprados/documents/coding/c/pshell/user_test.txt","r"); */
/*   char* command = NULL; */
/*   size_t size = 0; */
/*  */
/*   fseek(file,getFileSizeAtIndex(file,2),SEEK_SET); */
/*   getline(&command,&size,file); */
/*   cr_expect(strcmp(command,"ls -a\n") == 0,"Expected: simones\tReceived:%s",command); */
/*   fclose(file); */
/* } */
/*  */
/* Test(User_journey,typing_command_and_deleting_last_char){ */
/*   FILE* file = fopen("/Users/philipprados/documents/coding/c/pshell/user_test.txt","r"); */
/*   char* command = NULL; */
/*   size_t size = 0; */
/*  */
/*   fseek(file,getFileSizeAtIndex(file,3),SEEK_SET); */
/*   getline(&command,&size,file); */
/*   cr_expect(strcmp(command,"uw\n") == 0,"Expected: simones\tReceived:%s",command); */
/*   fclose(file); */
/* } */
/*  */
/* Test(User_journey,typing_command_and_pressing_up_arrow_and_adding_chars){ */
/*   FILE* file = fopen("/Users/philipprados/documents/coding/c/pshell/user_test.txt","r"); */
/*   char* command = NULL; */
/*   size_t size = 0; */
/*  */
/*   fseek(file,getFileSizeAtIndex(file,4),SEEK_SET); */
/*   getline(&command,&size,file); */
/*   cr_expect(strcmp(command,"simeones\n") == 0,"Expected: simones\tReceived:%s",command); */
/*   fclose(file); */
/* } */
/*  */
/* Test(User_journey,deleting_more_chars_than_command_len_and_typing_new){ */
/*   FILE* file = fopen("/Users/philipprados/documents/coding/c/pshell/user_test.txt","r"); */
/*   char* command = NULL; */
/*   size_t size = 0; */
/*  */
/*   fseek(file,getFileSizeAtIndex(file,5),SEEK_SET); */
/*   getline(&command,&size,file); */
/*   cr_expect(strcmp(command,"simon\n") == 0,"Expected: simones\tReceived:%s",command); */
/*   fclose(file); */
/* } */
