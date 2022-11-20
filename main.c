#include <stdio.h>
#include <stdlib.h>
#include "cjson/cJSON.h"
#include "getopt.h"
#include "main.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    get_option(argc, argv);
    return 0;
}

void get_option(int argc, char *argv[]) {
    int opt;
    int option = 0;
    struct option long_options[] = {
            {"install", required_argument, NULL, 'i'},
            {"remove",  required_argument, NULL, 'r'},
            {"git",     required_argument, NULL, 'g'},
            {"gitdel",  required_argument, NULL, 'd'},
            {0, 0, 0,                            0}
    };
    while ((opt = getopt_long(argc, argv, "i:r:g:d:", long_options, &option)) != -1) {
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
            case 'g':
                gitaclone(argv);
                break;
            case 'd':
                gitdel(argv);
                break;
            default:
                usage();
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

cJSON *cJson_GetObjectItem(char *argv[]) {
    char *data = read_file(argv[2]);
    cJSON *json = cJSON_Parse(data);
    if (json == NULL) {
        printf("get json item error");
        return NULL;
    }
    return json;
}

char *install(char *argv[]) {
    cJSON *pkgsArray = cJson_GetObjectItem(argv);
    cJSON *item = cJSON_GetObjectItem(pkgsArray, "pkgs");
    int size = cJSON_GetArraySize(item);
    for (int i = 0; i < size; i++) {
        cJSON *pkg = cJSON_GetArrayItem(item, i);
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
            } else {
                printf("\nPackage '%s' is already installed\n", str);
            }
        }
    }
}

char *uninstall(char *argv[]) {
    cJSON *pkgsArray = cJson_GetObjectItem(argv);
    cJSON *item = cJSON_GetObjectItem(pkgsArray, "pkgs");
    int size = cJSON_GetArraySize(item);
    for (int i = 0; i < size; i++) {
        cJSON *pkg = cJSON_GetArrayItem(item, i);
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
                printf("\nPackage '%s' is not installed\n", str);
            }
        }
    }
}

char *gitaclone(char *argv[]) {
    cJSON *gitArray = cJson_GetObjectItem(argv);
    cJSON *item = cJSON_GetObjectItem(gitArray, "gitSourceAndPath");
    cJSON *gitSAP = item->child;
    int size = cJSON_GetArraySize(item);
    for (int i = 0; i < size; i++) {
        if (access(gitSAP->valuestring, F_OK) == -1) {
            char cmd[100] = {""};
            sprintf(cmd, "git clone %s %s", gitSAP->string, gitSAP->valuestring);
            system(cmd);
        } else {
            printf("\n'%s' is already exist in '%s'\n", gitSAP->string, gitSAP->valuestring);
        }
        gitSAP = gitSAP->next;
    }
}

char *gitdel(char *argv[]) {
    cJSON *gitArray = cJson_GetObjectItem(argv);
    cJSON *item = cJSON_GetObjectItem(gitArray, "gitSourceAndPath");
    cJSON *gitSAP = item->child;
    int size = cJSON_GetArraySize(item);
    for (int i = 0; i < size; i++) {
        if (access(gitSAP->valuestring, F_OK) == 0) {
            char cmd[100] = {""};
            sprintf(cmd, "rm -rf %s", gitSAP->valuestring);
            system(cmd);
        } else {
            printf("\n'%s' is not exist in '%s'\n", gitSAP->string, gitSAP->valuestring);
        }
        gitSAP = gitSAP->next;
    }
}

static void usage() {
    printf("Usage:  bta <operation> [...]\n");
    printf(("operations:\n"));
    printf("    bta {-h --help}\n");
    printf("    bta {-i --install}\n");
    printf("    bta {-r --remove}\n");
    printf("    bta {-g --gitclone}\n");
    printf("    bta {-d --gitdel}\n");
    printf("\nuse 'bta {-h --help}' with an operation for available options\n");
}