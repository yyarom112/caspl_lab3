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
} state_result;

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')


int toy_printf(char *fs, ...);

int numDigits(const int n);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

state_result
(*func_machine_state_arr[3])(va_list args, int *out_printed_chars, state_args *state, int *neg, int *padd, int *zero);

state_result (*func_precent_state_arr[128])(va_list args, int *out_printed_chars, state_args *state, int *int_value,
                                            char *string_value, int *nDigits, int *zero, int *padd, int *neg);

state_result
(*func_A_state_arr[128])(va_list args, char **arr_string_value, char char_value, int *arr_value, char *string_value);

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

state_result
precent_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                      int *nDigits, int *zero,
                      int *padd, int *neg) {
    state_result res;
    res.next_state = st_printf_percent;
    res.printed_chars = 0;
    return res;
}

state_result
init_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                   int *nDigits, int *zero,
                   int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    switch (*(state->fs)) {
        case '%':
            res = precent_state_handler(args, out_printed_chars, state, int_value, string_value, nDigits, zero, padd,
                                        neg);
            return res;

        default:
            putchar(*(state->fs));
            res.printed_chars += 1;
    }
    res.next_state = st_printf_init;
    return res;
}

state_result
D_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                int *nDigits, int *zero,
                int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
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
            (*padd)--;
        }
        putchar('#');
        res.printed_chars++;
    }
    res.next_state = st_printf_init;
    return res;
}

state_result
S_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                int *nDigits, int *zero,
                int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    string_value = va_arg(args, char *);
    if (padd > 0)
        for (*nDigits = 0; string_value[*nDigits] != '\0'; (*nDigits)++);
    if (*neg == 1) {
        while (*padd - *nDigits > 0) {
            putchar(' ');
            res.printed_chars++;
            (*padd)--;
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
            (*padd)--;
        }
        putchar('#');
        res.printed_chars++;
    }
    res.next_state = st_printf_init;
    return res;
}

state_result
B_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                int *nDigits, int *zero,
                int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    *int_value = va_arg(args, int);
    res.printed_chars += print_int(*int_value, 2, digit);
    res.next_state = st_printf_init;
    return res;
}

state_result
O_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                int *nDigits, int *zero,
                int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    *int_value = va_arg(args, int);
    res.printed_chars += print_int(*int_value, 8, digit);
    res.next_state = st_printf_init;
    return res;
}

state_result
U_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                int *nDigits, int *zero,
                int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    *int_value = va_arg(args, int);
    res.printed_chars += print_int(*int_value, 10, digit);
    res.next_state = st_printf_init;
    return res;
}

state_result
x_small_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                      int *nDigits, int *zero,
                      int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    *int_value = va_arg(args, int);
    res.printed_chars += print_int(*int_value, 16, digit);
    res.next_state = st_printf_init;
    return res;
}

state_result
X_large_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                      int *nDigits, int *zero,
                      int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    *int_value = va_arg(args, int);
    res.printed_chars += print_int(*int_value, 16, DIGIT);
    res.next_state = st_printf_init;
    return res;
}

state_result
c_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                int *nDigits, int *zero,
                int *padd, int *neg) {

    state_result res;
    res.printed_chars = 0;
    char char_value = (char) va_arg(args, int);
    putchar(char_value);
    res.printed_chars += 1;
//    char_value = 0;
    res.next_state = st_printf_init;
    return res;
}

state_result DA_arr_state(va_list args, char **arr_string_value, char char_value, int *arr_value, char *string_value) {
    arr_value = va_arg(args, int*);
    int len = va_arg(args, int);
    int i = 0;
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_init;
    for (; i < len; i++) {
        res.printed_chars += toy_printf("%d", arr_value[i]);
        if (i < len - 1) {
            putchar(',');
            res.printed_chars++;
        }
    }
}

state_result BA_arr_state(va_list args, char **arr_string_value, char char_value, int *arr_value, char *string_value) {
    arr_value = va_arg(args, int*);
    int len = va_arg(args, int);
    int i = 0;
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_init;
    for (; i < len; i++) {
        res.printed_chars += toy_printf("%b", arr_value[i]);
        if (i < len - 1) {
            putchar(',');
            res.printed_chars++;
        }
    }
}

state_result OA_arr_state(va_list args, char **arr_string_value, char char_value, int *arr_value, char *string_value) {
    arr_value = va_arg(args, int*);
    int len = va_arg(args, int);
    int i = 0;
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_init;
    for (; i < len; i++) {
        res.printed_chars += toy_printf("%o", arr_value[i]);
        if (i < len - 1) {
            putchar(',');
            res.printed_chars++;
        }
    }
}

state_result
xA_small_arr_state(va_list args, char **arr_string_value, char char_value, int *arr_value, char *string_value) {
    arr_value = va_arg(args, int*);
    int len = va_arg(args, int);
    int i = 0;
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_init;
    for (; i < len; i++) {
        res.printed_chars += toy_printf("%x", arr_value[i]);
        if (i < len - 1) {
            putchar(',');
            res.printed_chars++;
        }
    }
}

state_result
XA_large_arr_state(va_list args, char **arr_string_value, char char_value, int *arr_value, char *string_value) {
    arr_value = va_arg(args, int*);
    int len = va_arg(args, int);
    int i = 0;
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_init;
    for (; i < len; i++) {
        res.printed_chars += toy_printf("%X", arr_value[i]);
        if (i < len - 1) {
            putchar(',');
            res.printed_chars++;
        }
    }
}

state_result UA_arr_state(va_list args, char **arr_string_value, char char_value, int *arr_value, char *string_value) {
    arr_value = va_arg(args, int*);
    int len = va_arg(args, int);
    int i = 0;
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_init;
    for (; i < len; i++) {
        res.printed_chars += toy_printf("%u", arr_value[i]);
        if (i < len - 1) {
            putchar(',');
            res.printed_chars++;
        }
    }
}

state_result SA_arr_state(va_list args, char **arr_string_value, char char_value, int *arr_value, char *string_value) {
    arr_string_value = va_arg(args, char**);
    int len = va_arg(args, int);
    int i = 0;
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_init;
    for (; i < len; i++) {
        res.printed_chars += toy_printf("%s", arr_string_value[i]);
        if (i < len - 1) {
            putchar(',');
            res.printed_chars++;
        }
    }
    return res;
}

state_result CA_arr_state(va_list args, char **arr_string_value, char char_value, int *arr_value, char *string_value) {
    string_value = va_arg(args, char*);
    int len = va_arg(args, int);
    int i = 0;
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_init;
    for (; i < len; i++) {
        res.printed_chars += toy_printf("%c", string_value[i]);
        if (i < len - 1) {
            putchar(',');
            res.printed_chars++;
        }
    }
    return res;
}

state_result
A_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                int *nDigits, int *zero,
                int *padd, int *neg) {
    state_result res, tmp;
    res.printed_chars = 0;
    int i = 0, len = 0;
    char **arr_string_value;
    char char_value;
    int *arr_value;
    char **s;
    ++state->fs;
    putchar('{');
    res.printed_chars++;
    tmp = func_A_state_arr[(*state->fs)](args, arr_string_value, char_value, arr_value, string_value);
    res.next_state = tmp.next_state;
    res.printed_chars += tmp.printed_chars;
    putchar('}');
    res.printed_chars++;
    return res;
}

state_result
minus_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                    int *nDigits, int *zero,
                    int *padd, int *neg) {
    *neg = 1;
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_percent;
    return res;
}

state_result
zero_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                   int *nDigits, int *zero,
                   int *padd, int *neg) {
    state_result res;
    res.next_state = st_printf_percent;
    res.printed_chars = 0;
    if (*padd == 0)
        *zero = 1;
    (*padd) = (*padd) * 10;
    return res;
}

state_result
one_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                  int *nDigits, int *zero,
                  int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_percent;
    *padd = (*padd) * 10 + 1;
    return res;
}

state_result
two_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                  int *nDigits, int *zero,
                  int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_percent;
    *padd = (*padd) * 10 + 2;
    return res;
}

state_result
three_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                    int *nDigits, int *zero,
                    int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_percent;
    *padd = (*padd) * 10 + 3;
    return res;
}

state_result
four_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                   int *nDigits, int *zero,
                   int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_percent;
    *padd = (*padd) * 10 + 4;
    return res;
}

state_result
five_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                   int *nDigits, int *zero,
                   int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_percent;
    *padd = (*padd) * 10 + 5;
    return res;
}

state_result
six_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                  int *nDigits, int *zero,
                  int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_percent;
    *padd = (*padd) * 10 + 6;
    return res;
}

state_result
seven_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                    int *nDigits, int *zero,
                    int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_percent;
    *padd = (*padd) * 10 + 7;
    return res;
}

state_result
eigth_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                    int *nDigits, int *zero,
                    int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_percent;
    *padd = (*padd) * 10 + 8;
    return res;
}

state_result
nine_state_handler(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                   int *nDigits, int *zero,
                   int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_percent;
    *padd = (*padd) * 10 + 9;
    return res;
}

state_result
error_func_precent(va_list args, int *out_printed_chars, state_args *state, int *int_value, char *string_value,
                   int *nDigits, int *zero,
                   int *padd, int *neg) {
    state_result res;
    res.printed_chars = 0;
    res.next_state = st_printf_error;
}


state_result
printf_percent_state_handler(va_list args, int *out_printed_chars, state_args *state, int *neg, int *padd,
                             int *zero) {
    int len = 0, i = 0;

    int int_value = 0;
    char *string_value;
    int nDigits;
    return func_precent_state_arr[*(state->fs)](args, out_printed_chars, state, &int_value, string_value,
                                                &nDigits, zero, padd, neg);
}

/* SUPPORTED:
 *   %b, %d, %o, %x, %X --
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */
state_result
error_state_handler(va_list args, int *out_printed_chars, state_args *state, int *neg, int *padd, int *zero) {
    toy_printf("toy_printf: Unknown state -- %d\n", (int) state->state);
    exit(-1);
}


int toy_printf(char *fs, ...) {
    int padd = 0, zero = 0, neg = 0, i = 0;
    int chars_printed = 0;
    va_list args;
    state_args state;
    state.fs = fs;
    va_start(args, state.fs);
    state_result res;
    func_machine_state_arr[0] = init_state_handler;
    func_machine_state_arr[1] = printf_percent_state_handler;
    func_machine_state_arr[2] = error_state_handler;

    for (i = 0; i < 128; i++) {
        func_precent_state_arr[i] = error_func_precent;
        func_A_state_arr[i] = error_func_precent;
    }
    func_precent_state_arr[48] = zero_state_handler;
    func_precent_state_arr[49] = one_state_handler;
    func_precent_state_arr[50] = two_state_handler;
    func_precent_state_arr[51] = three_state_handler;
    func_precent_state_arr[52] = four_state_handler;
    func_precent_state_arr[53] = five_state_handler;
    func_precent_state_arr[54] = six_state_handler;
    func_precent_state_arr[55] = seven_state_handler;
    func_precent_state_arr[56] = eigth_state_handler;
    func_precent_state_arr[57] = nine_state_handler;
    func_precent_state_arr[37] = precent_state_handler;
    func_precent_state_arr[100] = D_state_handler;
    func_precent_state_arr[98] = B_state_handler;
    func_precent_state_arr[111] = O_state_handler;
    func_precent_state_arr[120] = x_small_state_handler;
    func_precent_state_arr[88] = X_large_state_handler;
    func_precent_state_arr[115] = S_state_handler;
    func_precent_state_arr[99] = c_state_handler;
    func_precent_state_arr[117] = U_state_handler;
    func_precent_state_arr[65] = A_state_handler;
    func_precent_state_arr[45] = minus_state_handler;


    func_A_state_arr[88] = XA_large_arr_state;
    func_A_state_arr[98] = BA_arr_state;
    func_A_state_arr[99] = CA_arr_state;
    func_A_state_arr[100] = DA_arr_state;
    func_A_state_arr[111] = OA_arr_state;
    func_A_state_arr[115] = SA_arr_state;
    func_A_state_arr[117] = UA_arr_state;
    func_A_state_arr[120] = xA_small_arr_state;


    state.state = st_printf_init;
    for (; *state.fs != '\0'; ++state.fs) {
        res = func_machine_state_arr[state.state](args, &chars_printed, &state, &neg, &padd, &zero);
        chars_printed += res.printed_chars;
        state.state = res.next_state;
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