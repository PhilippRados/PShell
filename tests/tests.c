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
  strcpy(s1,"test ing");

  char* result = calloc(12,sizeof(char));
  result = removeWhitespace(s1);

  cr_expect(strcmp(s1, "test ing") == 0);
  free(s1);
  free(result);
}

Test(removing_whitespace, removing_single_whitespace){
  char* s1 = calloc(12, sizeof(char));
  strcpy(s1, "test ing");

  char* result = calloc(12,sizeof(char));
  result = removeWhitespace(s1);

  cr_expect(strcmp(result, "testing") == 0);
  free(s1);
  free(result);
}

Test(removing_whitespace, removing_multiple_whitespaces){
  char* s1 = calloc(12, sizeof(char));
  strcpy(s1,"test    ing");

  char* result = calloc(12,sizeof(char));
  result = removeWhitespace(s1);

  cr_expect(strcmp(result, "testing") == 0);
  free(s1);
  free(result);
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
  cr_expect(result.second == 7);
}

Test(findDisplayIndices, if_matching_commands_and_index_more_than_fuzzy_height){
  int matching_commands_len = 30;
  int cursor_height = 7;
  int index = 12;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 6);
  cr_expect(result.second == 13);
}

Test(findDisplayIndices, if_index_equals_fuzzy_height){
  int matching_commands_len = 10;
  int cursor_height = 7;
  int index = 7;

  integer_tuple result = findDisplayIndices(matching_commands_len, cursor_height, index);
  cr_expect(result.one == 1);
  cr_expect(result.second == 8);
}

Test(shift_prompt_fuzzy, shift_when_equal){
  int result = shiftPromptIfOverlapTest(11,11);

  cr_expect(result == 1);
}

Test(shift_prompt_fuzzy, dont_shift_when_cursor_higher){
  int result = shiftPromptIfOverlapTest(10,11);

  cr_expect(result == -1);
}

Test(shift_prompt_fuzzy, shift_when_cursor_lower){
  int result = shiftPromptIfOverlapTest(14,10);

  cr_expect(result == 5);
}

Test(getAppendingIndex, returns_3_if_second_word_is_len_3){
  char line[64] = "make mak";
  int result = getAppendingIndex(line, ' ');

  cr_expect(result == 3);
}

Test(getAppendingIndex, still_works_with_only_space){
  char line[64] = "make ";
  int result = getAppendingIndex(line, ' ');

  cr_expect(result == 0);
}

Test(removeDots, removes_string_when_has_dot){
  char** addr_one = calloc(4, sizeof(char*));
  addr_one[0] = calloc(strlen("one") + 1,1);
  strcpy(addr_one[0],"one");
  addr_one[1] = calloc(strlen("two") + 1,1);
  strcpy(addr_one[1],"two");
  addr_one[2] = calloc(strlen("../") + 1,1);
  strcpy(addr_one[2],"../");
  addr_one[3] = calloc(strlen("four") + 1,1);
  strcpy(addr_one[3],"four");

  string_array arr1 = {
    .len = 4,
    .values = addr_one
  };

  string_array result = removeDots(&arr1);
  
  cr_expect(strcmp(result.values[0], "one") == 0);
  cr_expect(strcmp(result.values[1], "two") == 0);
  cr_expect(strcmp(result.values[2], "four") == 0);
}

Test(removeDots, array_stays_same_when_no_dot){
  char** addr_one = calloc(4, sizeof(char*));
  addr_one[0] = calloc(strlen("one") + 1,1);
  strcpy(addr_one[0],"one");
  addr_one[1] = calloc(strlen("two") + 1,1);
  strcpy(addr_one[1],"two");
  addr_one[2] = calloc(strlen("three") + 1,1);
  strcpy(addr_one[2],"three");
  addr_one[3] = calloc(strlen("four") + 1,1);
  strcpy(addr_one[3],"four");

  string_array arr1 = {
    .len = 4,
    .values = addr_one
  };

  string_array result = removeDots(&arr1);
  
  cr_expect(strcmp(result.values[0], "one") == 0);
  cr_expect(strcmp(result.values[1], "two") == 0);
  cr_expect(strcmp(result.values[2], "three") == 0);
  cr_expect(strcmp(result.values[3], "four") == 0);
  free_string_array(&result);
}

Test(getAllMatchingFiles, should_match_only_one_file){
  char* current_dir_sub = "/Users/philipprados/documents/coding/c/pshell";
  char* removed_sub = "Ma";

  string_array result = getAllMatchingFiles(current_dir_sub, removed_sub);

  cr_expect(result.len == 1);
  cr_expect(strcmp(result.values[0], "Makefile") == 0);

  free_string_array(&result);
}

Test(getCurrentWordFromLineIndex, cursor_in_middle_of_word){
  char* one = "one";
  char* two = "two";
  char* addr_one[] = { one, two };

  string_array arr1 = {
    .len = 2,
    .values = addr_one
  };


  char* result = getCurrentWordFromLineIndex(arr1, 6);
  cr_expect(strcmp(result, "tw") == 0);
}

Test(getCurrentWordFromLineIndex, cursor_at_beginning_of_line){
  char* one = "one";
  char* two = "two";
  char* three = "three";
  char* addr_one[] = { one, two, three };

  string_array arr1 = {
    .len = 3,
    .values = addr_one
  };


  char* result = getCurrentWordFromLineIndex(arr1, 3);
  cr_expect(strcmp(result, "one") == 0);
}

Test(getCurrentWordFromLineIndex, cursor_at_end_of_line){
  char* one = "one";
  char* two = "two";
  char* addr_one[] = { one, two };

  string_array arr1 = {
    .len = 2,
    .values = addr_one
  };


  char* result = getCurrentWordFromLineIndex(arr1, 6);
  cr_expect(strcmp(result, "tw") == 0);
}

Test(insertCharAtPos, see_if_string_reference_changes){
  char line[24] = "uwe tested";

  insertCharAtPos(line, 3, 'i');
  cr_expect(strcmp(line, "uwei tested") == 0);
}

Test(insertStringAtPos, insert_string_in_middle){
  char* line = calloc(24, sizeof(char));
  strcpy(line,"testing the waters");
  char* insert_string = "cold ";

  insertStringAtPos(line, insert_string, 12);

  cr_expect(strcmp(line, "testing the cold waters") == 0);

  free(line);
}

Test(insertStringAtPos, insert_string_at_end){
  char* line = calloc(24, sizeof(char));
  strcpy(line,"testing the waters");
  char* insert_string = " here";

  insertStringAtPos(line, insert_string, strlen(line));
  cr_expect(strcmp(line, "testing the waters here") == 0);

  free(line);
}

Test(getWordEndIndex,index_in_middle_of_word){
  char* word = calloc(52, sizeof(char));
  strcpy(word , "testing Makefile works");
  int start = 9;

  int result = getWordEndIndex(word, start);

  cr_expect(result == 16);
  free(word);
}

Test(removeSlice,remove_end_if_cursor_middle){
  char* word = calloc(52, sizeof(char));
  strcpy(word , "testing if Makefile works");
  int start = 14;

  removeSlice(&word, start);

  cr_expect(strcmp(word, "testing if Mak works") == 0);
  free(word);
}

Test(removeSlice,remove_nothing_cursor_end_of_current_word){
  char* word = calloc(52, sizeof(char));
  strcpy(word , "testing if Makefile works");
  int start = 19;

  removeSlice(&word, start);

  cr_expect(strcmp(word, "testing if Makefile works") == 0);
  free(word);
}

Test(stringToLower, converting_string_lowercase_in_place){
  char* string = calloc(strlen("halle") + 1, sizeof(char));
  strcpy(string, "HaLLe");

  stringToLower(string);

  cr_expect(strcmp(string, "halle") == 0);
  free(string);
}

Test(isFile, check_if_existing_file_is_true){
  int result = isFile(".gitignore");
  cr_expect(result == true);
}

Test(isFile, check_if_not_existing_file_is_false){
  int result = isFile("not_existant");
  cr_expect(result == false);
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
