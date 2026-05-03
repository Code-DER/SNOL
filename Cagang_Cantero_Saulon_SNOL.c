#include <stdio.h>
#include <string.h>

int compare_strings(const char *str1, const char *str2);

int main() {

    printf("The SNOL environment is now active, you may proceed with giving your commands.\n");
    char command[100];
    do {
        printf("\nCommand: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

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