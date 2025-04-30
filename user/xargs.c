#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void run(char *commond, char **args) {
    int pid;
    if((pid = fork()) < 0) {
        fprintf(2, "xargs: fork failed\n");
        exit(1);
    } else if(pid == 0) {
        exec(commond, args);
        exit(0);
    }
    return;
}

int main(int argc, char *argv[]) {
    char buf[2048];  // 读入参数时使用的内存池
    char *p = buf, *last_p = buf;  // 当前参数的结束、开始指针
    char *argsbuf[128];  // 参数列表
    char **args = argsbuf;  // 指向argsbuf中的第一个参数
    for(int i = 1; i < argc; i++) {
        *args = argv[i];
        args++;
    }
    char **pa = args;  // pa 指向动态参数区域的开始

    char ch;
    while(read(0, &ch, 1) != 0) {
        *p = ch;
        if(ch == ' ' || ch == '\n') {
            // 读入一个参数完成时（参数间以空格或换行符分隔）
            *p = '\0';  // 以\0替换空格或换行符，这样可以直接使用buf中的字符串
            *pa++ = last_p;  // last_p指向一个参数的开始位置
            last_p = p + 1;  // buf中下一个参数的开始位置

            if(ch == '\n') {
                // 读入一行完成
                *pa = 0;  //参数列表末尾用NULL标志结束
                run(argv[1], argsbuf);  // 执行读取的一行命令
                pa = args;  // 重置 pa 到原来的位置
            }
        }
        p++;
    }
    if(pa != args) {  // 如果最后不是以换行符结尾
        *p = '\0';
        *pa++ = last_p;
        *pa = 0;
        run(argv[1], argsbuf);
    }

    while(wait(0) != -1) {}  // 循环等待所有子进程完成
    exit(0);
}