#pragma once
#ifndef _MY_TYPE_TRAITS_
#define _MY_TYPE_TRAITS_
#include"basic_define.h"

MY_STD_BEGIN

//以下需要开洞,故未实现:
//is_union,is_constant_evaluated

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
//据标准,其实现不能用折叠表达式,以防止代价高昂的实例化,故采用传统解包方法
template<bool, typename...>
struct conjunctionImpl;

template<typename First>
struct conjunctionImpl<true, First> :BoolConstant<First::value> {};

template<typename First, typename...Rest>
struct conjunctionImpl<true, First, Rest...> :conjunctionImpl<First::value, Rest...> {};

template<typename...Rest>
struct conjunctionImpl<false, Rest...> :FalseConstant {};//短路实例化,终止递归

template<typename...EmptyPack>
struct conjunction :TrueConstant {};//空包即为真

template<typename First>
struct conjunction<First> :conjunctionImpl<true, First> {};

template<typename First, typename...Rest>
struct conjunction<First, Rest...> :conjunctionImpl<First::value, Rest...> {};

template<typename...Tys>
inline constexpr bool conjunction_v = conjunction<Tys...>::value;

//negation
template<typename Ty>
struct negation :BoolConstant<!static_cast<bool>(Ty::value)> {};//防止B::value重载!先强转到bool

template <typename Ty>
inline constexpr bool negation_v = negation<Ty>::value;

//disjunction
//据标准,其实现不能用折叠表达式,以防止代价高昂的实例化,故采用传统解包方法
template<bool, typename...>
struct disjunctionImpl;

template<typename First>
struct disjunctionImpl<false, First> :BoolConstant<First::value> {};

template<typename First, typename...Rest>
struct disjunctionImpl<false, First, Rest...> :disjunctionImpl<First::value, Rest...> {};

template<typename...Rest>
struct disjunctionImpl<true, Rest...> :TrueConstant {};//短路实例化,终止递归

template<typename...EmptyPack>
struct disjunction :FalseConstant {};//空包即为假

template<typename First>
struct disjunction<First> :disjunctionImpl<false, First> {};

template<typename First, typename...Rest>
struct disjunction<First, Rest...> :disjunctionImpl<First::value, Rest...> {};

template<typename...Tys>
inline constexpr bool disjunction_v = disjunction<Tys...>::value;

//enable_if
template <bool Test, typename Ty = void>
struct enable_if {};//test为假时,无value成员

template <typename Ty>
struct enable_if<true, Ty> {
	using type = Ty;
};

template <bool Test, typename Ty = void>
using enable_if_t = typename enable_if<Test, Ty>::type;

//conditional
template <bool Test, typename Ty1, typename Ty2>
struct conditional {
	using type = Ty1;//Test为真时选择Ty1
};

template <typename Ty1, typename Ty2>
struct conditional<false, Ty1, Ty2> {
	using type = Ty2;//Test为真时选择Ty2
};

template <bool Test, typename Ty1, typename Ty2>
using conditional_t = typename conditional<Test, Ty1, Ty2>::type;

//is_same
template <typename Ty1, typename Ty2>
struct is_same {
	static constexpr bool value = false;//Ty1与Ty2不同时提供false
};

template <typename Ty>
struct is_same<Ty, Ty> {
	static constexpr bool value = true;//Ty1与Ty2相同时提供true
};

template <typename Ty1, typename Ty2>
inline constexpr bool is_same_v = is_same<Ty1, Ty2>::value;

//remove_const
template <typename Ty>
struct remove_const { //非const实参,保持其原本类型
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
struct remove_volatile { //非volatile实参,保持其原本类型
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
struct remove_cv { //非const/volatile实参,保持其原本类型
	using type = remove_volatile_t<remove_const_t<Ty>>;
};

template <typename Ty>
using remove_cv_t = typename remove_cv<Ty>::type;

UNNAMED_NAMESPACE_BEGIN

//用于检测给定的类型是否在类型集合中,它不是标准库的一部分
template <typename Ty, typename... TypeSet>
inline constexpr bool is_t_in_typeSet = disjunction_v<is_same<Ty, TypeSet>...>;

UNNAMED_NAMESPACE_END

//is_integral
//这段基本是从stl直接抄的
template<typename Ty>
inline constexpr bool is_integral_v = is_t_in_typeSet<remove_cv_t<Ty>, bool, char, signed char, unsigned char,
#ifdef __cpp_char8_t
	char8_t,
#endif // __cpp_char8_t
	wchar_t, char16_t, char32_t,//这些奇怪char是什么东西啊,stl还用一个宏去限制了char8_t的编译
	short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

template <typename Ty>
struct is_integral : BoolConstant<is_integral_v<Ty>> {};//先定义值在定义类型,妙

//is_floating_point
template <typename Ty>
inline constexpr bool is_floating_point_v = is_t_in_typeSet<remove_cv_t<Ty>, float, double, long double>;

template <typename Ty>
struct is_floating_point : BoolConstant<is_floating_point_v<Ty>> {};

//is_arithmetic
template <typename Ty>
struct is_arithmetic {
	static constexpr bool value = is_integral_v<Ty> or is_floating_point_v<Ty>;
};

template <typename Ty>
inline constexpr bool is_arithmetic_v = is_arithmetic<Ty>::value;

//remove_reference
template<typename Ty>
struct remove_reference
{
	using type = Ty;
};

template<typename Ty>
struct remove_reference<Ty&>
{
	using type = Ty;
};

template<typename Ty>
struct remove_reference<Ty&&>
{
	using type = Ty;
};

template <typename Ty>
using remove_reference_t = typename remove_reference<Ty>::type;

//is_void
template<typename Ty>
struct is_void
{
	//检查 T 是否为 void 类型。若 T 是类型 void 、 const void 、 volatile void 或 const volatile void ，
	//则提供等于 true 的成员常量 value。
	//虽然不知道cv限定的void有什么用,估测只是模板类型加工的中间产物
	static constexpr bool value = is_same_v<remove_cv_t<Ty>, void>;
};

template <typename Ty>
inline constexpr bool is_void_v = is_void<Ty>::value;

//void_t
template <typename...>
using void_t = void;

//add_const,add_volatile,add_cv
template <typename Ty>
struct add_const {
	using type = const Ty;
};

template <typename Ty>
using add_const_t = typename add_const<Ty>::type;

template <typename Ty>
struct add_volatile {
	using type = volatile Ty;
};

template <typename Ty>
using add_volatile_t = typename add_volatile<Ty>::type;

template <typename Ty>
struct add_cv {
	using type = const volatile Ty;
};

template <typename Ty>
using add_cv_t = typename add_cv<Ty>::type;

//add_lvalue_reference,add_rvalue_reference
template<typename Ty, typename = void>
struct AddReferenceImpl
{
	//对于不可创建引用的类型,例如void,据标准,提供源类型.
	using lref = Ty;
	using rref = Ty;
};

template<typename Ty>
struct AddReferenceImpl<Ty, void_t<Ty&>>//类型探测
{
	using lref = Ty&;
	using rref = Ty&&;
};

template <typename Ty>
struct add_lvalue_reference {
	using type = typename AddReferenceImpl<Ty>::lref;
};

template <typename Ty>
struct add_rvalue_reference {
	using type = typename AddReferenceImpl<Ty>::rref;
};

template <typename Ty>
using add_lvalue_reference_t = typename add_lvalue_reference<Ty>::type;

template <typename Ty>
using add_rvalue_reference_t = typename add_rvalue_reference<Ty>::type;

UNNAMED_NAMESPACE_BEGIN
template<typename...>
inline constexpr bool alwaysFalse = false;
UNNAMED_NAMESPACE_END

//declval
template<typename T>
typename add_rvalue_reference<T>::type declval() noexcept {
	static_assert(alwaysFalse<T>, "尝试调用declval");
	//如果直接用false,即使没有实例化declval,也会触发static_assert
	//需要用一个变量模板将static_assert延迟到T真正被实例化
}

//remove_extent,remove_all_extents
template<typename Ty>
struct remove_extent//非数组类型
{
	using type = Ty;
};

template<typename Ty>
struct remove_extent<Ty[]>//无界数组
{
	using type = Ty;
};

template<typename Ty, size_t sz>
struct remove_extent<Ty[sz]>//有界数组
{
	using type = Ty;
};

template<typename Ty>
struct remove_all_extents//非数组类型
{
	using type = Ty;
};

template<typename Ty>
struct remove_all_extents<Ty[]>//无界数组
{
	using type = typename remove_all_extents<Ty>::type;
};

template<typename Ty, size_t sz>
struct remove_all_extents<Ty[sz]>//有界数组
{
	using type = typename remove_all_extents<Ty>::type;
};

template<typename Ty>
using remove_extent_t = typename remove_extent<Ty>::type;

template<typename Ty>
using remove_all_extents_t = typename remove_all_extents<Ty>::type;

//remove_pointer
//据标准,remove_pointer不会对数组生效
//对指针,type是其所指向的类型,故会忽略底层cv限定,保留顶层cv限定
template <typename Ty>
struct remove_pointer {
	using type = Ty;
};

template <typename Ty>
struct remove_pointer<Ty*> {
	using type = Ty;
};

template <typename Ty>
struct remove_pointer<Ty* const> {
	using type = Ty;
};

template <typename Ty>
struct remove_pointer<Ty* volatile> {
	using type = Ty;
};

template <typename Ty>
struct remove_pointer<Ty* const volatile> {
	using type = Ty;
};

template <typename Ty>
using remove_pointer_t = typename remove_pointer<Ty>::type;

//add_pointer
template <typename Ty, typename = void>
struct addPointerImpl {
	using type = Ty;
};

template <typename Ty>//不能提供一个额外的typename RefTo=remove_reference_t<Ty>,因为特化时不能带默认模板参数
struct addPointerImpl<Ty, void_t<remove_reference_t<Ty>*>> {//把无法添加指针的类型sfinae掉
	using type = remove_reference_t<Ty>*;//指向 T 或 T 所引用类型的指针
};

template <typename Ty>
struct add_pointer {
	using type = typename addPointerImpl<Ty>::type;
};

template <typename Ty>
using add_pointer_t = typename add_pointer<Ty>::type;

//is_array
template <typename Ty>
struct is_array {
	static constexpr bool value = false;
};

template <typename Ty>
struct is_array<Ty[]> {
	static constexpr bool value = true;
};

template <typename Ty, size_t sz>
struct is_array<Ty[sz]> {
	static constexpr bool value = true;
};

template <typename Ty>
inline constexpr bool is_array_v = is_array<Ty>::value;

//is_reference
template <typename Ty>
struct is_lvalue_reference {
	static constexpr bool value = false;
};

template <typename Ty>
struct is_lvalue_reference<Ty&> {
	static constexpr bool value = true;
};

template <typename Ty>
struct is_rvalue_reference {
	static constexpr bool value = false;
};

template <typename Ty>
struct is_rvalue_reference<Ty&&> {
	static constexpr bool value = true;
};

template <typename Ty>
struct is_reference {
	static constexpr bool value = disjunction_v<is_lvalue_reference<Ty>, is_rvalue_reference<Ty>>;
};

template <typename Ty>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<Ty>::value;

template <typename Ty>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<Ty>::value;

template <typename Ty>
inline constexpr bool is_reference_v = is_reference<Ty>::value;

//is_pointer
template <typename>
inline constexpr bool is_pointer_v = false; // determine whether _Ty is a pointer

template <typename Ty>
inline constexpr bool is_pointer_v<Ty*> = true;

template <typename Ty>
inline constexpr bool is_pointer_v<Ty* const> = true;

template <typename Ty>
inline constexpr bool is_pointer_v<Ty* volatile> = true;

template <typename Ty>
inline constexpr bool is_pointer_v<Ty* const volatile> = true;

template <typename Ty>
struct is_pointer : BoolConstant<is_pointer_v<Ty>> {};

//is_null_pointer
template <typename Ty>
struct is_null_pointer {
	//据标准,若 T 为 std::nullptr_t 、 const std::nullptr_t 、 volatile std::nullptr_t
	//或 const volatile std::nullptr_t 类型，则提供等于 true 的成员常量 value 。
	//所以Ty实际期望的并不是指针类型,而是nullptr_t及其cv版本
	//在一开始的实现我犯了个错误,望文生义以为这是个函数,期望指针并检查指针是否指向空
	static constexpr bool value = is_same_v<remove_cv_t<Ty>, std::nullptr_t>;
};

template <typename Ty>
inline constexpr bool is_null_pointer_v = is_null_pointer<Ty>::value;

//is_fundamental
template <typename Ty>
struct is_fundamental {
	//若 T 为基础类型（即算术类型、 void 或 nullptr_t ），则提供等于 true 的成员常量 value 。
	static constexpr bool value = disjunction_v<is_arithmetic<Ty>, is_null_pointer<Ty>, is_void<Ty>>;
	//从写库的角度,因为这三个模板内部都会移除cv,所以无需在此处remove_cv_t<Ty>
	//从开发的角度,建议写上
};

template <typename Ty>
inline constexpr bool is_fundamental_v = is_fundamental<Ty>::value;

//is_convertible
//MSVC的实现是开洞

template <typename, typename>
constexpr FalseConstant isConvertibleImpl(...) noexcept { return FalseConstant{}; }

template <typename From, typename To>
constexpr decltype(static_cast<To>(declval<From>()), TrueConstant{}) isConvertibleImpl(int) noexcept { return TrueConstant{}; }

template <typename From, typename To>
struct is_convertible :decltype(isConvertibleImpl<From, To>(0)) {};

template <typename From, typename To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

//is_class
//MSVC的实现是开洞
//据标准,枚举类型也是类类型
template <typename Ty,typename=void>
struct is_class :FalseConstant {};

template <typename Ty>
struct is_class<Ty,void_t<int Ty::*>> :TrueConstant {};

template <typename Ty>
inline constexpr bool is_class_v = is_class<Ty>::value;

//is_enum

MY_STD_END
#endif//_MY_TYPE_TRAITS_