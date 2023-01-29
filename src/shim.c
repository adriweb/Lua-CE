#include "shim.h"

#include <errno.h>
#include <locale.h>

const static char *errors[] = {
	[EPERM]  = "EPERM",
	[EINVAL] = "EINVAL",
	[EIO]    = "EIO",
	[EDOM]   = "EDOM",
	[ERANGE] = "ERANGE",
};

char *strerror(int errnum) {
	return (char*)errors[errnum];
}

const static struct lconv lconv = {
	(char *)".",
	(char *)"",
	(char *)"",
	(char *)"",
	(char *)"",
	(char *)"",
	(char *)"",
	(char *)"",
	(char *)"",
	(char *)"",
	0x7f,
	0x7f,
	0x7f,
	0x7f,
	0x7f,
	0x7f,
	0x7f,
	0x7f,
	0x7f,
	0x7f,
	0x7f,
	0x7f,
	0x7f,
	0x7f,
};

struct lconv *localeconv (void) {
	return (struct lconv *) &lconv;
}

FILE * freopen ( const char * filename, const char * mode, FILE * stream ) {
	fclose(stream);
	return fopen(filename, mode);
}

#include <stdio.h>
#include <fileioc.h>

size_t __attribute__((weak)) fread(void *ptr, size_t size, size_t count, FILE *__restrict stream)
{
    size_t ncount;

    if (stream == NULL ||
        stream == stdout ||
        stream == stderr)
    {
        return 0;
    }

    if (stream == stdin)
    {
        int c;
        char *p = ptr;
        size_t len = size * count;

        for (; len > 0; len--)
        {
            if ((c = fgetc(stream)) == EOF)
            {
                break;
            }
            *p++ = c;
        }

        return count;
    }

    ncount = ti_Read(ptr, size, count, stream->slot);
    if (ncount != count)
    {
        stream->eof = 1;
    }

    return ncount;
}
