#ifndef ChatRoom_BlockingQueue_H
#define ChatRoom_BlockingQueue_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace ChatRoom{
	template<class T>
	class BlockingQueue{
		public:
			void put(const T & t){
				std::lock_guard<std::mutex> lk(mutex_);
				store_.push(t);
			}

			T get(){
				std::unique_lock<std::mutex> lk(mutex_);
				while(store_.empty()) cond_.wait(lk);
				T res(store_.front());
				store_.pop();
				return res;
			}
		
		private:
			std::mutex mutex_;
			std::condition_variable cond_;
			std::queue<T> store_;
	};
}

#endif
