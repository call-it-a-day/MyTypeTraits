#include"my_type_traits.h"
#include<iostream>
using namespace std;
using namespace myStd;

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
	cout << boolalpha;
	cout << "��ʼ����\n";

	return 0;
}