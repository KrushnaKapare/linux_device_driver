#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
    int fd, ret;

    char buf2[32] = "";
    
    fd = open("/dev/pchar0", O_RDONLY);  // open device file for read 
    if(fd < 0)
    {
        perror("failed to open device");
        _exit(1);
    }
    printf("device file opened\n");

    memset(buf2, 0, sizeof(buf2));  // read (26 bytes) from device file
    ret = read(fd, buf2, 26);       // returns 26
    printf("read-1 : %d bytes read in device ==> %s\n",ret,buf2);

    memset(buf2, 0, sizeof(buf2));  // read (10 bytes) from device file
    ret = read(fd, buf2, 10);       // returns 6
    printf("read-2 : %d bytes read in device ==> %s\n",ret,buf2);

    memset(buf2, 0, sizeof(buf2));  // read (4 bytes) from device file
    ret = read(fd, buf2, 4);       // returns 0
    printf("read-3 : %d bytes read in device ==> %s\n",ret,buf2);

    close(fd);                     // close device file
    printf("device file closed\n");
    return 0;
}
