#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd, ret;
    char buf2[32] = "";
    // open device file for rd
    fd = open("/dev/pchar0", O_RDONLY);
    if (fd < 0)
    {
        perror("failed to open device");
        _exit(1);
    }
    printf("device file opened.\n");
    // read (26 bytes) from device file
    memset(buf2, 0, sizeof(buf2));
    ret = read(fd, buf2, 26); // returns 26
    printf("Rd 1- bytes read in device: %d - %s\n", ret, buf2);
    // read (10 bytes) from device file
    memset(buf2, 0, sizeof(buf2));
    ret = read(fd, buf2, 10); // returns 6
    printf("Rd 2- bytes read in device: %d - %s\n", ret, buf2);
    // read (4 bytes) from device file
    memset(buf2, 0, sizeof(buf2));
    ret = read(fd, buf2, 4); // returns 0
    printf("Rd 3- bytes read in device: %d - %s\n", ret, buf2);
    // close device file
    close(fd);
    printf("device file closed.\n");
    return 0;
}

// run this after write_test
// cmd> gcc pchar_read_test.c -o pchar_rd_test.out
// cmd> sudo ./pchar_rd_test.out
// cmd> sudo dmesg | tail 20
// cmd> sudo rmmod pchar
