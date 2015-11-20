#ifndef __AS3UTIL_H
#define __AS3UTIL_H

#define ATOMIC_GET(mutex, value, into) do {  \
	pthread_mutex_lock(&(mutex));  \
	(into) = (value);  \
	pthread_mutex_unlock(&(mutex)); \
} while (0) \

#endif 
