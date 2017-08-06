#include "Timer.h"
#include <sys/time.h>

using namespace ChatRoom;

string Timer::toString(){
	time_t seconds = static_cast<time_t>(microSecondsFromEpoch_ / kmicroSecondsPerSecond); 
	int microSeconds = microSecondsFromEpoch_ % kmicroSecondsPerSecond;
	struct tm res;
	gmtime_r(&seconds, &res);
	char buff[32] = {'0'};
	snprintf(buff, sizeof buff, "%04d-%02d-%02d %02d:%02d:%02d.%06d",
			res.tm_year + 1900, res.tm_mon + 1, res.tm_mday,
			res.tm_hour, res.tm_min, res.tm_sec, microSeconds);
	return string(buff);
}

Timer Timer::now(){
	struct timeval res;
	gettimeofday(&res, nullptr);
	long long t = res.tv_sec * kmicroSecondsPerSecond + res.tv_usec;
	return Timer(t);
}
