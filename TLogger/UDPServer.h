#pragma once

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <queue>
#include <algorithm>

#define RCVBUF_SZ		1024

using boost::asio::ip::udp;

using namespace std;

class CUDPServer
{
public:
	CUDPServer(void);
	virtual ~CUDPServer(void);

	enum {
		SVR_TEMP = WM_USER+100,
		SVR_HANDLE_ERROR,
		SVR_UNKNOWN_IP_ERROR
	};

//	boost::array<char, RCVBUF_SZ>	m_rcvbuf;
	char*			m_pRcvBuf;
	vector<char>	m_buf;
	CString			m_sError;

	boost::asio::io_service	m_ios;
	udp::socket*	m_soc;
	udp::endpoint	m_rep;
	int				m_nPort;
	HANDLE			m_hThread;
	HWND			m_hWnd;

	bool			start(int port, HWND h);
	bool			stop();
	void			run();

private:
	inline void		post(DWORD msg, WPARAM arg1, LPARAM arg2) { if (m_hWnd) ::PostMessage(m_hWnd, msg, arg1, arg2); }
	void			updateTemp(DWORD ip, double temp);
	void			startRecieve();
	void			parse(const char* p, size_t sz);
	void			handleRecieve(const boost::system::error_code& e, std::size_t sz);

	static DWORD WINAPI threadProc(void* LPARAM) { ((CUDPServer*)LPARAM)->run(); return 1; }

};

