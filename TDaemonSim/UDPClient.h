#pragma once

#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class CUDPClient
{
public:
	CUDPClient(void);
	virtual ~CUDPClient(void);

public:
	bool	start(const char* ip, int port);
	void	stop();
	void	send(const char* p);

private:
	boost::asio::io_service m_ios;
	udp::socket*			m_pSoc;
	udp::endpoint			m_ep;
	HANDLE					m_hThread;

	void	run();
	static DWORD WINAPI threadProc(void* LPARAM) { 
		((CUDPClient*)LPARAM)->run(); 
		return 1; 
	}
};

