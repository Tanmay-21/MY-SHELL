#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char* urlencode (char* originalText) {
  // Allocate memory for the worst possible case (all chara need to be encoded)
  char *encoded = (char*) malloc (sizeof(char)*strlen(originalText)*3+1);
  const char *hex = "0123456789abcdef";
  int pos = 0;
  for (int i = 0; i < strlen(originalText); i++) {
    if (isalnum(originalText[i])) {
      encoded[pos++] = originalText[i];
    } else {
      encoded[pos++] = '%';
      encoded[pos++] = hex[originalText[i] >> 4];
      encoded[pos++] = hex[originalText[i] & 15];
    }
  }
  encoded[pos] = '\0';
  return encoded;
}
