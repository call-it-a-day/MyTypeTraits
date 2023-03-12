#pragma once
#ifndef _MY_TYPE_TRAITS_
#define _MY_TYPE_TRAITS_
#include"basic_define.h"

MY_STD_BEGIN

template<bool val>
struct BoolConstant
{
	static constexpr bool value = val;
};
using TrueConstant = BoolConstant<true>;
using FalseConstant = BoolConstant<false>;

MY_STD_END
#endif//_MY_TYPE_TRAITS_