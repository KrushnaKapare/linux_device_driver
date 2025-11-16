#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd, ret;
    char buf1[35] = "", buf2[32] = "";
    off_t pos;
    // open device file for rd-wr
    fd = open("/dev/pchar0", O_RDWR);
    if (fd < 0)
    {
        perror("failed to open device");
        _exit(1);
    }
    printf("device file opened.\n");
    // write (32 bytes) in device file
    strcpy(buf1, "012345ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ret = write(fd, buf1, strlen(buf1)); // returns 26
    printf("Wr - bytes written in device: %d\n", ret);
    pos = lseek(fd, 0, SEEK_CUR);
    printf("cur pos = %ld\n", pos);  
    lseek(fd, 0, SEEK_SET);

    memset(buf2, 0, sizeof(buf2));   
    ret = read(fd, buf2, 6);
    printf("Read - %d bytes read ==> %s\n",ret, buf2);
    pos = lseek(fd, 0, SEEK_CUR);
    printf("cur pos =%ld\n",pos);

    lseek(fd, -3, SEEK_END);
    memset(buf2, 0, sizeof(buf2));
    ret = read(fd, buf2, 3);
    printf("Rd - byts read: %d - %s\n", ret, buf2);

    close(fd);
    printf("device file closed.\n");
    return 0;

}