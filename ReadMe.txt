g++ -std=c++11 -o HttpClientTest Acceptor.cc Buffer.cc Channel.cc Condition.cc Epoll.cc EventLoop.cc MutexLock.cc MutexLockGuard.cc Socket.cc TcpConnection.cc TcpServer.cc Thread.cc ThreadPool.cc HttpContext.cc HttpResponse.cc HttpRequest.cc HttpServer.cc Connector.cc TcpClient.cc HttpClientTest.cc -lpthread

Install JDK:
sudo add-apt-repository ppa:webupd8team/java
sudo apt-get update
sudo apt-get install oracle-java8-installer
sudo apt-get install oracle-java8-set-default

Install tomcate:
http://www.open-open.com/lib/view/open1455011606261.html

/usr/lib/tomcat8/bin/startup.sh
/usr/lib/tomcat8/bin/shutdown.sh

