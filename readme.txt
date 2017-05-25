g++ -o EventTest.out MutexLock.cc MutexLockGuard.cc Condition.cc Thread.cc Channel.cc  Epoll.cc EventLoop.cc Test_EventLoop.c -std=c++11 -lpthread

g++ -o EventTest.out MutexLock.cc MutexLockGuard.cc Condition.cc Thread.cc Channel.cc  Epoll.cc EventLoop.cc Test_EventLoop.c Buffer.cc -std=c++11 -lpthread
