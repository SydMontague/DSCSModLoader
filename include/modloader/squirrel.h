#pragma once
#include <squirrel/squirrel.h>
#include <tuple>

/* Special thanks to Pherakki for providing most of this implementation. */

// convenience macro
#define SQUIRREL_AWAY(function) SquirrelAway<decltype(function)>::call((void**)function)

// ************************* //
// ***** PULL HELPERS ***** //
// ************************* //
// TODO Needs specializing for each type a squirrel function can return
template<typename ParamType>
void pullParamValue(HSQUIRRELVM vm, ParamType& slot, size_t index)
{
    // requires specialization
}

template<>
void pullParamValue<SQInteger&>(HSQUIRRELVM vm, SQInteger& slot, size_t index)
{
    sq_getinteger(vm, index, &slot);
}

// ************************* //
// ***** PUSH HELPERS ***** //
// ************************* //
// TODO Needs specializing for each type a squirrel function can return
template<typename RetType>
void pushReturnValue(HSQUIRRELVM vm, RetType& value)
{
    // requires specialization
}

template<>
void pushReturnValue<SQInteger>(HSQUIRRELVM vm, SQInteger& value) {
    sq_pushinteger(vm, value);
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
constexpr void apply_func_on_tuple(HSQUIRRELVM vm, std::tuple<Types...>& tpl) noexcept
{
    static_for<sizeof...(Types)>([&](auto index)
    {
        auto& tuple_entry = std::get<index.value>(tpl);
        pullParamValue<decltype(tuple_entry)>(vm, tuple_entry, index.value + 2);
    });
}

// ************************ //
// ***** CALL HELPER ***** //
// ************************ //
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
    // Set up the storage unit to hold the parameters we will pull out of the Squirrel VM
    static const uint8_t n_inputs = sizeof...(ParamTypes);
    std::tuple<ParamTypes...> inputs;

    //// Get the function (test function defined at top of file)
    SQUserPointer userData;
    sq_getuserdata(vm, -1, &userData, nullptr);
    typedef RetType(*fPtr)(ParamTypes...);
    fPtr p = (fPtr)*(void**)userData;


    // Now pull the parameters from the VM
    apply_func_on_tuple(vm, inputs);

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
