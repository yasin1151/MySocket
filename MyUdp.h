#ifndef _MY_UDP_H_
#define _MY_UDP_H_

#include <vector>
#include "CommSock.h"


namespace MySocket
{
	class UDP : public CommonSocket
	{
	public:
		UDP(void);
		UDP(const UDP&);

		template <class T> int send(Ip, Port, const T*, size_t);
		template <class T> int send(Address, const T*, size_t);
		template <class T> int send(Ip, Port, T);
		template <class T> int send(Address, T);
		template <class T> int send(Ip, Port, vector<T>);
		template <class T> int send(Address, vector<T>);

		template <class T> int receive(Address*, T*, size_t, unsigned int*);
		template <class T> Datagram<T*> receive(T*, size_t);
		template <class T, size_t N> Datagram<T[N]> receive(size_t);
		template <class T> Datagram<T> receive(void);
		template <class T> Datagram<vector<T> > receive(size_t);
	};



	UDP::UDP(void) : CommonSocket(SOCK_DGRAM)
	{
	}

	UDP::UDP(const UDP &udp) : CommonSocket()
	{
		this->_socket_id = udp._socket_id;
		this->_opened = udp._opened;
		this->_binded = udp._binded;
	}

	template <class T>
	int UDP::send(Ip ip, Port port, const T *data, size_t len)
	{
		if (!this->_opened) this->open();

		len *= sizeof(T);
		if (len > (SOCKET_MAX_BUFFER_LEN * sizeof(T)))
		{
			stringstream error;
			error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << data << "] [len=" << len << "] Data length higher then max buffer len";
			throw SocketException(error.str());
		}

		Address address(ip, port);
		int ret;

		if ((ret = sendto(this->_socket_id, (const char*)data, len, 0, (struct sockaddr*)&address, sizeof(struct sockaddr))) == -1)
		{
			stringstream error;
			error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << data << "] [len=" << len << "] Cannot send";
			throw SocketException(error.str());
		}

		return ret;
	}

	template <class T>
	int UDP::send(Address address, const T *data, size_t len)
	{
		return this->send<T>(address.ip(), address.port(), data, len);
	}

	template <class T>
	int UDP::send(Ip ip, Port port, T data)
	{
		return this->send<T>(ip, port, &data, 1);
	}

	template <class T>
	int UDP::send(Address address, T data)
	{
		return this->send<T>(address.ip(), address.port(), &data, 1);
	}

	template <>
	int UDP::send<string>(Ip ip, Port port, string data)
	{
		return this->send<char>(ip, port, data.c_str(), data.length() + 1);
	}

	template <>
	int UDP::send<string>(Address address, string data)
	{
		return this->send<char>(address.ip(), address.port(), data.c_str(), data.length() + 1);
	}

	template <class T>
	int UDP::send(Ip ip, Port port, vector<T> data)
	{
		return this->send<T>(ip, port, data.data(), data.size());
	}

	template <class T>
	int UDP::send(Address address, vector<T> data)
	{
		return this->send<T>(address.ip(), address.port(), data.data(), data.size());
	}

	template <class T>
	int UDP::receive(Address *address, T *data, size_t len, unsigned int *received_elements)
	{
		if (!this->_opened) this->open();
		if (!this->_binded) throw SocketException("[receive] Make the socket listening before receiving");

		len *= sizeof(T);
		if (len > (SOCKET_MAX_BUFFER_LEN * sizeof(T)))
		{
			stringstream error;
			error << "[send] with [buffer=" << data << "] [len=" << len << "] Data length higher then max buffer length";
			throw SocketException(error.str());
		}

		int received_bytes;
		socklen_t size = sizeof(struct sockaddr);

		if ((received_bytes = recvfrom(this->_socket_id, (char*)data, len, 0, (struct sockaddr*)address, (socklen_t*)&size)) == -1)
		{
			throw SocketException("[receive] Cannot receive");
		}

		*received_elements = (unsigned int)(received_bytes / sizeof(T));

		return received_bytes;
	}

	template <class T>
	Datagram<T*> UDP::receive(T *buffer, size_t len = SOCKET_MAX_BUFFER_LEN)
	{
		Datagram<T*> ret;

		ret.received_bytes = this->receive<T>(&ret.address, buffer, len, &ret.received_elements);
		ret.data = buffer;

		return ret;
	}

	template <class T, size_t N>
	Datagram<T[N]> UDP::receive(size_t len = N)
	{
		Datagram<T[N]> ret;
		ret.received_bytes = this->receive<T>(&ret.address, ret.data, len, &ret.received_elements);
		return ret;
	}

	template <class T>
	Datagram<T> UDP::receive(void)
	{
		Datagram<T> ret;
		ret.received_bytes = this->receive<T>(&ret.address, &ret.data, 1, &ret.received_elements);
		return ret;
	}

	template <>
	Datagram<string> UDP::receive<string>(void)
	{
		Datagram<string> ret;
		char buffer[SOCKET_MAX_BUFFER_LEN];

		ret.received_bytes = this->receive<char>(&ret.address, buffer, SOCKET_MAX_BUFFER_LEN, &ret.received_elements);
		ret.data = buffer;

		return ret;
	}

	template <class T>
	Datagram<vector<T> > UDP::receive(size_t len)
	{
		Datagram<vector<T> > ret;
		T buffer[len];

		ret.received_bytes = this->receive<T>(&ret.address, buffer, len, &ret.received_elements);
		for (int i = 0; i < ret.received_elements; i++) ret.data.push_back(buffer[i]);

		return ret;
	}
}

#endif // !_MY_UDP_H_
