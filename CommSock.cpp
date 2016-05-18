#include "CommSock.h"


namespace MySocket 
{
#ifdef WINDOWS
	unsigned int CommonSocket::_num_sockets = 0;
#endif

	/*SocketException class*/

	SocketException::SocketException(const string &message)
	{
		this->_error = message;
	}

	SocketException::~SocketException() throw()
	{
	}

	const char* SocketException::what() const throw()
	{
		return this->_error.c_str();
	}

	ostream& operator<< (ostream &out, SocketException &e)
	{
		out << e.what();
		return out;
	}


	/*CommonSocket class*/
	void CommonSocket::_socket(void)
	{
#ifdef WINDOWS
		this->_num_sockets++;
		if (this->_num_sockets == 1)
		{
			WSADATA wsaData;
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
				throw SocketException("[constructor] Cannot initialize socket environment");
		}
#endif
	}

	CommonSocket::CommonSocket(void)
	{
		this->_socket();
	}

	CommonSocket::CommonSocket(int socket_type)
	{
		this->_socket();

		this->_socket_type = socket_type;
		this->_opened = false;
		this->_binded = false;
		this->open();
	}

	CommonSocket::~CommonSocket(void)
	{
#ifdef WINDOWS
		this->_num_sockets--;
		if (this->_num_sockets == 0)
			WSACleanup();
#endif
	}

	void CommonSocket::open(void)
	{
		if (!this->_opened)
		{
			if ((this->_socket_id = socket(AF_INET, this->_socket_type, 0)) == -1)
				throw SocketException("[open] Cannot create socket");
			this->_opened = true;
			this->_binded = false;
		}
	}

	void CommonSocket::close(void)
	{
		if (this->_opened)
#ifdef WINDOWS
			closesocket(this->_socket_id);
#else
			shutdown(this->_socket_id, SHUT_RDWR);
#endif

		this->_opened = false;
		this->_binded = false;
	}

	void CommonSocket::listen_on_port(Port port)
	{
		if (this->_binded) throw SocketException("[listen_on_port] Socket already binded to a port, close the socket before to re-bind");

		if (!this->_opened) this->open();

		Address address(port);

		if (bind(this->_socket_id, (struct sockaddr*)&address, sizeof(struct sockaddr)) == -1)
		{
			stringstream error;
			error << "[listen_on_port] with [port=" << port << "] Cannot bind socket";
			throw SocketException(error.str());
		}

		this->_binded = true;
	}



	/*Adress struct*/

	void Address::_address(Ip ip, Port port)
	{
		this->sin_family = AF_INET;
		this->ip(ip);
		this->port(port);
	}

	Address::Address()
	{
		_address("0.0.0.0", 0);
	}

	Address::Address(Port port)
	{
		_address("0.0.0.0", port);
	}

	Address::Address(Ip ip, Port port)
	{
		_address(ip, port);
	}

	Address::Address(struct sockaddr_in address)
	{
		_address(inet_ntoa(address.sin_addr), address.sin_port);
	}

	Address::Address(const Address &address)
	{
		this->sin_family = address.sin_family;
		this->sin_addr = address.sin_addr;
		this->sin_port = address.sin_port;
	}

	Ip Address::ip(void)
	{
		return inet_ntoa(this->sin_addr);
	}

	Ip Address::ip(Ip ip)
	{
#ifdef WINDOWS
		unsigned long address = inet_addr(ip.c_str());

		if (address == INADDR_NONE)
		{
			stringstream error;
			error << "[ip] with [ip=" << ip << "] Invalid ip address provided";
			throw SocketException(error.str());
		}
		else
		{
			this->sin_addr.S_un.S_addr = address;
		}
#else
		if (inet_aton(ip.c_str(), &this->sin_addr) == 0)
		{
			stringstream error;
			error << "[ip] with [ip=" << ip << "] Invalid ip address provided";
			throw SocketException(error.str());
		}
#endif
		return this->ip();
	}

	Port Address::port(void)
	{
		return ntohs(this->sin_port);
	}

	Port Address::port(Port port)
	{
		this->sin_port = htons(port);
		return this->port();
	}

}