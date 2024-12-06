#pragma once

__attribute__((section(".text.start"))) int start();
__attribute__((weak)) void                  _PSX_PRINT(char *c);
__attribute__((weak)) void                  _PSX_PRINTF_1(char *f, void *a);
__attribute__((weak)) void                  _PSX_PRINTF_2(char *f, void *a, void *b);
__attribute__((weak)) void                  _PSX_PRINTF_3(char *f, void *a, void *b, void *c);
__attribute__((weak)) void                  _PSX_PRINTF_4(char *f, void *a, void *b, void *c, void *d);
#define PSX_PRINT(a)                _PSX_PRINT(a)
#define PSX_PRINTF_1(f, a)          _PSX_PRINTF_1(f, (void *)a)
#define PSX_PRINTF_2(f, a, b)       _PSX_PRINTF_2(f, (void *)a, (void *)b)
#define PSX_PRINTF_3(f, a, b, c)    _PSX_PRINTF_3(f, (void *)a, (void *)b, (void *)c)
#define PSX_PRINTF_4(f, a, b, c, d) _PSX_PRINTF_4(f, (void *)a, (void *)b, (void *)c, (void *)d)