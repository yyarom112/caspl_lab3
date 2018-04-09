#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>


/* the states in the printf state-machine */
enum printf_state {
    st_printf_init,
    st_printf_percent,
    st_printf_error,
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

state_result precent_state_handler(va_list args, int *out_printed_chars, state_args *state) {
    state_result res;
    res.next_state=st_printf_percent;
    res.printed_chars=0;
    return res;
}

state_result init_state_handler(va_list args, int *out_printed_chars, state_args *state, int *neg, int *padd, int *zero) {
    state_result res;
    res.printed_chars=0;
    switch (*(state->fs)) {
        case '%':
            res=precent_state_handler(args, out_printed_chars, state);
            return res;

        default:
            putchar(*(state->fs));
            res.printed_chars+=1;
    }
    res.next_state=st_printf_init;
    return res;
}

state_result D_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, int *nDigits, int *zero,
                int *padd, int *neg) {
    state_result res;
    res.printed_chars=0;
    *int_value = va_arg(args, int);
    *nDigits = numDigits(*int_value);
    if (*zero == 1) {
        if (*int_value < 0) {
            putchar('-');
            res.printed_chars++;
            (*padd)--;
            *int_value = -*int_value;
        }
        while ((*padd) - (*nDigits) > 0) {
            putchar('0');
            res.printed_chars++;
            (*padd)--;
        }
    }
    if (*neg == 1) {
        while (*padd - *nDigits > 0) {
            putchar(' ');
            res.printed_chars++;
            (*padd)--;
        }
    }
    if (*int_value < 0) {
        putchar('-');
        *int_value = -*int_value;
    }
    res.printed_chars += print_int((*int_value), 10, digit);
    if (*neg == 0 && (*padd) - (*nDigits) > 0) {
        while (*padd - *nDigits > 0) {
            putchar(' ');
            res.printed_chars++;
            (*padd--);
        }
        putchar('#');
        res.printed_chars++;
    }
    res.next_state= st_printf_init;
    return res;
}

state_result S_state_handler(va_list args, int *out_printed_chars, state_args *state, char *string_value, int *nDigits, int *zero,
                int *padd, int *neg) {
    state_result res;
    res.printed_chars=0;
    string_value = va_arg(args, char *);
    if (padd > 0)
        for (*nDigits = 0; string_value[*nDigits] != '\0'; (*nDigits)++);
    if (*neg == 1) {
        while (*padd - *nDigits > 0) {
            putchar(' ');
            res.printed_chars++;
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
            res.printed_chars++;
            padd--;
        }
        putchar('#');
        res.printed_chars++;
    }
    res.next_state=st_printf_init;
    return res;
}

state_result B_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value) {
    state_result res;
    res.printed_chars=0;
    *int_value = va_arg(args, int);
    res.printed_chars += print_int(*int_value, 2, digit);
    res.next_state=st_printf_init;
    return res;
}

state_result O_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value) {
    state_result res;
    res.printed_chars=0;
    *int_value = va_arg(args, int);
    res.printed_chars += print_int(*int_value, 8, digit);
    res.next_state=st_printf_init;
    return res;
}

state_result U_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value) {
    state_result res;
    res.printed_chars=0;
    *int_value = va_arg(args, int);
    res.printed_chars += print_int(*int_value, 10, digit);
    res.next_state=st_printf_init;
    return res;
}

state_result x_small_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value) {
    state_result res;
    res.printed_chars=0;
    *int_value = va_arg(args, int);
    res.printed_chars += print_int(*int_value, 16, digit);
    res.next_state=st_printf_init;
    return res;
}

state_result X_large_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value) {
    state_result res;
    res.printed_chars=0;
    *int_value = va_arg(args, int);
    res.printed_chars += print_int(*int_value, 16, DIGIT);
    res.next_state=st_printf_init;
    return res;
}

state_result c_state_handler(va_list args, int *out_printed_chars, state_args *state, char *char_value) {
    state_result res;
    res.printed_chars=0;
    *char_value = (char) va_arg(args, int);
    putchar(*char_value);
    res.printed_chars+=1;
    *char_value = 0;
    res.next_state= st_printf_init;
    return res;
}

state_result A_state_handler(va_list args, int *out_printed_chars, state_args *state) {
    state_result res;
    res.printed_chars=0;
    int i = 0, len = 0;
    char *string_value;
    char **arr_string_value;
    char char_value;
    int *arr_value;
    char** s;
    ++state->fs;
    putchar('{');
    res.printed_chars++;
    if (*state->fs == 's') {
        arr_string_value = va_arg(args, char**);
        len = va_arg(args, int);
        for (; i < len; i++) {
            res.printed_chars+=toy_printf("%s", arr_string_value[i]);
            if(i<len-1){
                putchar(',');
                res.printed_chars++;
            }
        }
        putchar('}');
        res.printed_chars++;
        res.next_state= st_printf_init;
        return res;
    } else {
        if (*state->fs == 'c') {
            string_value = va_arg(args, char*);
            len = va_arg(args, int);
            for (i = 0; i < len; i++) {
                res.printed_chars+=toy_printf("%c, ", string_value[i]);
                if(i<len-1) {
                    putchar(',');
                    res.printed_chars++;
                }
            }
            putchar('}');
            res.printed_chars++;
            res.next_state= st_printf_init;
            return res;
        } else {
            arr_value = va_arg(args, int*);
            len = va_arg(args, int);
            switch (*state->fs) {
                case 'd':
                    for (; i < len; i++) {
                        res.printed_chars+=toy_printf("%d", arr_value[i]);
                        if(i<len-1) {
                            putchar(',');
                            res.printed_chars++;
                        }
                    }
                    break;
                case 'u':
                    for (; i < len; i++) {
                        res.printed_chars+=toy_printf("%u", arr_value[i]);
                        if(i<len-1) {
                            putchar(',');
                            res.printed_chars++;
                        }
                    }
                    break;
                case 'b':
                    for (; i < len; i++) {
                        res.printed_chars+=toy_printf("%b", arr_value[i]);
                        if(i<len-1) {
                            putchar(',');
                            res.printed_chars++;
                        }
                    }
                    break;
                case 'o':
                    for (; i < len; i++) {
                        res.printed_chars+=toy_printf("%o", arr_value[i]);
                        if(i<len-1) {
                            putchar(',');
                            res.printed_chars++;
                        }
                    }
                    break;
                case 'x':
                    for (; i < len; i++) {
                        res.printed_chars+=toy_printf("%x", arr_value[i]);
                        if(i<len-1) {
                            putchar(',');
                            res.printed_chars++;
                        }
                    }
                    break;
                case 'X':
                    for (; i < len; i++) {
                        res.printed_chars+=toy_printf("%X", arr_value[i]);
                        if(i<len-1) {
                            putchar(',');
                            res.printed_chars++;
                        }
                    }
                    break;

                default:
                    break;
            }
            putchar('}');
            res.printed_chars++;
            res.next_state= st_printf_init;
            return res;
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




state_result printf_percent_state_handler(va_list args, int *out_printed_chars, state_args *state, int *neg, int *padd, int *zero) {
    int chars_printed = 0;
    int int_value = 0;
    char *string_value;
    char **arr_string_value;
    char char_value;
    int *arr_value;
    int len = 0, i = 0;
    int nDigits;
    state_result res;
    switch (*(state->fs)) {
        case '%':
            return precent_state_handler(args, out_printed_chars, state);
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
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;
            break;
        case '0':
            zero_state_handler(zero, padd);
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;            break;
        case '1':
            one_state_handler(padd);
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;
            break;
        case '2':
            two_state_handler(padd);
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;
            break;
        case '3':
            three_state_handler(padd);
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;
            break;
        case '4':
            four_state_handler(padd);
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;
            break;
        case '5':
            five_state_handler(padd);
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;

            break;
        case '6':
            six_state_handler(padd);
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;
            break;
        case '7':
            seven_state_handler(padd);
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;
            break;
        case '8':
            eight_state_handler(padd);
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;
            break;
        case '9':
            nine_state_handler(padd);
            res.next_state=st_printf_percent;
            res.printed_chars=0;
            return res;
            break;

        default:
            res.next_state=st_printf_error;
            return res;
    }
}

/* SUPPORTED:
 *   %b, %d, %o, %x, %X --
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */
state_result error_state_handler(va_list args, int *out_printed_chars, state_args *state, int *neg, int *padd, int *zero) {
    toy_printf("toy_printf: Unknown state -- %d\n", (int) state->state);
    exit(-1);
}

state_result (*func_machine_state_arr[3])(va_list args, int *out_printed_chars, state_args *state, int *neg, int *padd, int *zero);

int toy_printf(char *fs, ...) {
    int padd = 0, zero = 0, neg = 0;
    int chars_printed = 0;
    va_list args;
    state_args state;
    state.fs = fs;
    va_start(args, state.fs);
    state_result res;
    func_machine_state_arr[0]=init_state_handler;
    func_machine_state_arr[1]=printf_percent_state_handler;
    func_machine_state_arr[2]=error_state_handler;
    state.state=st_printf_init;
    for (; *state.fs != '\0'; ++state.fs) {
        res=func_machine_state_arr[state.state](args, &chars_printed, &state,&neg, &padd, &zero);
        chars_printed+=res.printed_chars;
        state.state=res.next_state;
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
