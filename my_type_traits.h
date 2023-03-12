#pragma once
#ifndef _MY_TYPE_TRAITS_
#define _MY_TYPE_TRAITS_
#include"basic_define.h"

MY_STD_BEGIN

template<bool val>
struct BoolConstant
{
	static constexpr bool value = val;
	constexpr operator bool()const noexcept {
		return value;
	}
};
using TrueConstant = BoolConstant<true>;
using FalseConstant = BoolConstant<false>;

//conjunction
//�ݱ�׼,��ʵ�ֲ������۵����ʽ,�Է�ֹ���۸߰���ʵ����,�ʲ��ô�ͳ�������
template<bool,typename...>
struct conjunctionImpl;

template<typename First>
struct conjunctionImpl<true, First> :BoolConstant<First::value> {};

template<typename First, typename...Rest>
struct conjunctionImpl<true, First, Rest...> :conjunctionImpl<First::value, Rest...> {};

template<typename...Rest>
struct conjunctionImpl<false, Rest...> :FalseConstant {};//��·ʵ����,��ֹ�ݹ�

template<typename...EmptyPack>
struct conjunction :TrueConstant {};//�հ���Ϊ��

template<typename First>
struct conjunction<First> :conjunctionImpl<true, First> {};

template<typename First,typename...Rest>
struct conjunction<First, Rest...> :conjunctionImpl<First::value, Rest...> {};

template<typename...Tys>
inline constexpr bool conjunction_v = conjunction<Tys...>::value;

//negation
template<typename Ty>
struct negation :BoolConstant<!static_cast<bool>(Ty::value)> {};//��ֹB::value����!��ǿת��bool

template <typename Ty>
inline constexpr bool negation_v = negation<Ty>::value;

//disjunction
//�ݱ�׼,��ʵ�ֲ������۵����ʽ,�Է�ֹ���۸߰���ʵ����,�ʲ��ô�ͳ�������
template<bool, typename...>
struct disjunctionImpl;

template<typename First>
struct disjunctionImpl<false, First> :BoolConstant<First::value> {};

template<typename First, typename...Rest>
struct disjunctionImpl<false, First, Rest...> :disjunctionImpl<First::value, Rest...> {};

template<typename...Rest>
struct disjunctionImpl<true, Rest...> :TrueConstant {};//��·ʵ����,��ֹ�ݹ�

template<typename...EmptyPack>
struct disjunction :FalseConstant {};//�հ���Ϊ��

template<typename First>
struct disjunction<First> :disjunctionImpl<false, First> {};

template<typename First, typename...Rest>
struct disjunction<First, Rest...> :disjunctionImpl<First::value, Rest...> {};

template<typename...Tys>
inline constexpr bool disjunction_v = disjunction<Tys...>::value;

//enable_if
template <bool Test, typename Ty = void>
struct enable_if {};//testΪ��ʱ,��value��Ա

template <typename Ty>
struct enable_if<true, Ty> {
	using type = Ty;
};

template <bool Test, typename Ty = void>
using enable_if_t = typename enable_if<Test, Ty>::type;

//conditional
template <bool Test, typename Ty1, typename Ty2>
struct conditional { 
	using type = Ty1;//TestΪ��ʱѡ��Ty1
};

template <typename Ty1, typename Ty2>
struct conditional<false, Ty1, Ty2> {
	using type = Ty2;//TestΪ��ʱѡ��Ty2
};

template <bool Test, typename Ty1, typename Ty2>
using conditional_t = typename conditional<Test, Ty1, Ty2>::type;

//is_same
template <typename Ty1, typename Ty2>
struct is_same {
	static constexpr bool value = false;//Ty1��Ty2��ͬʱ�ṩfalse
};

template <typename Ty>
struct is_same<Ty,Ty> {
	static constexpr bool value = true;//Ty1��Ty2��ͬʱ�ṩtrue
};

template <typename Ty1, typename Ty2>
inline constexpr bool is_same_v = is_same<Ty1, Ty2>::value;

//remove_const
template <typename Ty>
struct remove_const { //��constʵ��,������ԭ������
	using type = Ty;
};

template <typename Ty>
struct remove_const<const Ty> {
	using type = Ty;
};

template <typename Ty>
using remove_const_t = typename remove_const<Ty>::type;

//remove_volatile
template <typename Ty>
struct remove_volatile { //��volatileʵ��,������ԭ������
	using type = Ty;
};

template <typename Ty>
struct remove_volatile<volatile Ty> {
	using type = Ty;
};

template <typename Ty>
using remove_volatile_t = typename remove_volatile<Ty>::type;

//remove_cv
template <typename Ty>
struct remove_cv { //��const/volatileʵ��,������ԭ������
	using type = remove_volatile_t<remove_const_t<Ty>>;
};

template <typename Ty>
using remove_cv_t = typename remove_cv<Ty>::type;

#if(IS_CXX_GREATER_THAN_CPP20)
constexpr bool is_constant_evaluated() noexcept;//��Ҫ����
#endif

UNNAMED_NAMESPACE_BEGIN

//���ڼ������������Ƿ������ͼ�����,�����Ǳ�׼���һ����
template <typename Ty, typename... TypeSet>
inline constexpr bool is_t_in_typeSet = disjunction_v<is_same<Ty, TypeSet>...>;

UNNAMED_NAMESPACE_END

MY_STD_END
#endif//_MY_TYPE_TRAITS_