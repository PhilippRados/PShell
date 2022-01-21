#include "../include/criterion.h"
#include "../src/util.h"

// Unit Tests
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

Test(insertCharAtPos, see_if_string_reference_changes){
  char line[24] = "uwe tested";

  insertCharAtPos(line, 3, 'i');
  cr_expect(strcmp(line, "uwei tested") == 0);
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
