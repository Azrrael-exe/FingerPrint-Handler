#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

#include <iostream>
#include <fstream>

int test(unsigned char** d, unsigned int* s){
  unsigned char* val = (unsigned char*)malloc(10);
  for(int i = 0; i<10; i++){
    val[i] = 'A';
  }
  *s = sizeof(char[10]);
  *d = val;
  return 1;
}

int main() {
  unsigned char* data;
  unsigned int size;
  test(&data ,&size);
  printf("size: %d\n", size);
  printf("%s\n", data);
  return 0;
}
