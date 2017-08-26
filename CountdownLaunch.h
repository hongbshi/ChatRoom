#ifndef ChatRoom_CountdownLaunch_H
#define ChatRoom_CountdownLaunch_H

#include <mutex>
#include <condition_variable>

namespace ChatRoom{
	class CountdownLaunch{
		public:
			CountdownLaunch(int initialNum):num_(initialNum){}

			void countdown(){
				std::unique_lock<std::mutex> lk(mutex_);
				num_ = num_ - 1;
				if(num_ <= 0) cond_.notify_all();
			}

			void wait(){
				std::unique_lock<std::mutex> lk(mutex_);
				while(num_ > 0) cond_.wait(lk);
			}

		private:
			std::mutex mutex_;
			std::condition_variable cond_;
			int num_;
	};
}


#endif
