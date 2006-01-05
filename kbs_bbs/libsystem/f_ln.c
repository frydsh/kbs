/* ----------------------------------------------------- */
/* f_ln() : link() cross partition / disk		 */
/* ----------------------------------------------------- */


#include <fcntl.h>
#include <errno.h>
#include "system.h"

int f_ln(src, dst)
    char *src, *dst;
{
    int ret;

    if ((ret = link(src, dst))!=0) {
        if (errno != EEXIST)
            ret = f_cp(src, dst, O_EXCL);
    }
    return ret;
}