
#ifndef EXCEPTION_H
#define EXCEPTION_H

#define throw_if(condition) \
if((condition))\
{\
	fprintf(stderr, "[Исключение] Файл %s, строка %d\n", __FILE__, __LINE__);\
	throw 1;\
};

#define throw_null(pointer) throw_if((pointer) == NULL)

#endif

