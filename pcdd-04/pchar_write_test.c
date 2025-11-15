#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{

    int fd, ret;
    char buf1[32] = "";

    fd = open("/dev/pchar0", O_WRONLY);    // open device file for write

    if(fd<0)
    {
        perror("failed to open device");
        _exit(1);
    }

    printf("device file opened\n");

    strcpy(buf1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");  //write (26 bytes) into device file
    ret = write(fd, buf1, strlen(buf1));        // returns 26
    printf("Write-1 : %d bytes written in device\n",ret);

    strcpy(buf1, "0123456789");           // write (10 bytes) in device file
    ret = write(fd, buf1, strlen(buf1));  // returns 6
    printf("write-2 : %d bytes written in device\n",ret); 

    strcpy(buf1, "DESD");   // write (4 bytes) in device file
    ret = write(fd, buf1, strlen(buf1));  //returns 4
    if(ret < 0)
        perror("write-3 : failed");
    printf("write-3 : %d bytes written in device\n",ret); //return error

    close(fd);
    printf("device file closed\n");
    return 0;
}