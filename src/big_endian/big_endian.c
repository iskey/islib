#include "stdio.h"

int is_big_endian()
{
  union {
    unsigned int a;
    unsigned char c[sizeof(unsigned int)];
    }endian;

  endian.a = 0x12345678;

  return (endian.c[0] == 0x12);
}

int main()
{
  printf("This system is %s.\n",is_big_endian()? "BigEndian":"LittleEndian");
  return 0;
}