#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string>
#include "Buffer.h"
using namespace ChatRoom;
using namespace std;
void main()
{
	Buffer buff1,buff2,buff3;
	string first("abcde");
	string second("hijkl");
	char three[10] = "qwer";
	buff1.Append(first);
	cout << buff1.getString()<<'\t'<<buff1.getLen() << endl;

	buff2.Append(buff1);
	cout << buff2.getString() << '\t' << buff2.getLen() << endl;

	buff3.Append(three);
	cout << buff3.getString() << '\t' << buff3.getLen() << endl;

	string result;
	int len=2;
	buff1.Read(result, len);
	cout << result << '\t' << len << endl;
	cout << buff1.getString() << '\t' << buff1.getLen() << endl;
	buff1.Read(result);
	cout << result << '\t' << len << endl;
	cout << buff1.getString() << '\t' << buff1.getLen() << endl;

	buff2.Read(buff3, len);
	cout << len << endl;
	cout << buff2.getString() << '\t' << buff2.getLen() << endl;
	cout << buff3.getString() << '\t' << buff3.getLen() << endl;

	buff2.clear();
	cout << buff2.getString() << '\t' << buff2.getLen() << endl;
	system("pause");
}