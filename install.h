#include "main.h"

#ifndef UNTITLED__INSTALL_H_
#define UNTITLED__INSTALL_H_
#endif //UNTITLED__INSTALL_H_

int install(char **argv) {
    if (argv[2] == NULL) {
        printf("Please select pkgs.json!\n");
        return 1;
    }
    FILE *file_name;
    file_name = fopen(argv[2], "r");
    if (file_name == NULL) {
        fprintf(stderr, "file read failed!\n");
        return 1;
    }
    char buf[65536] = {0};
    fread(buf, sizeof(buf), 1, file_name);
    cJSON *cjson = cJSON_Parse(buf);
    if (cjson == NULL) {
        fprintf(stderr, "JSON read failed!\n");
        return 1;
    }
    cJSON *pkgsArray = cJSON_GetObjectItem(cjson, "pkgs");
    if (pkgsArray != NULL) {
        pid_t status;
        char finds[29] = {""};
        char command[28] = {""};
        int count = 0;
        while (cJSON_GetArrayItem(pkgsArray, count) != NULL) {
            char *str = cJSON_GetStringValue(cJSON_GetArrayItem(pkgsArray, count));
            printf("\n%d package\n", count + 1);
            printf("Check if the package exists：%s\n", str);
            sprintf(finds, "pacman -Qq %s > /dev/null", str);
            status = system(finds);
            if (status != 0) {
                printf("Installing: %s ...\n", str);
                sprintf(command, "pacman -S --noconfirm %s", str);
                system(command);
            } else {
                printf("you have installed %s\n", str);
            }
            count++;
        }
    }

    cJSON *gitSourceAndPathObject = cJSON_GetObjectItem(cjson, "gitSourceAndPath");
    if (gitSourceAndPathObject != NULL) {
        cJSON *gitSourceAndPathObjectIndex1 = gitSourceAndPathObject->child;
        char command[12] = {""};
        while (gitSourceAndPathObjectIndex1 != NULL) {
            if (access(gitSourceAndPathObjectIndex1->valuestring, F_OK) == 0) {
                printf("%s found!\n", gitSourceAndPathObjectIndex1->valuestring);
            } else {
                sprintf(command,
                        "git clone %s %s",
                        gitSourceAndPathObjectIndex1->string,
                        gitSourceAndPathObjectIndex1->valuestring);
                system(command);
            }
            gitSourceAndPathObjectIndex1 = gitSourceAndPathObjectIndex1->next;
        }
    }
    fclose(file_name);
}