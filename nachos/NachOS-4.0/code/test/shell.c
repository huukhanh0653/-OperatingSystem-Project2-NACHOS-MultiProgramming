#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = _ConsoleInput;
    OpenFileId output = _ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i;

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
	Write(prompt, 2, output);

	i = 10;
	
	Read(buffer, 32, input);

	if( i > 0 ) {
		newProc = Exec(buffer);
		Join(newProc);
	}
    }
}