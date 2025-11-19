#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd, ret;
    char buf1[32] = "";

    fd = open("/dev/pchar0", O_WRONLY);
    if(fd < 0)
    {
        perror("failed to opened");
        _exit(1);
    }
    printf("device file opened\n");

    strcpy(buf1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ret = write(fd, buf1, strlen(buf1)); 
    printf("write-1 : bytes written in device: %d\n",ret);

    strcpy(buf1, "0123456789");
    ret = write(fd, buf1, strlen(buf1)); 
    printf("write-1 : bytes written in device: %d\n",ret);

    strcpy(buf1, "DESD");
    ret = write(fd, buf1, strlen(buf1)); 
    printf("write-1 : bytes written in device: %d\n",ret);

    close(fd);
    printf("device file closed\n");
    return 0;

}