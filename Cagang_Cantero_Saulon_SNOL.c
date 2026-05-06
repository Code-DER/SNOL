#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Symbol Tables
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

// Function Prototypes
int compare_strings(const char *str1, const char *str2);
int lexical_analysis(char *command, Token tokens[]);
void syntax_analysis(Token tokens[], int token_count, Variable symbolTable[], int *varCount);
double resolve_value(Token tokens, Variable symbolTable[], int variable_count, SNOL_Type *type, int *error);
double execution_assignment(Token tokens[], Variable symbolTable[], int variable_count, SNOL_Type *resultType, int *error);
int search_for_variable(char *name, Variable symbolTable[], int varCount);
void execute_BEG_command(char *varName, Variable symbolTable[], int *varCount);
void execute_PRINT_command(Token outToken, Variable symbolTable[], int varCount);
void execute_assignment(Token tokens[], int token_count, Variable symbolTable[], int *varCount);

// Main Function
int main() {
    // Initialize the symbol table
    Variable symbol_table[100];
    int variable_count = 0;

    // Initialize the tokens
    Token tokens[50];
    int token_count = 0;

    printf("The SNOL environment is now active, you may proceed with giving your commands.\n");
    char command[256];

    // Loop for the command line
    do {
        printf("\nCommand: ");
        // Get the user input
        if(fgets(command, sizeof(command), stdin)) {
            command[strcspn(command, "\n")] = '\0';
        }
        
        // Count the total tokens though the lexical analysis function
        token_count = lexical_analysis(command, tokens);

        // If "EXIT!" is inputted, program is terminated.
        if (token_count > 0 && compare_strings(command, "EXIT!") == 0) {
            break;
        }

        // If token count is greater than 0, move to the syntax analysis function
        if (token_count > 0) {
            syntax_analysis(tokens, token_count, symbol_table, &variable_count);
        }

    } while (compare_strings(command, "EXIT!") != 0);
    
    printf("Interpreter is now terminated...");
    return 0;
}

// Function to compare strings
int compare_strings(const char *str1, const char *str2) {
    int i = 0;
    // Compare each character in the string
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            break;
        }
        i++;
    }
    // Return their difference
    return (unsigned char)str1[i] - (unsigned char)str2[i];
}

// Lexical analysis functions
int lexical_analysis(char *command, Token tokens[]) {
    int i = 0;
    int token_count = 0;

    // Increase the counter if space is encounterd
    while(isspace(command[i])) {
        i++;
    }

    // Assign "EXIT!", "PRINT", and "BEG" as keywords if seen in the command
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
    }

    // Continue while the loop has not hit the terminator yet
    while (command[i] != '\0') {

        // Continue if spaces are read
        if (isspace(command[i])) {
            i++;
            continue;
        }

        // Read the operators as tokens
        if (strchr("=+*/%", command[i]) || command[i] == '-') {

            // FIX: correct negative number handling
            if (command[i] == '-' && (isdigit(command[i+1]) || command[i+1] == '.')) {
                int k = 0;
                int dots_counter = 0;

                tokens[token_count].value[k++] = command[i++];

                while (isdigit(command[i]) || command[i] == '.') {
                    if (command[i] == '.') dots_counter++;
                    tokens[token_count].value[k++] = command[i++];
                }

                tokens[token_count].value[k] = '\0';
                tokens[token_count].type =
                    (dots_counter == 1) ? TOKEN_FLOAT : TOKEN_INTEGER;

                token_count++;
                continue;
            }

            tokens[token_count].value[0] = command[i];
            tokens[token_count].value[1] = '\0';
            tokens[token_count].type =
                (command[i] == '=') ? TOKEN_EQUALS : TOKEN_OPERATOR;

            token_count++;
            i++;
            continue;
        }

        // Read the words as variables
        if (isalpha(command[i])) {
            int j = 0;

            // Read the variables with numbers
            while(isalnum(command[i])) {
                tokens[token_count].value[j++] = command[i++];
            }
            tokens[token_count].value[j] = '\0';

            // FIX: keyword detection (important)
            if (strcmp(tokens[token_count].value, "PRINT") == 0 ||
                strcmp(tokens[token_count].value, "BEG") == 0 ||
                strcmp(tokens[token_count].value, "EXIT!") == 0) {
                tokens[token_count].type = TOKEN_KEYWORD;
            } else {
                tokens[token_count].type = TOKEN_VARIABLE;
            }

            token_count++;
        }

        // Read numbers
        else if (isdigit(command[i]) || command[i] == '.') {
            int k = 0;
            int dots_counter = 0;

            while (isdigit(command[i]) || command[i] == '.') {
                if (command[i] == '.') {
                    dots_counter++;
                }
                tokens[token_count].value[k++] = command[i++];
            }
            tokens[token_count].value[k] = '\0';
            // Assign token as a float
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
        
        // Identifies unknown words
        else {
            char unknown_word_buffer[100];
            int l = 0;
            // PLace in a buffer to identify the unknown
            while (command[i] != '\0' && !isspace(command[i]) && !isalnum(command[i]) && !strchr("+-*/%=", command[i])) {
                if (l < 99) {
                    unknown_word_buffer[l++] = command[i++];
                } else {
                    i++;
                }
            }

            unknown_word_buffer[l] = '\0';
            printf("Error! Unknown word: [%s]\n", unknown_word_buffer);
        }
    }

    // Return the final token count
    return token_count;
}

// Syntax analysis function
void syntax_analysis(Token tokens[], int token_count, Variable symbolTable[], int *varCount) {

    // Return if there are no tokens
    if (token_count == 0) {
        return;
    }

    // Call the BEG function
    if (tokens[0].type == TOKEN_KEYWORD && strcmp(tokens[0].value, "BEG") == 0) {
        if (token_count == 2 && tokens[1].type == TOKEN_VARIABLE) {
            // Calls to execute the BEG function
            execute_BEG_command(tokens[1].value, symbolTable, varCount);
        } else {
            // Print unknown command if different keyword is inputted
            printf("SNOL> Unknown command! Does not match any valid command of the language.\n");
        }
        return;
    }

    // Call the PRINT function
    if (tokens[0].type == TOKEN_KEYWORD && strcmp(tokens[0].value, "PRINT") == 0) {
        if (token_count == 2) {
            execute_PRINT_command(tokens[1], symbolTable, *varCount);
        } else {
            // Print unknown command if different keyword is inputted
            printf("SNOL> Unknown command! Does not match any valid command of the language.\n");
        }
        return;
    }

    if (token_count >= 3 && tokens[0].type == TOKEN_VARIABLE && tokens[1].type == TOKEN_EQUALS) {
        execute_assignment(tokens, token_count, symbolTable, varCount);
        return;
    }

    if (token_count == 3 && tokens[1].type == TOKEN_OPERATOR) {
        int error = 0;
        SNOL_Type type;
        execution_assignment(tokens, symbolTable, *varCount, &type, &error);
        return;
    }

    // Print unknown command for other types of input
    printf("SNOL> Unknown command! Does not match any valid command of the language.\n");
}

// resolve_value (UNCHANGED STRUCTURE)
double resolve_value(Token tokens, Variable symbolTable[], int variable_count, SNOL_Type *type, int *error) {

    if (tokens.type == TOKEN_INTEGER) {
        *type = TYPE_INT;
        return atof(tokens.value);
    }

    if (tokens.type == TOKEN_FLOAT) {
        *type = TYPE_FLOAT;
        return atof(tokens.value);
    }

    if (tokens.type == TOKEN_VARIABLE) {

        int index = search_for_variable(tokens.value, symbolTable, variable_count);

        if (index == -1) {
            printf("SNOL> Error! [%s] is not defined!\n", tokens.value);
            *error = 1;
            return 0;
        }

        *type = symbolTable[index].type;
        return symbolTable[index].value;
    }

    *error = 1;
    return 0;
}

// execution_assignment (UNCHANGED LOGIC FIX ONLY SAFE)
double execution_assignment(Token tokens[], Variable symbolTable[], int variable_count, SNOL_Type *resultType, int *error) {

    SNOL_Type leftType, rightType;

    double leftValue = resolve_value(tokens[0], symbolTable, variable_count, &leftType, error);
    if (*error) return 0;

    double rightValue = resolve_value(tokens[2], symbolTable, variable_count, &rightType, error);
    if (*error) return 0;

    if (leftType != rightType) {
        printf("SNOL> Error! Operands must be of the same type in an arithmetic operation!\n");
        *error = 1;
        return 0;
    }

    *resultType = leftType;

    switch (tokens[1].value[0]) {
        case '+': return leftValue + rightValue;
        case '-': return leftValue - rightValue;
        case '*': return leftValue * rightValue;
        case '/': return leftValue / rightValue;

        case '%':
            if (leftType != TYPE_INT) {
                printf("SNOL> Error! Modulo operation only allow integer type.\n");
                *error = 1;
                return 0;
            }
            return (int)leftValue % (int)rightValue;
    }

    *error = 1;
    return 0;
}

// Search for a variable function
int search_for_variable(char *name, Variable symbolTable[], int varCount) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Execute the 'BEG' command function
void execute_BEG_command(char *varName, Variable symbolTable[], int *varCount) {
    // Asks for user input
    char input[50];
    printf("SNOL> Please enter value for [%s]\n", varName);
    printf("Input: ");
    // Stores user input in 'input'
    fgets(input, sizeof(input), stdin);

    // Search for the variable in the Symbol Table
    int i = search_for_variable(varName, symbolTable, *varCount);

    if (i == -1) {
        // If the variable is not yet defined, it is stored in the next slot
        strcpy(symbolTable[*varCount].name, varName);
        symbolTable[*varCount].type = (strchr(input, '.') != NULL) ? TYPE_FLOAT : TYPE_INT;
        symbolTable[*varCount].value = atof(input);
        (*varCount)++;
        // printf("(New variable [%s] stored)\n", varName);
    } else {
        // If the variable is defined, its value and type is updated
        symbolTable[i].value = atof(input);
        symbolTable[i].type = (strchr(input, '.') != NULL) ? TYPE_FLOAT : TYPE_INT;
        // printf("(Variable [%s] updated)\n", varName);
    }
}

// Execute the 'PRINT' command function
void execute_PRINT_command(Token outToken, Variable symbolTable[], int varCount) {
    // If the token to be printed is a variable, print it out
    if (outToken.type == TOKEN_VARIABLE) {
        // Search for the variable if it is defined
        int i = search_for_variable(outToken.value, symbolTable, varCount);
        if (i != -1) {
            // If it is found, print the value of the variable
            printf("SNOL> [%s] = %g\n", symbolTable[i].name, symbolTable[i].value);
        } else {
            // If it is not defined, print an error.
            printf("SNOL> Error! [%s] is not defined!\n", outToken.value);
        }
    } else {
        // Print the value of the variable
        printf("SNOL> %s\n", outToken.value);
    }
}

// Assignment
void execute_assignment(Token tokens[], int token_count, Variable symbolTable[], int *varCount) {

    int error = 0;
    SNOL_Type type;
    double result;

    if (token_count == 3) {
        result = resolve_value(tokens[2], symbolTable, *varCount, &type, &error);
    } else if (token_count == 5) {
        result = execution_assignment(&tokens[2], symbolTable, *varCount, &type, &error);
    } else {
        printf("SNOL> Unknown command! Does not match any valid command of the language.\n");
        return;
    }

    if (error) return;

    int index = search_for_variable(tokens[0].value, symbolTable, *varCount);

    if (index == -1) {
        strcpy(symbolTable[*varCount].name, tokens[0].value);
        symbolTable[*varCount].value = result;
        symbolTable[*varCount].type = type;
        (*varCount)++;
    } else {
        symbolTable[index].value = result;
        symbolTable[index].type = type;
    }
}