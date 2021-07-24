#include <stdlib.h>
#include <string.h>

char** splitString(char* string_to_split,char delimeter){
    int start = 0;
    int j = 0;
    char** result = malloc(sizeof(char) * strlen(string_to_split));

    for (int i = 0;;i++){
        if (string_to_split[i] == delimeter || string_to_split[i] == '\0'){
            char* current_word = (char*)malloc(sizeof(char) * (i - start));
            current_word = strncpy(current_word,&string_to_split[start],(i - start));
            result[j] = (char*)malloc(sizeof(char) * (i - start));
            memcpy(result[j],current_word, sizeof(char) * strlen(current_word));
            start = i + 1;
            j++;
        }
        if (string_to_split[i] == '\0') break;
    }
    return result;
}
