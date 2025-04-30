#include "kernel/types.h"
#include "user/user.h"


int main (int argc, char *argv[]) {
    int p1[2];  // parent -> child
    int p2[2];  // chlid -> parent
    int pid;
    char buf;

    if (pipe(p1) < 0 || pipe(p2) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        fprintf(2, "fork failed\n");
        close(p1[0]);
        close(p1[1]);
        close(p2[0]);
        close(p2[1]);
        exit(1);
    } else if (pid == 0) {  // child
        close(p1[1]);
        close(p2[0]);

        if (read(p1[0], &buf, 1) != 1) {
            fprintf(2, "child: read failed\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());

        if (write(p2[1], "Q", 1) != 1) {  // send "pong"
            fprintf(2, "child: write failed\n");
            exit(1);
        }
        close(p1[0]);
        close(p2[1]);
        exit(0);
    } else {  // parent
        close(p2[1]);
        close(p1[0]);

        if (write(p1[1], "P", 1) != 1) {  // send "ping"
            fprintf(2, "parent: write failed\n");
            exit(1);
        }

        if (read(p2[0], &buf, 1) != 1) {
            fprintf(2, "parent: read failed\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        close(p2[0]);
        close(p1[1]);
        exit(0);
    }
}