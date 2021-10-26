#pragma once

/*
	Very hacky way of linking the mod loader in the freetype.dll
	Will probably change eventually.
*/
#ifdef __cplusplus
extern "C"
#endif
void __declspec(dllexport) doSomething() {};
