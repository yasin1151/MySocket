#ifndef _COMMON_SOCKET_H_
#define _COMMON_SOCKET_H_

#if defined _WIN32 || defined _WIN64
	#include <WinSock2.h>
	#define WINDOWS
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#endif // _WIN32

#include <exception>
#include <string>
#include <sstream>
using namespace std;

#define SOCKET_MAX_BUFFER_LEN 1024

namespace MySocket
{
	typedef int SocketId;
	typedef string Ip;
	typedef unsigned int Port;
#ifdef WINDOWS
	typedef int socklen_t;
#endif


	class SocketException : public exception
	{
	private:
		string _error;

	public:
		SocketException(const string&);
		~SocketException() throw();

		virtual const char* what() const throw();
		friend ostream& operator<< (ostream &out, SocketException &e);
	};

	struct Address : protected sockaddr_in
	{
	private:
		void _address(Ip, Port);

	public:
		Address();
		Address(Port);
		Address(Ip, Port);
		Address(struct sockaddr_in);
		Address(const Address&);

		Ip ip(void);
		Ip ip(Ip);

		Port port(void);
		Port port(Port);

		friend ostream& operator<< (ostream& out, Address& address)
		{
			out << address.ip() << ":" << address.port();
			return out;
		}
	};

	template <class DataType>
	struct Datagram
	{
	public:
		Address address;
		DataType data;
		unsigned int received_bytes;
		unsigned int received_elements;

		template <class T>
		void operator = (const Datagram<T> &datagram)
		{
			this->address = datagram.address;
			this->data = datagram.data;
		}
	};

	class CommonSocket
	{
	private:
#ifdef WINDOWS
		static unsigned int _num_sockets;
#endif
		void _socket(void);

	protected:
		SocketId _socket_id;
		int _socket_type;
		bool _opened;
		bool _binded;

	public:
		CommonSocket(void);
		CommonSocket(int);

		~CommonSocket(void);

		void open(void);
		void close(void);

		virtual void listen_on_port(Port);
	};
}


#endif