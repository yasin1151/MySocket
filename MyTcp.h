#ifndef _MY_TCP_H_
#define _MY_TCP_H_

#include <fstream>
#include "CommSock.h"


namespace MySocket
{
	class TCP : public CommonSocket
	{
	private:
		Address _address;
	public:
		TCP(void);
		TCP(const TCP&);

		Ip ip(void);
		Port port(void);
		Address address(void);

		void listen_on_port(Port, unsigned int);
		void connect_to(Address);

		TCP accept_client(void);

		template <class T> 
		int send(const T* buffer, size_t len);

		template <class T> 
		int receive(T* buffer, size_t len);

		void send_file(string);
		void receive_file(string);
	};



	template <class T>
	int TCP::send(const T* buffer, size_t len)
	{
		if (!this->_binded) throw SocketException("[send] Socket not binded");
		if (!this->_opened) throw SocketException("[send] Socket not opened");

		len *= sizeof(T);
		if (len > (SOCKET_MAX_BUFFER_LEN * sizeof(T)))
		{
			stringstream error;
			error << "[send] [len=" << len << "] Data length higher then max buffer len (" << SOCKET_MAX_BUFFER_LEN << ")";
			throw SocketException(error.str());
		}

		int ret;
		if ((ret = ::send(this->_socket_id, (const char*)buffer, len, 0)) == -1) throw SocketException("[send] Cannot send");
		return ret;
	}

	template <class T>
	int TCP::receive(T* buffer, size_t len)
	{
		if (!this->_binded) throw SocketException("[send_file] Socket not binded");
		if (!this->_opened) throw SocketException("[send_file] Socket not opened");

		len *= sizeof(T);
		if (len > (SOCKET_MAX_BUFFER_LEN * sizeof(T)))
		{
			stringstream error;
			error << "[receive] [len=" << len << "] Data length higher then max buffer len (" << SOCKET_MAX_BUFFER_LEN << ")";
			throw SocketException(error.str());
		}

		int ret;
#ifdef WINDOWS
		if ((ret = recv(this->_socket_id, (char*)buffer, len, 0)) == -1) throw SocketException("[send] Cannot receive");
#else
		if ((ret = recv(this->_socket_id, buffer, len, 0)) == -1) throw SocketException("[send] Cannot receive");
#endif

		return ret;
	}

}

#endif // !_MY_TCP_H
