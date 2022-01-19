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
