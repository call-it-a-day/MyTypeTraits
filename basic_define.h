#pragma once

#define MY_DEBUG true
#define MY_STD_BEGIN namespace myStd {
#define MY_STD_END }

#if(MY_DEBUG)
#define UNNAMED_NAMESPACE_BEGIN 
#define UNNAMED_NAMESPACE_END 
#else
#define UNNAMED_NAMESPACE_BEGIN namespace {
#define UNNAMED_NAMESPACE_END }
#endif

#define IS_CXX_GREATER_THAN_CPP14 (__cplusplus >= 201402L || _MSVC_LANG >= 201402L)
#define IS_CXX_GREATER_THAN_CPP17 (__cplusplus >= 201703L || _MSVC_LANG >= 201703L)
#define IS_CXX_GREATER_THAN_CPP20 (__cplusplus >= 202002L || _MSVC_LANG >= 202002L)