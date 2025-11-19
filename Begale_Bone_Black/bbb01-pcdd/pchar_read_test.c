#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd, ret;
    char buf2[32] ="";
    fd = open("/dev/pchar0", O_RDONLY);
    if(fd<0)
    {
        perror("failed to open device");
        _exit(1);
    }
    printf("device file opened\n");

    memset(buf2, 0, sizeof(buf2));
    ret = read(fd, buf2, 26);
    pintf("read-1 : bytes read in device %d ==> %s\n", ret, buf2);


    memset(buf2, 0, sizeof(buf2));
    ret = read(fd, buf2, 10);
    pintf("read-2 : bytes read in device %d ==> %s\n", ret, buf2);


    memset(buf2, 0, sizeof(buf2));
    ret = read(fd, buf2, 4);
    pintf("read-3 : bytes read in device %d ==> %s\n", ret, buf2);

    close(fd);
    printf("dvice file closed\n");

    return 0;
}