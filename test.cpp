#include"my_type_traits.h"
#include<iostream>
using std::cout;
using namespace myStd;

template<typename...Args>
void test(const Args&...args) {
	((cout << args << '\t'), ...);
	cout << '\n';
}

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
	test(is_same_v<int,int>);
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
	return 0;
}