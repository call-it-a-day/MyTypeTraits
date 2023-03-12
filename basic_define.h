#pragma once

#define MY_STD_BEGIN namespace myStd {
#define MY_STD_END }
#define IS_CXX_GREATER_THAN_CPP14 (__cplusplus >= 201402L || _MSVC_LANG >= 201402L)
#define IS_CXX_GREATER_THAN_CPP17 (__cplusplus >= 201703L || _MSVC_LANG >= 201703L)
