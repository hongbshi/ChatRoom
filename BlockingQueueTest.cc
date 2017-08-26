#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>

#include "BlockingQueue.h"
#include "CountdownLaunch.h"

using namespace std;
using namespace ChatRoom;

BlockingQueue<int> work;

void work_thread(){
	while(int t = work.get()) printf("%d\n", t);
}

int main(int argc, char **argv){
	std::thread t1(work_thread);
	for(int i = 3; i >= 0; --i) work.put(i);
	t1.join();
}
