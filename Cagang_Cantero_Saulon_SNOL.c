#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum {
    TOKEN_KEYWORD,
    TOKEN_VARIABLE,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_OPERATOR,
    TOKEN_EQUALS,
    TOKEN_ERROR
} Token_Type;

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_NONE
} SNOL_Type;

typedef struct {
    char name[100];
    double value;
    SNOL_Type type;
} Variable;

typedef struct {
    char value[100];
    Token_Type type;
} Token;

int compare_strings(const char *str1, const char *str2);
int lexical_analysis(char *command, Token tokens[]);
void syntax_analysis(Token tokens[], int token_count, Variable symbolTable[], int *varCount);
int search_for_variable(char *name, Variable symbolTable[], int varCount);
void execute_BEG_command(char *varName, Variable symbolTable[], int *varCount);
void execute_PRINT_command(Token outToken, Variable symbolTable[], int varCount);

int main() {
    Variable symbol_table[100];
    int variable_count = 0;

    Token tokens[50];
    int token_count = 0;

    printf("The SNOL environment is now active, you may proceed with giving your commands.\n");
    char command[256];

    do {
        printf("\nCommand: ");
        if(fgets(command, sizeof(command), stdin)) {
            command[strcspn(command, "\n")] = '\0';
        }
        
        token_count = lexical_analysis(command, tokens);

        if (token_count > 0 && compare_strings(command, "EXIT!") == 0) {
            break;
        }

        if (token_count > 0) {
            syntax_analysis(tokens, token_count, symbol_table, &variable_count);
        }

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

int lexical_analysis(char *command, Token tokens[]) {
    int i = 0;
    int token_count = 0;

    while(isspace(command[i])) {
            i++;
        }

    if (strncmp(&command[i], "EXIT!", 5) == 0) {
        strcpy(tokens[token_count].value, "EXIT!");
        tokens[token_count].type = TOKEN_KEYWORD;
        return 1;
    } else if (strncmp(&command[i], "PRINT", 5) == 0 && (isspace(command[i + 5]) || command[i + 5] == '\0')) {
        strcpy(tokens[token_count].value, "PRINT");
        tokens[token_count].type = TOKEN_KEYWORD;
        token_count++;
        i += 5;
    } else if (strncmp(&command[i], "BEG", 3) == 0 && (isspace(command[i + 3]) || command[i + 3] == '\0')) {
        strcpy(tokens[token_count].value, "BEG");
        tokens[token_count].type = TOKEN_KEYWORD;
        token_count++;
        i += 3;
    } else {
        
    }

    while (command[i] != '\0') {

        if (isspace(command[i])) {
            i++;
            continue;
        }

        if (command[i] == '+' || command[i] == '*' || command[i] == '/' || command[i] == '%' || command[i] == '=') {
            
            if (command[i] == '-' && isdigit(command[i + 1])) {

            } else {
                tokens[token_count].value[0] = command[i];
                tokens[token_count].value[1] = '\0';
                tokens[token_count].type = (command[i] == '=') ? TOKEN_EQUALS : TOKEN_OPERATOR;
                token_count++;
                i++;
                continue;
            }
        } 
        
        if (isalpha(command[i])) {
            int j = 0;

            while(isalnum(command[i])) {
                tokens[token_count].value[j++] = command[i++];
            }
            tokens[token_count].value[j] = '\0';
            tokens[token_count].type = TOKEN_VARIABLE;
            token_count++;
        } else 
        
        if (isdigit(command[i]) || command[i] == '.' || command[i] == '-') {
            int k = 0;
            int dots_counter = 0;
            if (command[i] == '-' && !isdigit(command[i + 1]) && command[i + 1] != '.') {
                printf("Operator: %c\n", command[i]);
                i++;
                continue;
            }

            while (isdigit(command[i]) || command[i] == '.') {
                if (command[i] == '.') {
                    dots_counter++;
                }
                tokens[token_count].value[k++] = command[i++];
            }
            tokens[token_count].value[k] = '\0';
            tokens[token_count].type = (dots_counter == 1) ? TOKEN_FLOAT : TOKEN_INTEGER;
            token_count++;

            // if (dots_counter > 1) {
            //     printf("Error! Invalid number format: %s\n", );
            // } else if (number_buffer[0] == '.' || (number_buffer[0] == '-' && number_buffer[1] == '.')) {
            //     printf("Error! Invalid number format. Missing leading digit.\n");
            // } else if (dots_counter == 1) {
            //     printf("Number (float): %s\n", number_buffer);
            // } else {
            //     printf("Number (Integer): %s\n", number_buffer);
            // }
        } 
        
        else {
            char unknown_word_buffer[100];
            int l = 0;
            while (command[i] != '\0' && !isspace(command[i]) && !isalnum(command[i]) && !strchr("+-*/%=", command[i])) {
                if (l < 99) {
                    unknown_word_buffer[l++] = command[i++];
                } else {
                    i++;
                }
            }
            unknown_word_buffer[l] = '\0';
            printf("Error! Unknown word: [%s]\n", unknown_word_buffer);
            i++;
        }
    }
    return token_count;
}

void syntax_analysis(Token tokens[], int token_count, Variable symbolTable[], int *varCount) {
    if (token_count == 0) {
        return;
    }

    if (tokens[0].type == TOKEN_KEYWORD && strcmp(tokens[0].value, "BEG") == 0) {
        if (token_count == 2 && tokens[1].type == TOKEN_VARIABLE) {
            execute_BEG_command(tokens[1].value, symbolTable, varCount);
        } else {
            printf("SNOL> Unknown command! Does not match any valid command of the language.\n");
        }
    } else

    if (tokens[0].type == TOKEN_KEYWORD && strcmp(tokens[0].value, "PRINT") == 0) {
        if (token_count == 2 && (tokens[1].type == TOKEN_VARIABLE || tokens[1].type == TOKEN_INTEGER || tokens[1].type == TOKEN_FLOAT)) {
            execute_PRINT_command(tokens[1], symbolTable, *varCount);
        } else {
            printf("SNOL> Unknown command! Does not match any valid command of the language.\n");
        }
    } else 

    if (token_count >= 3 && tokens[0].type == TOKEN_VARIABLE && tokens[1].type == TOKEN_EQUALS) {
        char *destinationName = tokens[0].value;
        double resultValue;
        SNOL_Type resultType;

        if (token_count == 3) {
            resultValue = atof(tokens[2].value);
            resultType = (tokens[2].type == TOKEN_FLOAT) ? TYPE_FLOAT : TYPE_INT;

            int i = search_for_variable(destinationName, symbolTable, *varCount);
            if (i == -1) {
                strcpy(symbolTable[*varCount].name, destinationName);
                symbolTable[*varCount].value = resultValue;
                symbolTable[*varCount].type = resultType;
                (*varCount)++;
            } else {
                symbolTable[i].value = resultValue;
                symbolTable[i].type = resultType;
            }
        }
    } else

    if (token_count == 3 && tokens[1].type == TOKEN_OPERATOR) {
        printf("Expression detected");
    }

    else {
        printf("SNOL> Unknown command! Does not match any valid command of the language.\n");
    }
}

int search_for_variable(char *name, Variable symbolTable[], int varCount) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void execute_BEG_command(char *varName, Variable symbolTable[], int *varCount) {
    printf("SNOL> Please enter value for [%s]\n", varName);

    char input[50];
    printf("Input: ");
    fgets(input, sizeof(input), stdin);

    int index = search_for_variable(varName, symbolTable, *varCount);

    if (index == -1) {
        strcpy(symbolTable[*varCount].name, varName);
        symbolTable[*varCount].type = (strchr(input, '.') != NULL) ? TYPE_FLOAT : TYPE_INT;
        symbolTable[*varCount].value = atof(input);
        (*varCount)++;
        // printf("(New variable [%s] stored)\n", varName);
    } else {
        symbolTable[index].value = atof(input);
        symbolTable[index].type = (strchr(input, '.') != NULL) ? TYPE_FLOAT : TYPE_INT;
        // printf("(Variable [%s] updated)\n", varName);
    }
}

void execute_PRINT_command(Token outToken, Variable symbolTable[], int varCount) {
    if (outToken.type == TOKEN_VARIABLE) {
        int index = search_for_variable(outToken.value, symbolTable, varCount);
        if (index != -1) {
            printf("SNOL> [%s] = %g\n", symbolTable[index].name, symbolTable[index].value);
        } else {
            printf("SNOL> Error! [%s] is not defined!\n", outToken.value);
        }
    } else {
        printf("SNOL> %s\n", outToken.value);
    }
}