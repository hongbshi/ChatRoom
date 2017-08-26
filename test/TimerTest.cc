#include "Timer.h"

#include <string>
#include <cstdio>
#include <iostream>

using namespace ChatRoom;
using namespace std;

int main(int argc, char **argv){
	Timer nowTime = Timer::now();
	cout<<nowTime.toString()<<endl;
	return 0;
}
