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

//is_integral
//��λ����Ǵ�stlֱ�ӳ���
template<typename Ty>
inline constexpr bool is_integral_v = is_t_in_typeSet<remove_cv_t<Ty>, bool, char, signed char, unsigned char,
#ifdef __cpp_char8_t
	char8_t,
#endif // __cpp_char8_t
	wchar_t,char16_t, char32_t,//��Щ���char��ʲô������,stl����һ����ȥ������char8_t�ı���
	short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

template <typename Ty>
struct is_integral : BoolConstant<is_integral_v<Ty>> {};//�ȶ���ֵ�ڶ�������,��

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
	static constexpr bool value = is_same_v<Ty, void>;
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
template<typename Ty,typename=void>
struct AddReferenceImpl
{
	//���ڲ��ɴ������õ�����,����void,�ݱ�׼,�ṩԴ����.
	using lref = Ty;
	using rref = Ty;
};

template<typename Ty>
struct AddReferenceImpl<Ty, void_t<Ty&>>//����̽��
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
	static_assert(alwaysFalse<T>,"���Ե���declval");
	//���ֱ����false,��ʹû��ʵ����declval,Ҳ�ᴥ��static_assert
	//��Ҫ��һ������ģ�彫static_assert�ӳٵ�T������ʵ����
}

//remove_extent,remove_all_extents
template<typename Ty>
struct remove_extent//����������
{
	using type = Ty;
};

template<typename Ty>
struct remove_extent<Ty[]>//�޽�����
{
	using type = Ty;
};

template<typename Ty,size_t sz>
struct remove_extent<Ty[sz]>//�н�����
{
	using type = Ty;
};

template<typename Ty>
struct remove_all_extents//����������
{
	using type = Ty;
};

template<typename Ty>
struct remove_all_extents<Ty[]>//�޽�����
{
	using type = typename remove_all_extents<Ty>::type;
};

template<typename Ty, size_t sz>
struct remove_all_extents<Ty[sz]>//�н�����
{
	using type = typename remove_all_extents<Ty>::type;
};

template<typename Ty>
using remove_extent_t = typename remove_extent<Ty>::type;

template<typename Ty>
using remove_all_extents_t = typename remove_all_extents<Ty>::type;

//remove_pointer
//�ݱ�׼,remove_pointer�����������Ч
//��ָ��,type������ָ�������,�ʻ���Եײ�cv�޶�,��������cv�޶�
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
template <typename Ty,typename = void>
struct addPointerImpl {
	using type = Ty;
};

template <typename Ty>//�����ṩһ�������typename RefTo=remove_reference_t<Ty>,��Ϊ�ػ�ʱ���ܴ�Ĭ��ģ�����
struct addPointerImpl<Ty, void_t<remove_reference_t<Ty>*>> {//���޷����ָ�������sfinae��
	using type = remove_reference_t<Ty>*;//ָ�� T �� T ���������͵�ָ��
};

template <typename Ty>
struct add_pointer {
	using type = typename addPointerImpl<Ty>::type;
};

template <typename Ty>
using add_pointer_t = typename add_pointer<Ty>::type;

MY_STD_END
#endif//_MY_TYPE_TRAITS_