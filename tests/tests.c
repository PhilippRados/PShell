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

Test(Split_func,only_command_len){
  string_array result = splitString("ma",' ');

  cr_expect(result.len == 1);
}

Test(Parsing_directory,get_last_two_dirs){
  char* last_two_dirs = getLastTwoDirs("/Users/philipprados/Documents/coding/c/pshell");

  cr_expect(strcmp(last_two_dirs,"c/pshell") == 0);
}

Test(Concatenating_command_history,check_concat_len){
  char* one = "one";
  char* two = "two";
  char* addr_one[] = { one, two };

  char* three = "three";
  char* four = "four";
  char* addr_two[] = { three, four };

  string_array arr1 = {
    .len = 2,
    .values = addr_one
  };

  string_array arr2 = {
    .len = 2,
    .values = addr_two
  };
  string_array result = concatenateArrays(arr1,arr2);

  cr_expect(result.len == 4);
}

Test(Concatenating_command_history,check_concat_elements){
  char* one = "one";
  char* two = "two";
  char* addr_one[] = { one, two };

  char* three = "three";
  char* four = "four";
  char* addr_two[] = { three, four };

  string_array arr1 = {
    .len = 2,
    .values = addr_one
  };

  string_array arr2 = {
    .len = 2,
    .values = addr_two
  };
  string_array result = concatenateArrays(arr1,arr2);
  char* concat[] = {one, two, three, four};
  string_array correct = {
    .values = concat,
    .len = 4
  };

  bool works = true;
  for (int i = 0; i < correct.len; i++){
    if (strcmp(correct.values[i],result.values[i]) != 0){
      works = false;
    }
  }
  cr_expect(works == true);
}

Test(removing_whitespace, initial_string_didnt_change){
  char* s1 = calloc(12, sizeof(char));
  s1 = "test ing";

  char* result = calloc(12,sizeof(char));
  result = removeWhitespace(s1);

  cr_expect(strcmp(s1, "test ing") == 0);
}

Test(removing_whitespace, removing_single_whitespace){
  char* s1 = calloc(12, sizeof(char));
  s1 = "test ing";

  char* result = calloc(12,sizeof(char));
  result = removeWhitespace(s1);

  cr_expect(strcmp(result, "testing") == 0);
}

Test(removing_whitespace, removing_multiple_whitespaces){
  char* s1 = calloc(12, sizeof(char));
  s1 = "test    ing";

  char* result = calloc(12,sizeof(char));
  result = removeWhitespace(s1);

  cr_expect(strcmp(result, "testing") == 0);
}

Test(findDisplayIndices, if_matching_commands_less_than_fuzzy_height){
  int matching_commands_len = 3;
  int cursor_height = 7;
  int index = 2;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 0);
  cr_expect(result.second == 3);
}

Test(findDisplayIndices, if_matching_commands_more_than_fuzzy_height_but_index_less){
  int matching_commands_len = 30;
  int cursor_height = 7;
  int index = 2;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 0);
  cr_expect(result.second == 8);
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
