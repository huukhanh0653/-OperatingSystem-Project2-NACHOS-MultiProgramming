//! Utility for User program

char buffer[33];

void strCopy(char *dest, const char *src)
{
    while (*src != '\0')
    {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

int len(char *str)
{
    int len = 0;
    while (*str != '\0')
    {
        str++;
        len++;
    }
    return len;
}

int abs(int x) { return (x > 0 ? x : -x); }

char* int_to_cstring(int num) {
    char *str;
    int i = 0, sign = 0;

    if (num < 0) {
        sign = 1;
        num = -num;
    }

    /* allocate memory for the cstring */
    str = (char*) malloc(20);

    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while(num > 0);

    if (sign)
        str[i++] = '-';

    str[i] = '\0';

    /* reverse string */
    int j = 0;
    while (j < i/2) {
        char temp = str[j];
        str[j] = str[i-j-1];
        str[i-j-1] = temp;
        j++;
    }

    return str;
}


char *_concat(char s1[], char s2[])
{
    int i, j;
    i = len(s1);
    for (j = 0; s2[j] != '\0'; i++, j++)
    {
        s1[i] = s2[j];
    }
    s1[i] = '\0';
    return s1;
}

void clrStr(char *buffer,int len)
{
    int i;
    for (i = 0; i < len; i++)
        buffer[i] = '\0';
}

void concatWithSpace(char s1[], char s2[])
{
    _concat(s1, " ");
    _concat(s1, s2);
}
