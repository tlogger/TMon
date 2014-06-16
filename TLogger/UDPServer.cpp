#include "StdAfx.h"
#include "UDPServer.h"

CUDPServer::CUDPServer(void)
{
	m_soc = NULL;
	m_pRcvBuf = NULL;
	m_pRcvBuf = new char[RCVBUF_SZ];
	m_hWnd = 0;
}

CUDPServer::~CUDPServer(void)
{
	if (m_pRcvBuf)
		delete m_pRcvBuf;
}

bool CUDPServer::stop()
{
	if (TerminateThread(m_hThread, 0) == 0) {
		TRACE("can't stop thread\n");
		return 0;
	}

	m_ios.stop();
	if (m_soc != NULL) {
		delete m_soc;
		m_soc = NULL;
	}
	return true;
}

bool CUDPServer::start(int port, HWND h)
{
	m_nPort = port;
	m_hWnd = h;

	DWORD nThreadId;
	m_soc = new udp::socket(m_ios, udp::endpoint(udp::v4(), m_nPort));
	if (m_soc == NULL) {
		TRACE("can't create socket\n");
		return false;
	}

	m_hThread = CreateThread(NULL, 0, &CUDPServer::threadProc, this, NORMAL_PRIORITY_CLASS, &nThreadId);
	return (m_hThread != INVALID_HANDLE_VALUE);
}

void CUDPServer::run()
{
	try {
		startRecieve();
		m_ios.run();
	}
	catch(...)
	{
		TRACE("%s: catch exception\n", __FUNCTION__);
	}
}

void CUDPServer::startRecieve()
{
	m_soc->async_receive_from(
		boost::asio::buffer(m_pRcvBuf, RCVBUF_SZ), m_rep,
//		boost::asio::buffer(m_rcvbuf), m_rep,
		boost::bind(&CUDPServer::handleRecieve, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)
	);
}

void CUDPServer::parse(const char* p, size_t sz)
{
#if 0
	string data(m_pRcvBuf, sz);
	TRACE("%s: %s\n", m_rep.address().to_string().c_str(), data.c_str());
#endif	

	// for 짤방
	m_buf.insert(m_buf.end(), p, p+sz);

	DWORD m_nIP = inet_addr(m_rep.address().to_string().c_str());
	vector<char>::iterator it;
	double temp = 0.0;

	for (it = find(m_buf.begin(), m_buf.end(), '\r'); it != m_buf.end(); it = find(m_buf.begin(), m_buf.end(), '\r')) {

		string data(m_buf.begin(), it);
		m_buf.erase(m_buf.begin(), it+1);

		char* pp = (char*)data.c_str();
		char* q;

		for (size_t i = 0; i < config.m_aEquip.size(); i++) {
			CEquip* p = config.m_aEquip[i];
			// IP와 prefix가 일치할 때만 적법
			q = strstr(pp, p->m_sPrefix);
			if (p->m_nIP == m_nIP && q != NULL) {
				temp = atof(pp + p->m_sPrefix.GetLength());
				p->putTemp(temp);
				// TODO 온도 변화시 알림?
				// post(SVR_TEMP, i, int(temp * 100.0));
			}
		}
		// 알려지지 않은 서버가 보내온 데이터일 경우 warning으로 처리
		post(SVR_UNKNOWN_IP_ERROR, m_nIP, int(temp * 100.0));
	}
}

void CUDPServer::handleRecieve(const boost::system::error_code& e, std::size_t sz)
{
	if (!e || e == boost::asio::error::message_size) {
		parse(m_pRcvBuf, sz);
		startRecieve();
	}
	else {
		m_sError = e.message().c_str();
		TRACE("%s: %s\n", __FUNCTION__, m_sError);
		post(SVR_HANDLE_ERROR, 0, 0);
	}
}
