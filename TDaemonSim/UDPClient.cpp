#include "StdAfx.h"
#include "UDPClient.h"


CUDPClient::CUDPClient(void)
{
	m_pSoc = NULL;
}

CUDPClient::~CUDPClient(void)
{
	if (m_pSoc)
		delete m_pSoc;
}

bool CUDPClient::start(const char* ip, int port)
{
	CString s;
	
	s.Format("%d", port);
	udp::resolver res(m_ios);
	udp::resolver::query q(udp::v4(), ip, (LPCTSTR)s);
	m_ep = *res.resolve(q);

	if (m_pSoc) {
		delete m_pSoc;
		m_pSoc = NULL;
	}

	TRACE("host: %s, port: %d\n", m_ep.address().to_string().c_str(), m_ep.port());
	m_pSoc = new udp::socket(m_ios);
	if (m_pSoc == NULL) {
		TRACE("can't create socket\n");
		return false;
	}
	
	// boost::system::error_code ec = 
	m_pSoc->open(udp::v4());
/*
	if (ec) {
		TRACE("can't open socket\n");
		return false;
	}
*/
	DWORD nThreadId;
	m_hThread = CreateThread(NULL, 0, &CUDPClient::threadProc, this, NORMAL_PRIORITY_CLASS, &nThreadId);
	return (m_hThread != INVALID_HANDLE_VALUE);
}

void CUDPClient::run()
{
	m_ios.run();
}

void CUDPClient::stop()
{
	if (!TerminateThread(m_hThread, 0)) {
		TRACE("can't terminate thread\n");
	}

	m_ios.stop();
}

void CUDPClient::send(const char* p)
{
	size_t x = m_pSoc->send_to(boost::asio::buffer(p, strlen(p)), m_ep);
	TRACE("%s\n", p);
}
