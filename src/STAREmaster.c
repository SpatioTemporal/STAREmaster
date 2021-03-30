/**
 * @file
 *
 * Contains the STAREmaster_inq_libver() function.
 */

#include "config.h"
#include "STAREmaster.h"

const char *
STAREmaster_inq_libvers(void)
{
    return PACKAGE_STRING;
}
