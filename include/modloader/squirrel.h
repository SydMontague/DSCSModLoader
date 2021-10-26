#pragma once
#include <squirrel/squirrel.h>
#include <tuple>
#include <stdint.h>

/* Special thanks to Pherakki for providing most of this implementation. */

// convenience macro
#define SQUIRREL_AWAY(function) SquirrelAway<decltype(function)>::call((void**)function)

// ************************* //
// ***** PULL HELPERS ****** //
// ************************* //
// TODO Needs specializing for each type a squirrel function can return
template<typename ParamType>
int32_t pullParamValue(HSQUIRRELVM vm, ParamType& slot, size_t index)
{
    // requires specialization
    return 0;
}

template<>
int32_t pullParamValue<SQInteger&>(HSQUIRRELVM vm, SQInteger& slot, size_t index)
{
    sq_getinteger(vm, index, &slot);
    return 1;
}

template<>
int32_t pullParamValue<int32_t&>(HSQUIRRELVM vm, int32_t& slot, size_t index)
{
    SQInteger val;
    sq_getinteger(vm, index, &val);
    slot = (int32_t) val;
    return 1;
}

template<>
int32_t pullParamValue<uint32_t&>(HSQUIRRELVM vm, uint32_t& slot, size_t index)
{
    SQInteger val;
    sq_getinteger(vm, index, &val);
    slot = (uint32_t) val;
    return 1;
}

template<>
int32_t pullParamValue<uint64_t&>(HSQUIRRELVM vm, uint64_t& slot, size_t index)
{
    SQInteger val;
    sq_getinteger(vm, index, &val);
    slot = val;
    return 1;
}

template<>
int32_t pullParamValue<SQFloat&>(HSQUIRRELVM vm, SQFloat& slot, size_t index)
{
    sq_getfloat(vm, index, &slot);
    return 1;
}

template<>
int32_t pullParamValue<bool&>(HSQUIRRELVM vm, bool& slot, size_t index)
{
    uint64_t l;
    sq_tobool(vm, index, &l);
    slot = l != 0;
    return 1;
}

template<>
int32_t pullParamValue<HSQUIRRELVM&>(HSQUIRRELVM vm, HSQUIRRELVM& slot, size_t index)
{
    slot = vm;
    return 0;
}

template<>
int32_t pullParamValue<const SQChar*&>(HSQUIRRELVM vm, const SQChar*& slot, size_t index)
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
template<typename RetType>
void pushReturnValue(HSQUIRRELVM vm, RetType& value)
{
    // requires specialization
}

template<>
void pushReturnValue<uint64_t>(HSQUIRRELVM vm, uint64_t& value) {
    sq_pushinteger(vm, value);
}

template<>
void pushReturnValue<int32_t>(HSQUIRRELVM vm, int32_t& value) {
    sq_pushinteger(vm, value);
}

template<>
void pushReturnValue<uint32_t>(HSQUIRRELVM vm, uint32_t& value) {
    sq_pushinteger(vm, value);
}

template<>
void pushReturnValue<SQInteger>(HSQUIRRELVM vm, SQInteger& value) {
    sq_pushinteger(vm, value);
}

template<>
void pushReturnValue<SQFloat>(HSQUIRRELVM vm, SQFloat& value) {
    sq_pushfloat(vm, value);
}

template<>
void pushReturnValue<bool>(HSQUIRRELVM vm, bool& value) {
    sq_pushbool(vm, value);
}

template<>
void pushReturnValue<const SQChar*>(HSQUIRRELVM vm, const SQChar*& value) {
    sq_pushstring(vm, value, -1);
}

// ************************* //
// **** PARAM ITERATORS **** //
// ************************* //
// From https://stackoverflow.com/a/56819980
template <typename T, auto ...I, typename F>
constexpr void static_for_low(F&& func, std::integer_sequence<T, I...>)
{
    (void(func(std::integral_constant<T, I>{})), ...);
}

template <auto N, typename F>
constexpr void static_for(F&& func)
{
    static_for_low(func, std::make_integer_sequence<decltype(N), N>{});
}

template <typename ...Types>
constexpr void pullInputParams(HSQUIRRELVM vm, std::tuple<Types...>& tpl) noexcept
{
    uint32_t sqId = 2;
    static_for<sizeof...(Types)>([&](auto index)
    {
        auto& tuple_entry = std::get<index.value>(tpl);
        sqId += pullParamValue<decltype(tuple_entry)>(vm, tuple_entry, sqId);
    });
}

// *********************** //
// ***** CALL HELPER ***** //
// *********************** //
template<typename RetType, typename Func, typename Tuple, size_t... I>
RetType call_function(Func function, Tuple&& t, std::index_sequence<I...>)
{
    if constexpr (std::is_same<RetType, void>::value)
        function(std::get<I>(t)...);
    else
        return function(std::get<I>((std::forward<Tuple>(t)))...);
}

// ************************ //
// ****** END RESULT ****** //
// ************************ //
struct SquirrelFunction {
    void** function;
    SQFUNCTION closure;
};

template<typename RetType, typename... ParamTypes>
SQInteger squirrelize(HSQUIRRELVM vm)
{
    // Get the function
    SQUserPointer userData;
    sq_getuserdata(vm, -1, &userData, nullptr);
    typedef RetType(*fPtr)(ParamTypes...);
    fPtr p = (fPtr)*(void**)userData;

    // Set up the storage unit to hold the parameters we will pull out of the Squirrel VM
    static const uint8_t n_inputs = sizeof...(ParamTypes);
    std::tuple<ParamTypes...> inputs;
    // Now pull the parameters from the VM
    pullInputParams(vm, inputs);

    // Finally implement the call and return: split on void type
    auto indices = std::make_index_sequence<n_inputs>();
    if constexpr (std::is_same<RetType, void>::value)
    {
        // Do the void function call
        call_function<RetType>(p, inputs, indices);
        return 0;
    }
    else
    {
        // Make the function call
        RetType value = call_function<RetType>(p, inputs, indices);

        // Push back into the VM
        pushReturnValue(vm, value);
        return 1;
    }
}

template<typename> 
struct SquirrelAway;

template<typename R, typename... Args>
struct SquirrelAway<R(Args...)> {
    static SquirrelFunction call(void** f) {
        return { f, squirrelize<R, Args...> };
    }
};
