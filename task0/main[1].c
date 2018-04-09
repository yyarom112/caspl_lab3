#include "toy_stdio.h"


int main(int argc, char *argv[]) {
    toy_printf("Hex unsigned: %x\n", -1);
    toy_printf("Octal unsigned: %o\n", -1);
    toy_printf("Unsigned value: %u\n", 15);
    toy_printf("signed value: %d\n", 15);
    toy_printf("Unsigned value: %u\n", -100);
    toy_printf("signed value: %d\n", -110);
    int integers_array[] = {1,2,3,4,5};
    char * strings_array[] = {"This", "is", "array", "of", "strings"};
    char chr_array[] = {'T', 'i', '@', '*', 's'};

    int array_size = 5;
    toy_printf("%Ad\n", integers_array, array_size);
    toy_printf("Print array of strings: %As\n", strings_array, array_size);
    toy_printf("Print array of char: %Ac\n", chr_array, array_size);
    toy_printf("signed value: %d\n", 15);

    toy_printf("Non-padded string: %s\n", "str");
    toy_printf("%6s\n", "str");//Right-padded string:
    toy_printf("Left-added string: %-6s\n", "str");
    toy_printf("Non-padded int: %d\n", 4);
    toy_printf("Right-padded int: %6d\n", -14);
    toy_printf("Left-added int: %-6d\n", -14);
    toy_printf("With numeric placeholders: %010d\n", -1);
    toy_printf("With numeric placeholders: %0-1d\n", -1);

}
