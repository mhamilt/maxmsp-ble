/// Credit goes to https://gist.github.com/HoX/abfe15c40f2d9daebc35

#ifndef __SWITCHS_H__
#define __SWITCHS_H__

#include <string.h>
#include <stdbool.h>

/// Begin a switch for the string x
#define switchs(x) \
{ t_symbol *__sw = (x); bool __done = false; bool __cont = false; do {

/// Check if the string matches the cases argument (case sensitive) */
#define cases(x)    } if ( __cont || gensym(x) == __sw ) \
                        { __done = true; __cont = true;

/// continue with default case
#define defaults    } if ( !__done || __cont ) {

/// Close the switchs */
#define switchs_end } while ( 0 ); }

#endif // __SWITCHS_H__
