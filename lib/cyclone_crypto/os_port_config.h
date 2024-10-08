#ifndef _OS_PORT_CONFIG_H
#define _OS_PORT_CONFIG_H

#ifndef osAllocMem
   #include <stdlib.h>
   #define osAllocMem(size) calloc(1, size)
#endif

#ifndef osFreeMem
   #include <stdlib.h>
   #define osFreeMem(p) free(p)
#endif

#endif