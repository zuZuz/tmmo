#include <ctype.h>
#include <stddef.h>

char* get_word(char **position)
{
    char *w_start;
    char *pos = *position;

    while (isspace(*pos) && (*pos != '\0') )
        pos++;

    w_start = pos;

    while(*pos != '\0')
    {
        if(!isspace(*pos))
        {
            *pos =  tolower( *pos );
            pos++;
        }
        else
        {
            *pos = '\0';
            pos++;
            break;
        }

    }

    *position = pos;

    return w_start != pos ? w_start : NULL;
}

