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

void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
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

void clrStr(char *buffer, int len)
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
