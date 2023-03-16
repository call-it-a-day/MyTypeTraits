#pragma once
#ifndef _MY_TYPE_TRAITS_
#define _MY_TYPE_TRAITS_

//该库实现了大部分标准库type_traits内容,采用的标准是c++17
//由于type_traits中的模板类,部分需要开洞实现,例如is_pod,is_move_constructible,故其中只实现了部分
//is_convertible,is_class,is_enum和is_empty,MSVC的实现都是开洞,
//我尝试着不开洞写了一版实现;其中is_empty明确检查出了无法修复的bug

#pragma warning(disable: 4180 4227)
//4180:对函数/引用加cv限定无意义.在实现is_function时用到了此特性
//4227:对引用加以cv是一种过时的做法
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
//据标准,其实现不能用折叠表达式,以防止代价高昂的实例化,故采用传统解包方法
IMPL_BEGIN
template<bool, typename...>
struct conjunctionImpl;

template<typename First>
struct conjunctionImpl<true, First> :BoolConstant<First::value> {};

template<typename First, typename...Rest>
struct conjunctionImpl<true, First, Rest...> :conjunctionImpl<First::value, Rest...> {};

template<typename...Rest>
struct conjunctionImpl<false, Rest...> :FalseConstant {};//短路实例化,终止递归
IMPL_END

template<typename...EmptyPack>
struct conjunction :TrueConstant {};//空包即为真

template<typename First>
struct conjunction<First> :IMPL conjunctionImpl<true, First> {};

template<typename First, typename...Rest>
struct conjunction<First, Rest...> :IMPL conjunctionImpl<First::value, Rest...> {};

template<typename...Tys>
inline constexpr bool conjunction_v = conjunction<Tys...>::value;

//negation
template<typename Ty>
struct negation :BoolConstant<!static_cast<bool>(Ty::value)> {};//防止B::value重载!先强转到bool

template <typename Ty>
inline constexpr bool negation_v = negation<Ty>::value;

//disjunction
//据标准,其实现不能用折叠表达式,以防止代价高昂的实例化,故采用传统解包方法
IMPL_BEGIN
template<bool, typename...>
struct disjunctionImpl;

template<typename First>
struct disjunctionImpl<false, First> :BoolConstant<First::value> {};

template<typename First, typename...Rest>
struct disjunctionImpl<false, First, Rest...> :disjunctionImpl<First::value, Rest...> {};

template<typename...Rest>
struct disjunctionImpl<true, Rest...> :TrueConstant {};//短路实例化,终止递归
IMPL_END

template<typename...EmptyPack>
struct disjunction :FalseConstant {};//空包即为假

template<typename First>
struct disjunction<First> :IMPL disjunctionImpl<false, First> {};

template<typename First, typename...Rest>
struct disjunction<First, Rest...> :IMPL disjunctionImpl<First::value, Rest...> {};

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

IMPL_BEGIN

//用于检测给定的类型是否在类型集合中,它不是标准库的一部分
template <typename Ty, typename... TypeSet>
inline constexpr bool is_t_in_typeSet = disjunction_v<is_same<Ty, TypeSet>...>;

IMPL_END

//is_integral
//这段基本是从stl直接抄的
template<typename Ty>
inline constexpr bool is_integral_v = IMPL is_t_in_typeSet<remove_cv_t<Ty>, bool, char, signed char, unsigned char,
#ifdef __cpp_char8_t
	char8_t,
#endif // __cpp_char8_t
	wchar_t, char16_t, char32_t,//这些奇怪char是什么东西啊,stl还用一个宏去限制了char8_t的编译
	short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

template <typename Ty>
struct is_integral : BoolConstant<is_integral_v<Ty>> {};//先定义值在定义类型,妙

//is_floating_point
template <typename Ty>
inline constexpr bool is_floating_point_v = IMPL is_t_in_typeSet<remove_cv_t<Ty>, float, double, long double>;

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
IMPL_BEGIN
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
IMPL_END

template <typename Ty>
struct add_lvalue_reference {
	using type = typename IMPL AddReferenceImpl<Ty>::lref;
};

template <typename Ty>
struct add_rvalue_reference {
	using type = typename IMPL AddReferenceImpl<Ty>::rref;
};

template <typename Ty>
using add_lvalue_reference_t = typename add_lvalue_reference<Ty>::type;

template <typename Ty>
using add_rvalue_reference_t = typename add_rvalue_reference<Ty>::type;

IMPL_BEGIN
template<typename...>
inline constexpr bool alwaysFalse = false;
IMPL_END

//declval
template<typename T>
typename add_rvalue_reference<T>::type declval() noexcept {
	static_assert(IMPL alwaysFalse<T>, "尝试调用declval");
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
IMPL_BEGIN
template <typename Ty, typename = void>
struct addPointerImpl {
	using type = Ty;
};

template <typename Ty>//不能提供一个额外的typename RefTo=remove_reference_t<Ty>,因为特化时不能带默认模板参数
struct addPointerImpl<Ty, void_t<remove_reference_t<Ty>*>> {//把无法添加指针的类型sfinae掉
	using type = remove_reference_t<Ty>*;//指向 T 或 T 所引用类型的指针
};
IMPL_END

template <typename Ty>
struct add_pointer {
	using type = typename IMPL addPointerImpl<Ty>::type;
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
IMPL_BEGIN
template <typename, typename>
constexpr FalseConstant isConvertibleImpl(...) noexcept { return FalseConstant{}; }

template <typename From, typename To>
constexpr decltype(static_cast<To>(declval<From>()), TrueConstant{}) isConvertibleImpl(int) noexcept { return TrueConstant{}; }
IMPL_END
template <typename From, typename To>
struct is_convertible :decltype(IMPL isConvertibleImpl<From, To>(0)) {};

template <typename From, typename To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

//is_enum
//MSVC的实现是开洞
template <typename Ty, typename = void>
struct is_enum {
	//先看能否向整型转换,再排除所有其他类型:算术,指针
	static constexpr bool value = is_convertible_v<Ty, int>
		&& !is_fundamental_v<Ty>
		&& !is_pointer_v<Ty>;
};

template <typename Ty>
struct is_enum<Ty, void_t<int Ty::*>> :FalseConstant {};//先检测是否是类类型

template <typename Ty>
inline constexpr bool is_enum_v = is_enum<Ty>::value;

//is_class
//MSVC的实现是开洞
//据标准,枚举类型也是类类型
template <typename Ty, typename = void>
struct is_class :BoolConstant<is_enum_v<Ty>> {};

template <typename Ty>
struct is_class<Ty, void_t<int Ty::*>> :TrueConstant {};

template <typename Ty>
inline constexpr bool is_class_v = is_class<Ty>::value;

//is_compound
template <typename Ty>
struct is_compound : BoolConstant<!is_fundamental_v<Ty>> {};

template <typename Ty>
inline constexpr bool is_compound_v = is_compound<Ty>::value;

//is_member_function_pointer
IMPL_BEGIN
template <typename Ty>
struct is_member_function_pointer_impl :FalseConstant {};

template <typename Class_t, typename Ret, typename...Args>
struct is_member_function_pointer_impl<Ret(Class_t::*)(Args...)> :TrueConstant {};
IMPL_END

template <typename Ty>
struct is_member_function_pointer :IMPL is_member_function_pointer_impl<remove_cv_t<Ty>> {};

template <typename Ty>
inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<Ty>::value;

//is_const
//如果参数是const或const volatile,type均为真
template <typename Ty>
inline constexpr bool is_const_v = false;

template <typename Ty>
inline constexpr bool is_const_v<const Ty> = true;//变量模板可以偏特化

template <typename Ty>
struct is_const :BoolConstant<is_const_v<Ty>> {};

//is_volatile
//如果参数是volatile或const volatile,type均为真
template <typename Ty>
inline constexpr bool is_volatile_v = false;

template <typename Ty>
inline constexpr bool is_volatile_v<volatile Ty> = true;

template <typename Ty>
struct is_volatile :BoolConstant<is_volatile_v<Ty>> {};

//is_function
//取巧的做法,仅函数或引用类型无法被cv限定
template <typename Ty>
struct is_function
{
	static constexpr bool value = !is_const_v<const Ty> && !is_reference_v<Ty>;
};

template <typename Ty>
inline constexpr bool is_function_v = is_function<Ty>::value;

//is_object
//若 T 为对象类型（即任何函数、引用或 void 类型外的可有 cv 限定的类型
//仅函数或引用类型无法被cv限定
template <typename Ty>
struct is_object
{
	static constexpr bool value = is_const_v<const Ty> && !is_void_v<Ty>;
};

template <typename Ty>
inline constexpr bool is_object_v = is_object<Ty>::value;

//is_member_object_pointer
IMPL_BEGIN
template <typename>
struct is_member_object_pointer_impl
{
	static constexpr bool value = false;
};

template <typename Class_t, typename Object_t>
struct is_member_object_pointer_impl<Object_t Class_t::*>
{
	static constexpr bool value = !is_function_v<Object_t>;
	//符合Object_t Class_t::*形式的只可能是成员函数指针或是成员对象指针
	//最初实现is_member_object_pointer和is_member_function_pointer时我回忆起深探上讲起过这俩指针的本质
	//在于成员对象指针其实是个索引,取其值会发现其实是个整数而非地址
	//而成员函数指针则因为可能是个虚表索引,也可能是个地址,故而编译器会为其分配一个结构体来确定其值
	//起初想从其本质下文章,还好看了下msvc的实现...真简单
};
IMPL_END

template <typename Ty>
struct is_member_object_pointer
{
	static constexpr bool value = IMPL is_member_object_pointer_impl<remove_cv_t<Ty>>::value;
};

template <typename Ty>
inline constexpr bool is_member_object_pointer_v = is_member_object_pointer<Ty>::value;

//is_member_pointer
template <typename Ty>
struct is_member_pointer
{
	static constexpr bool value = is_member_object_pointer_v<Ty> || is_member_function_pointer_v<Ty>;
};

template <typename Ty>
inline constexpr bool is_member_pointer_v = is_member_pointer<Ty>::value;

//is_scalar
template <typename Ty>
struct is_scalar
{
	//下列类型统称为标量类型：
	//算术类型,枚举类型,指针类型,成员指针类型,std::nullptr_t,这些类型的有 cv 限定版本
	using RemoveCVTy = remove_cv_t<Ty>;
	static constexpr bool value = disjunction_v<is_arithmetic<RemoveCVTy>, is_enum<RemoveCVTy>,
		is_pointer<RemoveCVTy>, is_member_pointer<Ty>, is_null_pointer<Ty>>;
};

template <typename Ty>
inline constexpr bool is_scalar_v = is_scalar<Ty>::value;

//is_empty
//MSVC的实现是开洞

//下面是个错误的实现,因为sizeof(空类)得到1,虽然sizeof是不求值表达式,但空类必须实例化,必须有唯一地址
//template <typename Ty>
//struct is_empty
//{
//	static constexpr bool value = (sizeof(Ty) == 0);
//};
//
//template <typename Ty>
//inline constexpr bool is_empty_v = is_empty<Ty>::value;

//这个实现的想法是利用空基类优化
//但它有个bug,要求参数一定要能被继承,即对于final类会编译错误,所以只能开洞了
IMPL_BEGIN
template <typename, typename = void>
struct is_empty_impl {};

template <typename Ty>
struct is_empty_impl <Ty, void_t<enable_if_t<is_class_v<Ty>>>> :Ty {//防止Ty是非法基类
	int m;//m的大小只要比1字节大就行
};

//struct EmptyClass {};

IMPL_END

template <typename Ty>
//考虑工程的写法
//inline constexpr bool is_empty_v = (sizeof(IMPL is_empty_impl<Ty>) == sizeof(IMPL is_empty_impl<IMPL EmptyClass>));
//考虑效率的写法
inline constexpr bool is_empty_v = (sizeof(IMPL is_empty_impl<Ty>) == sizeof(int));

template <typename Ty>
struct is_empty :BoolConstant<is_empty_v<Ty>> {};

//is_signed
template <typename Ty, typename = void>
struct is_signed {
	static constexpr bool value = false;
};

template <typename Ty>
struct is_signed<Ty, enable_if_t<is_arithmetic_v<Ty>>> {
	using RemoveCVTy = remove_cv_t<Ty>;
	static constexpr bool value = (RemoveCVTy(-1) < RemoveCVTy(0));
};

template <typename Ty>
inline constexpr bool is_signed_v = is_signed<Ty>::value;

//is_unsigned
template <typename Ty, typename = void>
struct is_unsigned {
	static constexpr bool value = false;
};

template <typename Ty>
struct is_unsigned<Ty, enable_if_t<is_arithmetic_v<Ty>>> {
	using RemoveCVTy = remove_cv_t<Ty>;
	static constexpr bool value = (RemoveCVTy(0) < RemoveCVTy(-1));
};

template <typename Ty>
inline constexpr bool is_unsigned_v = is_unsigned<Ty>::value;

//alignment_of
//学了快两年cpp还是头一次听说这玩意
//提供等于 T 类型对齐要求的成员常量 value ，如同用 alignof 表达式获得。若 T 是数组类型，
//则返回元素类型(试了一下,对于多维数组,得到的不是alignof指针)的对齐要求，若 T 是引用类型，则返回备用用类型的对齐要求。
//据标准,若 alignof(T) 不是合法表达式，则行为未定义。故只需考虑成功情形
//若T为枚举类型,则返回其底层类型
template <typename Ty>
struct alignment_of {
	static constexpr size_t value = alignof(remove_all_extents_t<remove_reference_t<remove_cv_t<Ty>>>);
};

template <typename Ty>
inline constexpr size_t alignment_of_v = alignment_of<Ty>::value;

//aligned_storage,aligned_union已经被弃用了,就偷个懒不实现了
//rank
template <typename Ty>
inline constexpr size_t rank_v = 0;

template <typename Ty>
inline constexpr size_t rank_v<Ty[]> = rank_v<Ty>+1;

template <typename Ty, size_t sz>
inline constexpr size_t rank_v<Ty[sz]> = rank_v<Ty>+1;

template <typename Ty>
struct rank {
	static constexpr size_t value = rank_v<Ty>;
};

//extent
template <typename Ty, unsigned N = 0>
inline constexpr size_t extent_v = 0;

template <typename Ty, size_t Eles>
inline constexpr size_t extent_v<Ty[Eles], 0> = Eles;//这里获取元素数量

//只对数组继续递归
template <typename Ty, size_t Eles, unsigned N>
inline constexpr size_t extent_v <Ty[Eles], N> = extent_v<Ty, N - 1>;

template <typename Ty, unsigned N>
inline constexpr size_t extent_v <Ty[], N> = extent_v<Ty, N - 1>;

template <typename Ty, unsigned N = 0>
struct extent {
	static constexpr size_t value = extent_v<Ty, N>;
};

//decay
//对类型 T 应用左值到右值、数组到指针及函数到指针隐式转换，移除 cv 限定符，
//◦若 T 指名“ U 的数组”或“到 U 的数组的引用”类型，则成员 typedef type 为 U* 。
//◦否则，若 T 为函数类型 F 或到它的引用，则成员 typedef type 为std::add_pointer<F>::type 。
//◦否则，成员 typedef type 为 std::remove_cv<std::remove_reference<T>::type>::type 。
IMPL_BEGIN
template <typename Ty, typename = void>
struct decay_impl {
	using type = Ty;
};

template <typename Ty>
struct decay_impl<Ty, enable_if_t<is_array_v<Ty> || is_function_v<Ty>>> {
	using type = add_pointer_t<remove_extent_t<Ty>>;
};
IMPL_END

template <typename Ty>
struct decay {
	using type = typename IMPL decay_impl<remove_cv_t<remove_reference_t<Ty>>>::type;
};

template <typename Ty>
using decay_t = typename decay<Ty>::type;

//common_type
template< typename... EmptyPack >
struct common_type {};

template<typename SingleTy>
struct common_type<SingleTy> {
	//◦若 sizeof...(T) 为一（即 T... 只含一个类型 T0 ），
	//则成员 type 指名与 std::common_type<T0, T0>::type 相同的类型，若它存在；否则无成员 type 
	//以上是标准的定义,但我不是很理解这样做的深意,msvc的实现是:
	//template <class _Ty1>
	//struct common_type<_Ty1> : common_type<_Ty1, _Ty1> {};
	using type = SingleTy;
};

IMPL_BEGIN
template<typename Ty1, typename Ty2,typename=void>
struct common_type_2_impl {};

template<typename Ty1, typename Ty2>
struct common_type_2_impl<Ty1,Ty2, void_t<decay_t<decltype(false ? declval<Ty1>() : declval<Ty2>())>>> {
	using type = decay_t<decltype(false ? declval<Ty1>() : declval<Ty2>())>;
	//原来三目运算符的返回类型是俩表达式的公共类型
};
IMPL_END

//◦若应用 std::decay 到至少 T1 与 T2 中至少一个类型后产生相异类型，则成员 type 指名与
//std::common_type<std::decay<T1>::type, std::decay<T2>::type>::type 相同的类型
template<typename Ty1, typename Ty2>
struct common_type<Ty1, Ty2> : IMPL common_type_2_impl<decay_t<Ty1>, decay_t<Ty2>, void> {};

IMPL_BEGIN
template<typename,typename Ty1, typename Ty2, typename... Rest>
struct common_type_3_impl {};

template<typename Ty1, typename Ty2, typename... Rest>
struct common_type_3_impl<void_t<typename common_type<Ty1, Ty2>::type>, Ty1, Ty2, Rest...> :
	common_type<typename common_type<Ty1, Ty2>::type, Rest...> {};//探测是否有typename common_type<Ty1, Ty2>::type
IMPL_END

//◦若 sizeof...(T) 大于二（即 T... 由类型 T1, T2, R... 组成），则若 std::common_type<T1, T2>::type 存在，
//则成员 type 指代 std::common_type<std::common_type<T1, T2>::type, R...>::type ，若存在这种类型。
//其他所有情况下，无成员 type 。
template<typename Ty1, typename Ty2,typename... Rest>
struct common_type<Ty1, Ty2, Rest...> :IMPL common_type_3_impl<void, Ty1, Ty2, Rest...> {};

template <typename... Tys>
using common_type_t = typename common_type<Tys...>::type;

//forward
template<typename Ty>
NODISCARD constexpr Ty&& forward(remove_reference_t<Ty>& arg) noexcept {
	return static_cast<Ty&&>(arg);
}

//forward<左值引用>(右值)非法
template<typename Ty>
NODISCARD constexpr enable_if_t<!is_lvalue_reference_v<Ty>, Ty&&> forward(remove_reference_t<Ty>&& arg) noexcept {
	return static_cast<Ty&&>(arg);
}

//move
template<typename Ty>
NODISCARD constexpr remove_reference_t<Ty>&& move(Ty&& arg) noexcept { // forward _Arg as movable
	return static_cast<remove_reference_t<Ty>&&>(arg);
}

//remove_cvref
template<typename Ty>
struct remove_cvref
{
	using type = remove_cv_t<remove_reference_t<Ty>>;
};

template<typename Ty>
using remove_cvref_t = typename remove_cvref<Ty>::type;

//invoke
//template< typename F, class... Args >
//
//constexpr invoke_result_t<F, Args...>invoke(F&& f, Args&&... args) noexcept {
//
//}

MY_STD_END
#endif//_MY_TYPE_TRAITS_