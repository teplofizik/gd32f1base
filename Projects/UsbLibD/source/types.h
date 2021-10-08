//
// types.h
//
// Basic types
//

#include <stdint.h>
#include <stdbool.h>

#ifndef _TYPES_H
#define _TYPES_H

	typedef void (* TEventHandler)(void);
	
	// Serial types
    typedef void (* TCharHandler)(char Ch);
    typedef void (* TSerialHandler)(char * Ch);

#endif
