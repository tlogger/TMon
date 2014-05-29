#include "StdAfx.h"
#include "Config.h"

#include <math.h>

CConfig::CConfig(void)
{
	m_nPort = 1000;
	m_nServerLogFileSize = 65536;
	m_nLogFileSize = 1048576;
	m_nLogMax = 100;
	m_nLogTempMax = 100;
	m_nLogTimeout = 0;
}

CConfig::~CConfig(void)
{
	for (size_t i = 0; i < m_aEquip.size(); i++)
		delete m_aEquip[i];
	m_aEquip.clear();
}

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

void CConfig::s2t(const char* s, time_t& t)
{
//	struct tm tm;
//	_strptime(s, "%Y-%m-%d %H:%M:%S", &tm);
	try {
		ptime x = time_from_string(s);
		tm pt_tm = to_tm(x);
		t = mktime(&pt_tm);
	}
	catch(...)
	{
		t = 0;
	}
}

CString& CConfig::convertValidFilename(CString& s, bool bPath /* = false */)
{
	int pos = 0;
	
	if (bPath)
		while ((pos = s.FindOneOf("/:*?\"<>|")) != -1)
			s.Replace(s[pos], '_');
	else
		while ((pos = s.FindOneOf("/:*?\"<>|\\")) != -1)
			s.Replace(s[pos], '_');

	return s;
}

char* CConfig::t2s(time_t t, const char* fmt)
{
	static char d[20];
	struct tm tp;
	localtime_s(&tp, &t);
	if (fmt == NULL)
		strftime(d, sizeof(d), "%Y-%m-%d %H:%M:%S", &tp);
	else
		strftime(d, sizeof(d), fmt, &tp);
	return d;
}

bool CConfig::testDir(const char* dir)
{
	char cd[512];

	GetCurrentDirectory(sizeof(cd), cd);
	if (!SetCurrentDirectory(dir)) {
		if (!CreateDirectory(dir, NULL)) {
			return false;
		}
	}

	SetCurrentDirectory(cd);
	return true;
}

bool CConfig::load(const char* fn)
{
	CStdioFile f;
	CString line, cat, s;
	int lno = 1, pos;
	bool bInConfig = false;
	CEquip* p = NULL;
	bool ret = false;
	bool bConfig = false;
	bool bEquip = false;
	int nIndex = 0;

	m_sConfigFn = fn;
	if (!f.Open(m_sConfigFn, CFile::modeRead | CFile::shareDenyNone)) {
		s.Format("config file '%s' load error", m_sConfigFn);
		::MessageBox(NULL, s, "File Open", MB_OK | MB_ICONERROR);
		return false;
	}

	try {
		while (f.ReadString(line)) {
			line.TrimLeft().TrimRight();
			if (line.GetLength() == 0 || line[0] == ';') {
				lno++;
				continue;
			}

			if (line[0] == '[') {
				if (p != NULL)
					m_aEquip.push_back(p);

				pos = line.Find(']');
				cat = line.Mid(1, pos-1);
				if (cat == "config")
					bInConfig = true;
				else
					bInConfig = false;

				if (cat == "equip") {
					p = new CEquip;
					p->m_nIndex = nIndex++;
				}
			}
			else {
				if ((pos = line.Find('=')) == -1)
					throw lno;

				CString var = line.Left(pos);
				line = line.Mid(pos+1);
				line.TrimLeft().TrimRight();
				if (pos = line.Find('\"') != -1)				// quotation Á¦°Å
					line = line.Mid(1, line.GetLength()-2);
				
				if (bInConfig) {
					if (var == "port")					m_nPort = atoi(line);
					else if (var == "server-log-size")	m_nServerLogFileSize = atoi(line);
					else if (var == "log-size")			m_nLogFileSize = atoi(line);
					else if (var == "log-max")			m_nLogMax = atoi(line);
					else if (var == "log-temp-max")		m_nLogTempMax = atoi(line);
					else if (var == "log-timeout")		m_nLogTimeout = atoi(line);
					else if (var == "log-dir") {
						m_sLogPath = line;
						if (!testDir(m_sLogPath)) {
							s.Format("can't create log root directory '%s'", m_sLogPath);
							::MessageBox(NULL, s, "File Open", MB_OK | MB_ICONERROR);
							return false;
						}
					}
					else
						throw lno;
					bConfig = true;
				}
				else {
					if (var == "id")					p->m_sID = line;
					else if (var == "logger-type")		p->m_sLoggerType = line;
					else if (var == "ip")				p->m_sIP = line;
					else if (var == "prefix")			p->m_sPrefix = line;
					else if (var == "log-method")		p->m_sLogMethod = line;
					else if (var == "interval")			p->m_nInterval = atoi(line);
					else if (var == "tolerance")		p->m_sTolerance = line;
					else if (var == "start-condition")	p->m_sStartCond = line;
					else if (var == "end-condition")	p->m_sEndCond = line;
					else if (var == "start-date")		p->m_sStartTime = line;
					else if (var == "end-date")			p->m_sEndTime = line;
					else
						throw lno;
					bEquip = true;
				}
			}
		}

		if (p != NULL)
			m_aEquip.push_back(p);

		ret = true;
	}
	catch (int lno) {
		s.Format("File '%s': line number %d has syntax error", m_sConfigFn, lno);
		MessageBox(NULL, s, "Load Config Error", MB_OK | MB_ICONEXCLAMATION);
		if (p != NULL)
			delete p;
	}
	catch (...) {
		s.Format("File '%s': read  error", m_sConfigFn);
		MessageBox(NULL, s, "Load Config Error", MB_OK | MB_ICONEXCLAMATION);
	}

	f.Close();

	if (!bConfig) {
		s.Format("File '%s': there's no configuration exist", m_sConfigFn);
		MessageBox(NULL, s, "Load Config Error", MB_OK | MB_ICONEXCLAMATION);
	}
	
	if (!bEquip) {
		s.Format("File '%s': there's no equipment setup data", m_sConfigFn);
		MessageBox(NULL, s, "Load Config Error", MB_OK | MB_ICONEXCLAMATION);
	}

	for (size_t i = 0; i < m_aEquip.size(); i++) {
		p = m_aEquip[i];
		p->convert();
	}

	return ret;
}

bool CConfig::save()
{
	CEquip* p;
	for (size_t i = 0; i < m_aEquip.size(); i++) {
		p = m_aEquip[i];
		p->convert();
	}

	CStdioFile f;
	CString s;
	bool ret = false;

	if (!f.Open(m_sConfigFn, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone)) {
		s.Format("config file '%s' save error", m_sConfigFn);
		::MessageBox(NULL, s, "File Save", MB_OK | MB_ICONERROR);
		return false;
	}

	try {
		s.Format("[config]\n"
			"\tport=%d\n"
			"\tlog-dir=%s\n"
			"\tserver-log-size=%d\n"
			"\tlog-size=%d\n"
			"\tlog-max=%d\n"
			"\tlog-temp-max=%d\n"
			"\tlog-timeout=%d\n"
			"\n"
			, m_nPort, m_sLogPath, m_nServerLogFileSize, m_nLogFileSize, m_nLogMax, m_nLogTempMax, m_nLogTimeout);
		f.WriteString(s);

		for (size_t i = 0; i < m_aEquip.size(); i++) {
			CEquip* p = m_aEquip[i];
			s.Format("[equip]\n"
				"\tid=\"%s\"\n"
				"\tip=\"%s\"\n"
				"\tprefix=\"%s\"\n"
				"\tlogger-type=\"%s\"\n"
				"\tlog-method=\"%s\"\n"
				"\tinterval=%d\n"
				"\ttolerance=%.2f\n"
				"\tstart-condition=\"%s\"\n"
				"\tend-condition=\"%s\"\n"
				"\tstart-date=\"%s\"\n"
				"\tend-date=\"%s\"\n"
				"\n"
				, p->m_sID, p->m_sIP, p->m_sPrefix, p->m_sLoggerType, p->m_sLogMethod, p->m_nInterval, 
				p->m_fTolerance, p->m_sStartCond, p->m_sEndCond, p->m_sStartTime, p->m_sEndTime);
			f.WriteString(s);
		}
		ret = true;
	}
	catch(...) {
		s.Format("config file '%s' write error", m_sConfigFn);
		::MessageBox(NULL, s, "File Save", MB_OK | MB_ICONERROR);
	}

	f.Close();
	return true;
}

char* CConfig::d2s(time_t tm)
{
	CString s;
	static CString buf;

	buf = "";
	time_t d = tm / 86400;
	if (d > 0) {
		s.Format("%dd", d);
		buf += s;
		tm -= d * 86400;
	}

	time_t h = tm / 3600;
	if (h > 0) {
		s.Format("%dh", h);
		buf += s;
		tm -= h * 3600;
	}

	time_t m = tm / 60;
	time_t ss = tm % 60;
	if (m > 0) {
		s.Format("%dm", m);
		buf += s;
	}

	s.Format("%ds", ss);
	buf += s;

	return (LPTSTR)(LPCTSTR)buf;
}

time_t CConfig::s2d(const char* s)
{
	char buf[100], *q, *p = buf;
	time_t d = 0;

	while (*s)  
		*p++ = toupper(*s++); 
	*p = '\0';

	p = buf;	
	q = strchr(p, 'D'); if (q != NULL) { d = atoi(p) * 86400; p = q + 1; }
	q = strchr(p, 'H'); if (q != NULL) { d += atoi(p) * 3600; p = q + 1; }
	q = strchr(p, 'M'); if (q != NULL) { d += atoi(p) * 60;	 p = q + 1;	}
	
	return d + atoi(p);
}
