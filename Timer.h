#ifndef  ChatRoom_Timer_H
#define  ChatRoom_Timer_H

#include <time.h>
#include <string>
using namespace std;

namespace ChatRoom
{
	class Timer
	{
	public:
		Timer(long long now = 0):microSecondsFromEpoch_(now){};
		string toString();
		static Timer now();
		static const long long kmicroSecondsPerSecond = 1000 * 1000;
	private:
		long long microSecondsFromEpoch_;
	};
}
#endif // ! ChatRoom_Timer_H
