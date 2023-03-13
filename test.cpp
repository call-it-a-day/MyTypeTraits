#include"my_type_traits.h"
#include<iostream>
using std::cout;
using namespace myStd;

template<typename...Args>
void test(const Args&...args) {
	((cout << args << '\t'), ...);
	cout << '\n';
}

struct TestClass
{
	void f(int, int) {
		cout << 1;
	}
};

class Base {};
class Derived : public Base {};

enum class TestEnum{};

int main() {
	//��������
	int i = 42;
	int& lrefi = i;
	int&& rrefi = 42;
	int arri[]{ 1,2,3 };
	const int ci = 42;
	volatile int vi = 42;
	const volatile int cvi = 42;
	int* pi = &i;
	const int* pci = &i;
	int* const cpi = &i;
	const char* pstr = "hello";
	char arrstr[] = "hello";

	//���Բ���
	cout << std::boolalpha;
	cout << "��ʼ����\n";
	//����BoolConstant
	test(TrueConstant::value, FalseConstant::value);
	cout << "\n";
	//����conjunction
	test(conjunction_v<TrueConstant, FalseConstant, TrueConstant>);//3����
	test(conjunction_v<FalseConstant>);//������
	test(conjunction_v<>);//�ղ���
	cout << "\n";
	//����negation
	test(negation_v<FalseConstant>);
	cout << "\n";
	//����disjunction
	test(disjunction_v<FalseConstant, TrueConstant>);//3����
	test(disjunction_v<FalseConstant>);//������
	test(disjunction_v<>);//�ղ���
	cout << "\n";
	//����enable_if
	test(typeid(enable_if_t<true, int>).name());
	//test(typeid(enable_if_t<false, int>).name());Ϊfalseʱ����ʧ��,��type��Ա,����Ԥ��
	cout << "\n";
	//����conditional
	test(typeid(conditional_t<true, int, char>).name());
	test(typeid(conditional_t<false, int, char>).name());
	cout << "\n";
	//����is_same
	test(is_same_v<int, int>);
	test(is_same_v<const int, int>);
	cout << "\n";
	//����remove_const
	test(typeid(remove_const_t<int>).name());
	test(typeid(remove_const_t<const int>).name());
	cout << "\n";
	//����remove_volatile
	test(typeid(remove_volatile_t<int>).name());
	test(typeid(remove_volatile_t<volatile int>).name());
	cout << "\n";
	//����remove_cv
	test(typeid(remove_cv_t<int>).name());
	test(typeid(remove_cv_t<volatile int>).name());
	test(typeid(remove_cv_t<const int>).name());
	test(typeid(remove_cv_t<const volatile int>).name());
	cout << "\n";
#if(MY_DEBUG)
	//����is_t_in_typeSet
	test(is_t_in_typeSet<int, char, double, long>);
	test(is_t_in_typeSet<int, char, double, long, int>);
	cout << "\n";
#endif
	//����is_integral
	test(is_integral_v<long>);
	test(is_integral_v<float>);
	cout << "\n";
	//����is_floating_point
	test(is_floating_point_v<long>);
	test(is_floating_point_v<double>);
	cout << "\n";
	//����is_arithmetic
	test(is_arithmetic_v<long>);
	test(is_arithmetic_v<TestClass>);
	cout << "\n";
	//����remove_reference
	test(typeid(remove_reference_t<int(&)[3]>).name());
	test(typeid(remove_reference_t<int*&>).name());
	cout << "\n";
	//����is_void��void_t
	test(is_void_v<int>);
	test(is_void_v<void_t<int>>);
	cout << "\n";
	//����add_const,add_volatile,add_cv
	test(is_same_v<add_const_t<int*>, const int*>);
	test(is_same_v<add_const_t<int*>, int* const>);
	test(is_same_v<add_volatile_t<int*>, volatile int*>);
	test(is_same_v<add_volatile_t<int*>, int* volatile>);
	test(is_same_v<add_cv_t<int*>, const volatile int*>);
	test(is_same_v<add_cv_t<int*>, int* volatile const>);
	cout << "\n";
	//����add_lvalue_reference��add_rvalue_reference
	test(is_same_v<add_lvalue_reference_t<int>, int&>);
	test(is_same_v<add_lvalue_reference_t<int>, const int&>);
	test(is_same_v<add_rvalue_reference_t<int>, int&&>);
	test(typeid(add_rvalue_reference_t<void>).name());
	cout << "\n";
	//����remove_extent��remove_all_extents
	test(is_same_v<remove_extent_t<int[]>, int>);
	test(is_same_v<remove_extent_t<int[][5]>, int[5]>);
	test(is_same_v<remove_extent_t<int[5][4]>, int[5]>);
	test(is_same_v<remove_all_extents_t<int[5][4][3]>, int>);
	cout << "\n";
	//����remove_pointer
	test(is_same_v<remove_pointer_t<int[]>, int>);
	test(is_same_v<remove_pointer_t<int*>, int>);
	test(is_same_v<remove_pointer_t<const volatile int*>, int>);
	test(is_same_v<remove_pointer_t<int* const volatile>, int>);
	cout << "\n";
	//����add_pointer
	test(is_same_v<add_pointer_t<const int>, int*>);
	test(is_same_v<add_pointer_t<int[]>, int(*)[]>);
	cout << "\n";
	//����is_array
	test(is_array_v<int[]>);
	test(is_array_v<int*>);
	cout << "\n";
	//����is_reference
	test(is_reference_v<int>);
	test(is_reference_v<int&>);
	test(is_reference_v<int&&>);
	test(is_lvalue_reference_v<int>);
	test(is_lvalue_reference_v<int&>);
	test(is_lvalue_reference_v<int&&>);
	test(is_rvalue_reference_v<int>);
	test(is_rvalue_reference_v<int&>);
	test(is_rvalue_reference_v<int&&>);
	cout << "\n";
	//����is_pointer
	test(is_pointer_v<int>);
	test(is_pointer_v<int*>);
	test(is_pointer_v<int[]>);
	test(is_pointer_v<int(*)(void)>);
	test(is_pointer_v<int*const volatile>);
	cout << "\n";
	//����is_null_pointer
	test(is_null_pointer_v<const volatile std::nullptr_t>);
	test(is_null_pointer_v<int*>);
	cout << "\n";
	//����is_fundamental
	test(is_fundamental_v<const volatile std::nullptr_t>);
	test(is_fundamental_v<const void>);
	test(is_fundamental_v<const volatile int>);
	test(is_fundamental_v<int*>);
	test(is_fundamental_v<TestClass>);
	cout << "\n";
	//����is_convertible
	test(is_convertible_v<int,double>);
	test(is_convertible_v<Derived*, Base*>);
	test(is_convertible_v<const Derived*, Derived*>);
	test(is_convertible_v<Base, Derived>);
	test(is_convertible_v<Derived*, Base>);
	test(is_convertible_v<Base, TestClass>);
	cout << "\n";
	//����is_enum
	test(is_enum_v<TestClass>);
	test(is_enum_v<int[]>);
	test(is_enum_v<int*>);
	test(is_enum_v<TestEnum>);
	cout << "\n";
	//����is_class
	test(is_class_v<TestClass>);
	test(is_class_v<int>);
	test(is_class_v<TestEnum>);
	cout << "\n";
	//����is_compound
	test(is_compound_v<TestClass>);
	test(is_compound_v<int(void)>);
	test(is_compound_v<int>);
	test(is_compound_v<TestEnum>);
	cout << "\n";
	//����is_member_function_pointer
	test(is_member_function_pointer_v<decltype(&TestClass::f)>);
	test(is_member_function_pointer_v<int*>);
	cout << "\n";
	//����is_const
	test(is_const_v<int>);
	test(is_const_v<const int>);
	test(is_const_v<volatile int>);
	test(is_const_v<const volatile int>);
	cout << "\n";
	//����is_volatile
	test(is_volatile_v<int>);
	test(is_volatile_v<const int>);
	test(is_volatile_v<volatile int>);
	test(is_volatile_v<const volatile int>);
	cout << "\n";


	//����

	cout << "\n";
	return 0;
}