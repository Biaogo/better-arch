#include "main.h"
#include "install.h"
#include "usage.h"

void get_option(int argc, char **argv) {
    while (1) {
        int option = 0;
        struct option long_options[] = {
                {"help", 0, 0},
                {"file", 1, 0}
        };
        int ch;
        ch = getopt_long(argc, argv, "hf:", long_options, &option);
        if (ch == -1) {
            break;
        }
        switch (ch) {
            case 'h':
            default:
                usage();
                break;
            case 'f':
                install(argv);
                break;
        }
    }
}

int main(int argc, char **argv) {
    get_option(argc, argv);
    return 0;
}