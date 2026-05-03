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
    printf("\n== Tokens ==\n");
    while (command[i] != '\0') {
        
        if (isspace(command[i])) {
            i++;
            continue;
        }

        if (command[i] == '+' || command[i] == '-' || command[i] == '*' || command[i] == '/' || command[i] == '%') {
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
            int k = 0;

            while(isdigit(command[i]) || command[i] == '.') {
                number_buffer[k++] = command[i++];
            }
            
            number_buffer[k] = '\0';
            printf("Number: %s\n", number_buffer);
            i++;
        } 
        
        else {
            printf("Invalid character: %c\n", command[i]);
            i++;
        }
    }
}