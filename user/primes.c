#include "kernel/types.h"
#include "user/user.h"

void panic (char *s) {
    fprintf(2, "%s\n", s);
    exit(1);
}

int fork1 () {
    int pid;
    pid = fork();
    if (pid < 0) {
        panic("fork");
    }
    return pid;
}

// void primeproc (int read_fd) {
//     int len;
//     char buf[16];

//     if (read(read_fd, &len, sizeof(int)) != sizeof(int)) {
//         exit(0);  // 读到字符串末尾了
//     }

//     if (read(read_fd, buf, len) != len) panic("child read\n");
//     buf[len] = '\0';
//     int prime = atoi(buf);
//     printf("prime %d\n", prime);

//     int p2[2];
//     if (pipe(p2) < 0) panic("pipe 2\n");
    
//     if (fork1() > 0) {
//         close(p2[0]);
//         while (read(read_fd, &len, sizeof(int)) == sizeof(int)) {
//             if (read(read_fd, buf, len) != len) panic("child read\n");
//             buf[len] = '\0';
//             int n = atoi(buf);
//             if (n % prime != 0) {
//                 if (write(p2[1], &len, sizeof(int)) != sizeof(int) || write(p2[1], buf, len) != len) {
//                     panic("write in primeproc\n");
//                 }
//             }
//         }
//         close(read_fd);
//         close(p2[1]);
//         wait(0);
//     } else {
//         close(p2[1]);
//         primeproc(p2[0]);
//     }
// }

// int main (int argc, char *argv[]) {
//     char *nums[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "11",
//                     "12", "13", "14", "15", "16", "17", "18", "19", "20",
//                     "21", "22", "23", "24", "25", "26", "27", "28", "29",
//                     "30", "31", "32", "33", "34", "35", 0};

//     printf("prime %s\n", nums[0]);  // print 2

//     int p1[2];
//     if (pipe(p1) < 0) {
//         panic("pipe");
//     }

//     if (fork1() > 0) {  // parent
//         close(p1[0]);  // 关闭读端
//         for (int i = 1; nums[i]; i++) {
//             if (atoi(nums[i]) % 2 != 0) {
//                 int len = strlen(nums[i]);
//                 if (write(p1[1], &len, sizeof(int)) != sizeof(int) || write(p1[1], nums[i], len) != strlen(nums[i])) {
//                     panic("write");
//                 }
//             }
//         }
//         close(p1[1]);
//         wait(0);
//         exit(0);
//     } else {  // child
//         close(p1[1]);
//         primeproc(p1[0]);
//     }

//     return 0;
// }

void primeProc(int read_fd) {
    int num;
    int prime;

    if (read(read_fd, &num, sizeof(int)) != sizeof(int)) {
        exit(0);
    }
    if (num == -1) {
        exit(0);
    }
    prime = num;
    printf("prime %d\n", prime);

    int p2[2];
    if (pipe(p2) < 0) panic("pipe");

    if (fork1() > 0) {
        close(p2[0]);
        while (read(read_fd, &num, sizeof(int)) == sizeof(int) && num != -1) {
            if (num % prime != 0) {
                if (write(p2[1], &num, sizeof(int)) != sizeof(int)) panic("write");
            }
        }
        num = -1;
        if (write(p2[1], &num, sizeof(int)) != sizeof(int)) panic("write");
        close(read_fd);
        close(p2[1]);
        wait(0);
    } else {
        close(p2[1]);
        primeProc(p2[0]);
    }
}

int main (int argc, char *argv[]) {
    int p1[2];
    if (pipe(p1) < 0) panic("pipe");
    
    if (fork1() > 0) {  // parent
        close(p1[0]);
        int i;
        for (i = 2; i <= 35; i++) {
            if (write(p1[1], &i, sizeof(int)) != sizeof(int)) panic("write");
        }
        i = -1;  // 结束的标志
        if (write(p1[1], &i, sizeof(int)) != sizeof(int)) panic("write");
        close(p1[1]);
        wait(0);
        exit(0);
    } else {  // child
        close(p1[1]);
        primeProc(p1[0]);
    }

    return 0;
}