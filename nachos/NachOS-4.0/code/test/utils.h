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
    char* temp;
    strCopy()
    int len = 0;
    while (*str != '\0')
    {
        str++;
        len++;
    }
    return len;
}

int abs(int x) { return (x > 0 ? x : -x); }

char* str(int num)
{
    int i = 0;
    // Handle negative integers
    if (num < 0) {
        str[i++] = '-';
        num = -num;
    }
    // Convert individual digits into a string
    if (num == 0) {
        str[i++] = '0';
    } else {
        while (num != 0) {
            int rem = num % 10;
            str[i++] = (rem > 9) ? (rem-10)+'a' : rem+'0';
            num /= 10;
        }
    }
    str[i] = '\0'; // Add null character to the end of the string
    // Reverse the string
    int len = i;
    for (i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len-i-1];
        str[len-i-1] = temp;
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
