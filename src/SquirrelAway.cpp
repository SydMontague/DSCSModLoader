#pragma once
#include <squirrel/squirrel.h>
#include "modloader/squirrel.h"
#include <stdint.h>

#include <tuple>

/* Special thanks to Pherakki for providing most of this implementation. */

// ************************* //
// ***** PULL HELPERS ****** //
// ************************* //
// TODO Needs specializing for each type a squirrel function can return
template<> int32_t pullParamValue<SQInteger&>(HSQUIRRELVM vm, SQInteger& slot, size_t index)
{
    sq_getinteger(vm, index, &slot);
    return 1;
}

template<> int32_t pullParamValue<int32_t&>(HSQUIRRELVM vm, int32_t& slot, size_t index)
{
    SQInteger val;
    sq_getinteger(vm, index, &val);
    slot = (int32_t)val;
    return 1;
}

template<> int32_t pullParamValue<uint32_t&>(HSQUIRRELVM vm, uint32_t& slot, size_t index)
{
    SQInteger val;
    sq_getinteger(vm, index, &val);
    slot = (uint32_t)val;
    return 1;
}

template<> int32_t pullParamValue<uint64_t&>(HSQUIRRELVM vm, uint64_t& slot, size_t index)
{
    SQInteger val;
    sq_getinteger(vm, index, &val);
    slot = val;
    return 1;
}

template<> int32_t pullParamValue<SQFloat&>(HSQUIRRELVM vm, SQFloat& slot, size_t index)
{
    sq_getfloat(vm, index, &slot);
    return 1;
}

template<> int32_t pullParamValue<bool&>(HSQUIRRELVM vm, bool& slot, size_t index)
{
    uint64_t l;
    sq_tobool(vm, index, &l);
    slot = l != 0;
    return 1;
}

template<> int32_t pullParamValue<HSQUIRRELVM&>(HSQUIRRELVM vm, HSQUIRRELVM& slot, size_t index)
{
    slot = vm;
    return 0;
}

template<> int32_t pullParamValue<const SQChar*&>(HSQUIRRELVM vm, const SQChar*& slot, size_t index)
{
    sq_tostring(vm, index);
    sq_getstring(vm, -1, &slot);
    sq_pop(vm, 1);
    return 1;
}

// ************************* //
// ***** PUSH HELPERS ****** //
// ************************* //
// TODO Needs specializing for each type a squirrel function can return
template<> void pushReturnValue<uint64_t>(HSQUIRRELVM vm, uint64_t& value) { sq_pushinteger(vm, value); }

template<> void pushReturnValue<int32_t>(HSQUIRRELVM vm, int32_t& value) { sq_pushinteger(vm, value); }

template<> void pushReturnValue<uint32_t>(HSQUIRRELVM vm, uint32_t& value) { sq_pushinteger(vm, value); }

template<> void pushReturnValue<SQInteger>(HSQUIRRELVM vm, SQInteger& value) { sq_pushinteger(vm, value); }

template<> void pushReturnValue<SQFloat>(HSQUIRRELVM vm, SQFloat& value) { sq_pushfloat(vm, value); }

template<> void pushReturnValue<bool>(HSQUIRRELVM vm, bool& value) { sq_pushbool(vm, value); }

template<> void pushReturnValue<const SQChar*>(HSQUIRRELVM vm, const SQChar*& value) { sq_pushstring(vm, value, -1); }
