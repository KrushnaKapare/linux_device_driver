#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd, ret;
    char buf1[32] = "";
    // open device file for wr
    fd = open("/dev/pchar0", O_WRONLY);
    if (fd < 0)
    {
        perror("failed to open device");
        _exit(1);
    }
    printf("device file opened.\n");
    // write (26 bytes) in device file
    strcpy(buf1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ret = write(fd, buf1, strlen(buf1)); // returns 26
    printf("Wr 1- bytes written in device: %d\n", ret);
    // write (10 bytes) in device file
    strcpy(buf1, "0123456789");
    ret = write(fd, buf1, strlen(buf1)); // returns 6
    printf("Wr 2- bytes written in device: %d\n", ret);
    // write (4 bytes) in device file
    strcpy(buf1, "DESD");
    ret = write(fd, buf1, strlen(buf1));
    if (ret < 0)
        perror("Wr 3- failed");
    printf("Wr 3- bytes written in device: %d\n", ret); // returns error
    // close device file
    close(fd);
    printf("device file closed.\n");
    return 0;
}

// cmd> gcc pchar_write_test.c -o pchar_wr_test.out
// cmd> sudo insmod pchar.ko     # if driver is not already loaded
// cmd> sudo ./pchar_wr_test.out
// cmd> sudo dmesg | tail 20
