#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <fstream>

#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__; __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK(__Declaration__) __pragma(pack(push, 1)) __Declaration__; __pragma(pack(pop))
#endif

template<typename Test, template<typename...> class Ref>
struct is_specialization : std::false_type { };

template<template<typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type { };
