#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    int i,j,k;
    int ret;
    char buf1[5],buf2[5],buf3[5];

    i = open("test_dup.c", O_RDONLY);
    j = dup2(i,0);
    k = open("test_dup.c", O_RDONLY);
    printf("%d, %d, %d\n", i, j, k);

    char a = getchar();
    printf("%c\n", a);
    printf("%zd\n", read(i, buf1, sizeof(buf1)));
    printf("%zd\n", read(j, buf2, sizeof(buf2)));
    read(k, buf3, sizeof(buf3));
    close(k);
    close(i);
    close(j);

    for (int k=0; k<5; k++) {
        printf("%c",buf1[k]);
    }
    printf("\n");
    for (int k=0; k<5; k++) {
        printf("%c",buf2[k]);
    }
    printf("\n");
    for (int k=0; k<5; k++) {
        printf("%c",buf3[k]);
    }
    printf("\n");

    return 0;
}
