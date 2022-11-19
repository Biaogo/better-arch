#include <stdio.h>
#include <stdlib.h>
#include "cjson/cJSON.h"
#include "getopt.h"
#include "main.h"

int main(int argc, char **argv) {
    get_option(argc, argv);
    return 0;
}

void get_option(int argc, char **argv) {
    int opt;
    int option = 0;
    struct option long_options[] = {
            {"install", required_argument, NULL, 'i'},
            {"remove",  required_argument, NULL, 'r'},
            {0, 0, 0,                            0}
    };
    while ((opt = getopt_long(argc, argv, "i:r:",long_options,&option)) != -1) {
        switch (opt) {
//            case 'f':
//                read_file(argv);
//                break;
            case 'i':
                install(argv);
                break;
            case 'r':
                uninstall(argv);
                break;
            default:
                printf("Usage: bta <operation> [...]\r\noperations:\n\tbta {-i --install}\n\tbta {-r --remove}\n");
                break;
        }
    }
}

char *read_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("open file error");
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *data = (char *) malloc(len + 1);
    fread(data, 1, len, fp);
    fclose(fp);
    data[len] = '\0';
    return data;
}

cJSON *cJson_GetObjectItem(char **argv) {
    char *data = read_file(argv[2]);
    cJSON *json = cJSON_Parse(data);
    cJSON *item = cJSON_GetObjectItem(json, "pkg");
    if (item == NULL) {
        printf("get json item error");
        return NULL;
    }
    return item;
}

char *install(char **argv) {
    cJSON *pkgsArray = cJson_GetObjectItem(argv);
    int size = cJSON_GetArraySize(pkgsArray);
    for (int i = 0; i < size; i++) {
        cJSON *pkg = cJSON_GetArrayItem(pkgsArray, i);
        char *str = cJSON_GetStringValue(pkg);
        pid_t pid;
        if (str != NULL) {
            char cmd[100] = {""};
            char cmd2[100] = {""};
            sprintf(cmd, "pacman -Qq %s > /dev/null 2>&1", str);
            pid = system(cmd);
            if (pid != 0) {
                sprintf(cmd2, "pacman -S --noconfirm %s", str);
                system(cmd2);
                //execlp("pacman", "pacman", "-S", "--noconfirm", str, NULL);
            } else {
                printf("\nPackage %s is already installed\n", str);
            }
        }
    }
}

char *uninstall(char **argv) {
    cJSON *pkgsArray = cJson_GetObjectItem(argv);
    int size = cJSON_GetArraySize(pkgsArray);
    for (int i = 0; i < size; i++) {
        cJSON *pkg = cJSON_GetArrayItem(pkgsArray, i);
        char *str = cJSON_GetStringValue(pkg);
        pid_t pid;
        if (str != NULL) {
            char cmd[100] = {""};
            char cmd2[100] = {""};
            sprintf(cmd, "pacman -Qq %s > /dev/null 2>&1", str);
            pid = system(cmd);
            printf("%s", cmd);
            if (pid == 0) {
                sprintf(cmd2, "pacman -R --noconfirm %s", str);
                system(cmd2);
            } else {
                printf("\nPackage %s is not installed\n", str);
            }
        }
    }
}