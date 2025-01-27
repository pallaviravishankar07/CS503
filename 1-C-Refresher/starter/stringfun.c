#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SZ 50

// prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);
int count_words(char *, int, int);
void reverse_string(char *, int);
void print_words(char *, int);
void replace_substring(char *, int, const char *, const char *);

int setup_buff(char *buff, char *user_str, int len) {
    char *src = user_str;
    char *dst = buff;
    int space_added = 0;
    int char_count = 0;

    while (*src != '\0') {
        if (*src == ' ' || *src == '\t') {
            if (!space_added && char_count < len - 1) {
                *dst++ = ' ';
                char_count++;
                space_added = 1;
            }
        } else {
            if (char_count < len - 1) {
                *dst++ = *src;
                char_count++;
                space_added = 0;
            } else {
                return -1; // Input string too large
            }
        }
        src++;
    }

    // Remove trailing space if any
    if (space_added && dst != buff) {
        dst--;
        char_count--;
    }

    // Fill remaining buffer with '.'
    while (char_count < len) {
        *dst++ = '.';
        char_count++;
    }

    return char_count;
}

void print_buff(char *buff, int len) {
    printf("Buffer:  ");
    for (int i = 0; i < len; i++) {
        putchar(*(buff + i));
    }
    putchar('\n');
}

void usage(char *exename) {
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int count_words(char *buff, int len, int str_len) {
    int words = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        if (*(buff + i) != ' ' && *(buff + i) != '.') {
            if (!in_word) {
                words++;
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
    }

    return words;
}

void reverse_string(char *buff, int str_len) {
    char *start = buff;
    char *end = buff + str_len - 1;
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

void print_words(char *buff, int str_len) {
    printf("Word Print\n----------\n");

    char *start = buff;
    int word_count = 0;
    while (*start != '.' && start < buff + str_len) {
        if (*start != ' ') {
            char *word_start = start;
            int length = 0;
            while (*start != ' ' && *start != '.' && start < buff + str_len) {
                length++;
                start++;
            }
            printf("%d. ", ++word_count);
            fwrite(word_start, sizeof(char), length, stdout);
            printf(" (%d)\n", length);
        } else {
            start++;
        }
    }
}

void replace_substring(char *buff, int buff_len, const char *find, const char *replace) {
    char temp[BUFFER_SZ];
    char *pos, *start = buff;
    int find_len = strlen(find);
    int replace_len = strlen(replace);
    int temp_idx = 0;

    while ((pos = strstr(start, find)) != NULL) {
        // Copy part before the match
        while (start < pos && temp_idx < BUFFER_SZ - 1) {
            temp[temp_idx++] = *start++;
        }
        // Copy the replacement string
        for (int i = 0; i < replace_len && temp_idx < BUFFER_SZ - 1; i++) {
            temp[temp_idx++] = replace[i];
        }
        start += find_len; // Skip the found string
    }

    // Copy remaining part of the original string
    while (*start != '\0' && temp_idx < BUFFER_SZ - 1) {
        temp[temp_idx++] = *start++;
    }

    // Null-terminate and copy back to buffer
    temp[temp_idx] = '\0';
    strncpy(buff, temp, buff_len);
}

int main(int argc, char *argv[]) {
    char *buff;             // Placeholder for the internal buffer
    char *input_string;     // Holds the string provided by the user on cmd line
    char opt;               // Used to capture user option from cmd line
    int rc;                 // Used for return codes
    int user_str_len;       // Length of user-supplied string

    // Ensure proper usage
    if ((argc < 2) || (*argv[1] != '-')) {
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1] + 1);   // Get the option flag

    if (opt == 'h') {
        usage(argv[0]);
        exit(0);
    }

    if (argc < 3) {
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2];

    // Allocate space for the buffer
    buff = (char *)malloc(BUFFER_SZ);
    if (!buff) {
        printf("Error: Memory allocation failed\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0) {
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt) {
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0) {
                printf("Error counting words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        case 'r':
            reverse_string(buff, user_str_len);
            printf("Reversed String: ");
            print_buff(buff, user_str_len);
            break;

        case 'w':
            print_words(buff, user_str_len);
            break;

        case 'x':
            if (argc < 5) {
                printf("Error: Provide both find and replace strings for -x option\n");
                free(buff);
                exit(1);
            }
            replace_substring(buff, BUFFER_SZ, argv[3], argv[4]);
            printf("Modified String: ");
            print_buff(buff, user_str_len);
            break;

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    print_buff(buff, BUFFER_SZ);
    free(buff);
    exit(0);
}
