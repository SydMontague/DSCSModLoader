#pragma once

#define BASE_ADDRESS (char*)GetModuleHandle(NULL)
#define ptr2bytes(ptr) ((uint8_t*) &ptr)[0],((uint8_t*) &ptr)[1],((uint8_t*) &ptr)[2],((uint8_t*) &ptr)[3],((uint8_t*) &ptr)[4],((uint8_t*) &ptr)[5],((uint8_t*) &ptr)[6],((uint8_t*) &ptr)[7]

#ifdef __cplusplus
extern "C"
#endif
void __declspec(dllexport) doSomething() {};
