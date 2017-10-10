#pragma once

#include <type_traits>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>

namespace sgl {

namespace detail {

    template <class T>
    using ParamPrinter = void (*) (const char * name, const T& value, const void * userData);

    template <class T>
    void DefaultPrinter (const char * name, const T& value, const void * userData) {
        std::cout << name << ": " << value << std::endl;
    }

    template <class T>
    void NoPrinter (const char * name, const T& value, const void * userData) {}

    // TODO: Figure out how to make this a struct
    template <class V, class S>
    using ParamIncrementer = void (*) (bool sign, V& value, S step);

    template <class V, class S>
    void DefaultIncrementer (bool sign, V& value, S step) {
        if (sign) value += step;
        else value -= step;
    }
} // end namespace


// ParamPrinter
template <class T>
void VecPrinter (const char * name, const T& value, const void * userData){
    int len = T::length();
    std::cout << name << ": {";
    for (int i = 0; i < len; i++){
        std::cout << value[i];
        if (i < len - 1) std::cout << ", ";
    }
    std::cout << "}" << std::endl;
}

// ParamPrinter
template <class T, class V>
struct ListPrinter {
    static void print (const char * name, const T& value, const void * data) {
        const V* values = static_cast<const V*>(data);
        std::cout << name << ": " << values[value] << std::endl;
    }
};

template <
    class V,
    class S = typename std::remove_reference<V>::type>
class Param {
private:
    using RawType = typename std::remove_reference<V>::type;

    detail::ParamPrinter<RawType> printer;
    detail::ParamIncrementer<RawType,S> incrementer;

    const char * name;
    const void * userData;
    V value;
    S step;

public:

    Param () :
        value(),
        step(1),
        printer(detail::NoPrinter<RawType>),
        incrementer(detail::DefaultIncrementer<RawType,S>)
    {}

    Param (const char * name) :
        name(name),
        value(),
        step(1),
        printer(detail::DefaultPrinter<RawType>),
        incrementer(detail::DefaultIncrementer<RawType,S>)
    {}


    Param (const char * name, S step) :
        name(name),
        step(step),
        printer(detail::DefaultPrinter<RawType>),
        incrementer(detail::DefaultIncrementer<RawType,S>)
    {}

    Param (V value, S step) :
        value(value),
        step(step),
        printer(detail::NoPrinter<RawType>),
        incrementer(detail::DefaultIncrementer<RawType,S>)
    {}

    Param (const char * name, V value, S step) :
        name(name),
        value(value),
        step(step),
        printer(detail::DefaultPrinter<RawType>),
        incrementer(detail::DefaultIncrementer<RawType,S>)
    {}

    Param (const char * name, detail::ParamPrinter<RawType>&& printer, const void * data = nullptr) :
        name(name),
        step(1),
        printer(printer),
        incrementer(detail::DefaultIncrementer<RawType,S>),
        userData(data)
    {}

    Param (const char * name, S step, detail::ParamPrinter<RawType>&& printer, const void* data = nullptr) :
        name(name),
        step(step),
        printer(printer),
        incrementer(detail::DefaultIncrementer<RawType,S>),
        userData(data)
    {}

    Param (const char * name, V value, S step, detail::ParamPrinter<RawType>&& printer, const void * data = nullptr) :
        name(name),
        value(value),
        step(step),
        printer(printer),
        incrementer(detail::DefaultIncrementer<RawType,S>),
        userData(data)
    {}

    Param (const char * name, S step, detail::ParamPrinter<RawType>&& printer, detail::ParamIncrementer<RawType,S> incrementer, const void * data = nullptr) :
        name(name),
        step(step),
        printer(printer),
        incrementer(incrementer),
        userData(data)
    {}

    operator V() const { return value; }

    void operator++ () { increment(); }
    void operator-- () { decrement(); }

    void increment () {
        incrementer(true,value,step);
        printer(name,value, userData);
    }

    void decrement () {
        incrementer(false,value,step);
        printer(name,value,userData);
    }

    void set (const typename std::remove_reference<V>::type& newValue) {
        value = newValue;
        printer(name,value,userData);
    }

    RawType get () {
        return value;
    }
};


} // end namespace
