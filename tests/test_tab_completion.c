#include "../include/criterion.h"
#include "../src/main.h"
#include <stdio.h>

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

