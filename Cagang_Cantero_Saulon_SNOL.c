#include <stdio.h>
#include <string.h>
#include <ctype.h>

int compare_strings(const char *str1, const char *str2);
void lexical_analysis(char *command);

int main() {

    printf("The SNOL environment is now active, you may proceed with giving your commands.\n");
    char command[100];

    do {
        printf("\nCommand: ");
        if(fgets(command, sizeof(command), stdin)) {
            command[strcspn(command, "\n")] = '\0';
        }
        
        if (compare_strings(command, "EXIT!") == 0) {
            break;
        }

        lexical_analysis(command);

    } while (compare_strings(command, "EXIT!") != 0);
    
    printf("Interpreter is now terminated...");
    return 0;
}

int compare_strings(const char *str1, const char *str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            break;
        }
        i++;
    }
    return (unsigned char)str1[i] - (unsigned char)str2[i];
}

void lexical_analysis(char *command) {
    int i = 0;

    while(isspace(command[i])) {
            i++;
        }

    if (strncmp(&command[i], "EXIT!", 5) == 0) {
        printf("Command: EXIT!\n");
        return;
    } else if (strncmp(&command[i], "PRINT ", 6) == 0) {
        printf("Command: PRINT\n");
        i += 5;
    } else if (strncmp(&command[i], "BEG ", 4) == 0) {
        printf("Command: BEG\n");
        i += 3;
    } else {
        
    }

    printf("\n== Tokens ==\n");
    while (command[i] != '\0') {

        if (isspace(command[i])) {
            i++;
            continue;
        }

        if (command[i] == '+' || command[i] == '-' || command[i] == '*' || command[i] == '/' || command[i] == '%' || command[i] == '=') {
            printf("Operator: %c\n", command[i]);
            i++;
            continue;
        } else 
        
        if (isalpha(command[i])) {
            char variable_buffer[100];
            int j = 0;

            while(isalnum(command[i])) {
                variable_buffer[j++] = command[i++];
            }

            variable_buffer[j] = '\0';
            printf("Variable: %s\n", variable_buffer);
        } else 
        
        if (isdigit(command[i]) || command[i] == '.') {
            char number_buffer[100];
            int dots_counter = 0;
            int k = 0;

            if (command[i] == '-') {
                number_buffer[k++] = command[i++];
            }

            while (isdigit(command[i]) || command[i] == '.') {
                if (command[i] == '.') {
                    dots_counter++;
                }

                if (k < 99) {
                    number_buffer[k++] = command[i++];
                } else {
                    i++;
                }
            }

            number_buffer[k] = '\0';

            if (dots_counter > 1) {
                printf("Invalid number format: %s\n", number_buffer);
            } else if (dots_counter == 1) {
                printf("Number (float): %s\n", number_buffer);
            } else {
                printf("Number (Integer): %s\n", number_buffer);
            }
            i++;
        } 
        
        else {
            printf("Invalid character: %c\n", command[i]);
            i++;
        }
    }
}