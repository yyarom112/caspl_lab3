/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>


/* the states in the printf state-machine */
enum printf_state {
    st_printf_init,
    st_printf_percent,
    st_printf_octal2,
    st_printf_octal3
} printf_state;

typedef struct {
    char *fs;
    enum printf_state state;
} state_args;

typedef struct {
    int printed_chars;
    enum printf_state next_state;
}state_result;

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

int toy_printf(char *fs, ...);

int numDigits(const int n);


const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int print_int_helper(unsigned int n, int radix, const char *digit) {
    int result;

    if (n < radix) {
        putchar(digit[n]);
        return 1;
    } else {
        result = print_int_helper(n / radix, radix, digit);
        putchar(digit[n % radix]);
        return 1 + result;
    }
}

int print_int(unsigned int n, int radix, const char *digit) {
    if (radix < 2 || radix > 16) {
        toy_printf("Radix must be in [2..16]: Not %d\n", radix);
        exit(-1);
    }

    if (n > 0) {
        return print_int_helper(n, radix, digit);
    }
    if (n == 0) {
        putchar('0');
        return 1;
    } else {
        putchar('-');
        return 1 + print_int_helper(-n, radix, digit);
    }
}

enum printf_state precent_state_handler(va_list args, int *out_printed_chars, state_args *state) {
    return st_printf_percent;
}

enum printf_state init_state_handler(va_list args, int *out_printed_chars, state_args *state) {
    switch (*(state->fs)) {
        case '%':
            return precent_state_handler(args, out_printed_chars, state);

        default:
            putchar(*(state->fs));
            ++(*out_printed_chars);
    }
    return st_printf_init;
}

enum printf_state D_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, int *nDigits, int *zero,
                int *padd, int *neg) {
    *int_value = va_arg(args, int);
    *nDigits = numDigits(*int_value);
    if (*zero == 1) {
        if (*int_value < 0) {
            putchar('-');
            (*padd)--;
            *int_value = -*int_value;
        }
        while ((*padd) - (*nDigits) > 0) {
            putchar('0');
            (*padd)--;
        }
    }
    if (*neg == 1) {
        while (*padd - *nDigits > 0) {
            putchar(' ');
            (*padd)--;
        }
    }
    if (*int_value < 0) {
        putchar('-');
        *int_value = -*int_value;
    }
    *out_printed_chars += print_int((*int_value), 10, digit);
    if (*neg == 0 && (*padd) - (*nDigits) > 0) {
        while (*padd - *nDigits > 0) {
            putchar(' ');
            (*padd)--;
        }
        putchar('#');
    }
    return st_printf_init;
}

enum printf_state S_state_handler(va_list args, int *out_printed_chars, state_args *state, char *string_value, int *nDigits, int *zero,
                int *padd, int *neg) {
    string_value = va_arg(args, char *);
    if (padd > 0)
        for (*nDigits = 0; string_value[*nDigits] != '\0'; (*nDigits)++);
    if (*neg == 1) {
        while (*padd - *nDigits > 0) {
            putchar(' ');
            padd--;
        }
    }
    while (*string_value) {
        (*out_printed_chars)++;
        putchar(*string_value);
        string_value++;
    }
    if (*padd - *nDigits > 0) {
        while (*padd - *nDigits > 0) {
            putchar(' ');
            padd--;
        }
        putchar('#');
    }
    return st_printf_init;
}

enum printf_state B_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value) {
    *int_value = va_arg(args, int);
    (*out_printed_chars) += print_int(*int_value, 2, digit);
    return st_printf_init;
}

enum printf_state O_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value) {
    *int_value = va_arg(args, int);
    (*out_printed_chars) += print_int(*int_value, 8, digit);
    return st_printf_init;
}

enum printf_state U_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value) {
    *int_value = va_arg(args, int);
    (*out_printed_chars) += print_int(*int_value, 10, digit);
    return st_printf_init;
}

enum printf_state x_small_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value) {
    *int_value = va_arg(args, int);
    (*out_printed_chars) += print_int(*int_value, 16, digit);
    return st_printf_init;
}

enum printf_state X_large_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value) {
    *int_value = va_arg(args, int);
    (*out_printed_chars) += print_int(*int_value, 16, DIGIT);
    return st_printf_init;
}

enum printf_state c_state_handler(va_list args, int *out_printed_chars, state_args *state, char *char_value) {
    *char_value = (char) va_arg(args, int);
    putchar(*char_value);
    ++(*out_printed_chars);
    *char_value = 0;
    return st_printf_init;
}

enum printf_state A_state_handler(va_list args, int *out_printed_chars, state_args *state) {
    int i = 0, len = 0;
    char *string_value;
    char **arr_string_value;
    int *arr_value;
    ++state->fs;
    putchar('{');
    if (*state->fs == 's') {
        arr_string_value = va_arg(args, char**);
        len = va_arg(args, int);
        for (; i < len; i++) {
            toy_printf("%s", arr_string_value[i]);
            if(i<len-1){
                putchar(',');
            }
        }
        putchar('}');
        return st_printf_init;
    } else {
        if (*state->fs == 'c') {
            string_value = va_arg(args, char*);
            len = va_arg(args, int);
            for (i = 0; i < len; i++) {
                toy_printf("%c, ", string_value[i]);
                if(i<len-1) {
                    putchar(',');
                }
            }
            putchar('}');
            return st_printf_init;
        } else {
            arr_value = va_arg(args, int*);
            len = va_arg(args, int);
            switch (*state->fs) {
                case 'd':
                    for (; i < len; i++) {
                        toy_printf("%d", arr_value[i]);
                        if(i<len-1)
                            putchar(',');
                    }
                    break;
                case 'u':
                    for (; i < len; i++) {
                        toy_printf("%u", arr_value[i]);
                        if(i<len-1)
                            putchar(',');
                    }
                    break;
                case 'b':
                    for (; i < len; i++) {
                        toy_printf("%b", arr_value[i]);
                        if(i<len-1)
                            putchar(',');
                    }
                    break;
                case 'o':
                    for (; i < len; i++) {
                        toy_printf("%o", arr_value[i]);
                        if(i<len-1)
                            putchar(',');
                    }
                    break;
                case 'x':
                    for (; i < len; i++) {
                        toy_printf("%x", arr_value[i]);
                        if(i<len-1)
                            putchar(',');
                    }
                    break;
                case 'X':
                    for (; i < len; i++) {
                        toy_printf("%X", arr_value[i]);
                        if(i<len-1)
                            putchar(',');
                    }
                    break;

                default:
                    break;
            }
//            for (; i < len; i++) {
//                        s='%'+*(state->fs);
//                        toy_printf(s, arr_value[i]);
//                        if(i<len-1)
//                            putchar(',');
//                    }
            putchar('}');
            return st_printf_init;
        }

    }
}

void minus_state_handler(int *neg) {
    *neg = 1;
}

void zero_state_handler(int *zero, int *padd) {
    if (*padd == 0)
        *zero = 1;
    (*padd) = (*padd) * 10;
}

void one_state_handler(int *padd) {
    *padd = (*padd) * 10 + 1;
}

void two_state_handler(int *padd) {
    *padd = (*padd) * 10 + 2;
}

void three_state_handler(int *padd) {
    *padd = (*padd) * 10 + 3;
}

void four_state_handler(int *padd) {
    *padd = (*padd) * 10 + 4;
}

void five_state_handler(int *padd) {
    *padd = (*padd) * 10 + 5;
}

void six_state_handler(int *padd) {
    *padd = (*padd) * 10 + 6;
}

void seven_state_handler(int *padd) {
    *padd = (*padd) * 10 + 7;
}

void eight_state_handler(int *padd) {
    *padd = (*padd) * 10 + 8;
}

void nine_state_handler(int *padd) {
    *padd = (*padd) * 10 + 9;
}




enum printf_state
printf_percent_state_handler(va_list args, int *out_printed_chars, state_args *state, int *neg, int *padd, int *zero) {

    int int_value = 0;
    char *string_value="";

    char char_value;


    int nDigits;
    switch (*(state->fs)) {
        case '%':
            return precent_state_handler(args, out_printed_chars, state);
            break;
        case 'd':
            return D_state_handler(args, out_printed_chars, state, &int_value, &nDigits, zero, padd, neg);
            break;

        case 'b':
            return B_state_handler(args, out_printed_chars, state, &int_value);
            break;

        case 'o':
            return O_state_handler(args, out_printed_chars, state, &int_value);
            break;

        case 'x':
            return x_small_state_handler(args, out_printed_chars, state, &int_value);

            break;

        case 'X':
            return X_large_state_handler(args, out_printed_chars, state, &int_value);
            break;

        case 's':
            return S_state_handler(args, out_printed_chars, state, string_value, &nDigits, zero, padd, neg);
            break;

        case 'c':
            return c_state_handler(args, out_printed_chars, state, &char_value);
            break;

        case 'u':
            return U_state_handler(args, out_printed_chars, state, &int_value);
            break;

        case 'A':
            return A_state_handler(args,out_printed_chars, state);
            break;
        case '-':
            minus_state_handler(neg);
            return st_printf_percent;
            break;
        case '0':
            zero_state_handler(zero, padd);
            return st_printf_percent;
            break;
        case '1':
            one_state_handler(padd);
            return st_printf_percent;

            break;
        case '2':
            two_state_handler(padd);
            return st_printf_percent;

            break;
        case '3':
            three_state_handler(padd);
            return st_printf_percent;

            break;
        case '4':
            four_state_handler(padd);
            return st_printf_percent;

            break;
        case '5':
            five_state_handler(padd);
            return st_printf_percent;

            return st_printf_percent;

            break;
        case '6':
            six_state_handler(padd);
            return st_printf_percent;

            break;
        case '7':
            seven_state_handler(padd);
            return st_printf_percent;

            break;
        case '8':
            eight_state_handler(padd);
            return st_printf_percent;

            break;
        case '9':
            nine_state_handler(padd);
            return st_printf_percent;

            break;

        default:
            toy_printf("Unhandled format %%%c...\n", state->fs);
            exit(-1);
    }
}

/* SUPPORTED:
 *   %b, %d, %o, %x, %X --
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
    int padd = 0, zero = 0, neg = 0;
    int chars_printed = 0;
    va_list args;
    state_args state;
    state.fs = fs;
    va_start(args, state.fs);

    state.state = st_printf_init;

    for (; *state.fs != '\0'; ++state.fs) {
        switch (state.state) {
            case st_printf_init:
                state.state = init_state_handler(args, &chars_printed, &state);
//                chars_printed += handler_printed_chars;
                break;

            case st_printf_percent:
                state.state = printf_percent_state_handler(args, &chars_printed, &state, &neg, &padd, &zero);
                break;

            default:
                toy_printf("toy_printf: Unknown state -- %d\n", (int) state.state);
                exit(-1);
        }
    }

    va_end(args);

    return chars_printed;
}

int numDigits(const int n) {
    if (n < 0)
        return numDigits(n * (-1));
    else if (n < 10) return 1;
    return 1 + numDigits(n / 10);
}
