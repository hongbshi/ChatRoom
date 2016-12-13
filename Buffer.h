//Not Thread Safe

#ifndef ChatRoom_Buffer_H
#define ChatRoom_Buffer_H

#include<vector>
#include<string>
namespace ChatRoom
{
	class Buffer
	{
		public:
			Buffer() :size_(0) {}
			//Original data will be cleared  (All Read Function)
			void Read(std::string& d); 
			void Read(Buffer& d);
			void Read(std::string& d,int& len);
			void Read(Buffer& d, int& len);
			void Write(const std::string& s);
			void Write(const Buffer& s);
			void Write(const char* s);
			int getSize();
			std::string getString();
			void clear();
		private:
			vector<char> store_;
			int size_;
	}
}
#endif
