#include "StdAfx.h"
#include "Equip.h"

#define COND_MATCH(x) ( \
	(x.op == COND_EQUAL && abs(m_fPV - x.temp) < m_fTolerance) || \
	(x.op == COND_BIG && m_fPV > x.temp) || \
	(x.op == COND_LESS && m_fPV < x.temp) )

const char* CEquip::m_aLoggerType[] = { "GSTECH", "Logger", "" };
const char* CEquip::m_aLogMethod[] = { "Periodic", "Detect", "" };

CEquip::CEquip(void)
{
	m_sID = "";
	m_nLoggerType = 0;
	m_sIP = "";
	m_nLogMethod = 1;
	m_fTolerance = 1.0;
	m_nInterval = 60;
	m_tStart = m_tEnd = m_tLastLogged = 0;
	m_nLogCount = 0;
	m_tLastUpdate = time(NULL);
	m_bLogging = false;
	m_bLock = false;
	m_fPV = 25.0;
	m_bErrorLogged = false;
}

CEquip::~CEquip(void)
{
	closeTempFiles();
}

void CEquip::putTemp(double f)
{
	m_tLastUpdate = time(NULL);
	m_bErrorLogged = false;
	
//	TRACE("%s: %d %f\n", __FUNCTION__, m_bLogging, f);
	if (m_nLoggerType == LOGGER_GSTECH) {
		if (f == LOGGER_GSTECH_START_COND) {
			f = 25.0;
			start();
			post(LOG_GSTECH_START, m_nIndex, 0);
		}
		else if (f == LOGGER_GSTECH_END_COND) {
			f = 25.0;
			stop();
			post(LOG_GSTECH_END, m_nIndex, 0);
		}
	}
	
	m_fPV = f;
}

CEquip::cond_t& CEquip::s2cond(const char* str)
{
	int pos = 0;
	CString s = str;
	CString t = s.Tokenize(",", pos);
	static cond_t cond;

	if (t.GetLength() == 0)
		return cond;

	if (t[0] == '=')
		cond.op = CEquip::COND_EQUAL;
	else if (t[0] == '>')
		cond.op = CEquip::COND_BIG;
	else if (t[0] == '<')
		cond.op = CEquip::COND_LESS;
	else
		cond.op = CEquip::COND_EQUAL;

	if (t.GetLength() > 0) {
		cond.temp = atof(t.Mid(1));
	}
	s = s.Mid(pos);
	cond.duration = config.s2d(s);

	return cond;
}

CString& CEquip::cond2s(CEquip::cond_t& cond)
{
	CString x;
	static CString s;

	s = "";
	if (cond.op == CEquip::COND_EQUAL)
		x.Format("=%.2f%s", cond.temp, s);
	else if (cond.op == CEquip::COND_BIG)
		x.Format(">%.0f%s", cond.temp, s);
	else if (cond.op == CEquip::COND_LESS)
		x.Format("<%.0f%s", cond.temp, s);


	if (cond.duration != 0) 
		s.Format("%s,%s", x, config.d2s(cond.duration));
	else
		s = x;

	return s;
}

bool CEquip::isStartCondition()
{
	// 이미 시작되었으면 return false
	if (m_bLogging) {
		m_condStart.m_tStart = 0;
		return false;
	}

	// force stop되어 lock이 걸려있으면 시작될 수 없음.
	if (m_bLock)
		return false;

	// GSTECH은 condition check를 하지 않음.
	if (m_nLoggerType == LOGGER_GSTECH)
		return false;

	time_t t = time(NULL);
	if (m_nLogMethod == LOGTYPE_PERIODIC) {	// periodic
		// 시각에 이르렀으면 start condition을 찾은 것.
		if (time(NULL) > m_tStart && time(NULL) < m_tEnd) {
			return true;
		}
	}
	else {					// detect
		if (COND_MATCH(m_condStart)) {
			// duration이 지정되지 않았다면
			if (m_condStart.duration == 0) {
				return true;
			}
			// duration이 지정되었다면,
			else {
				// 조건이 맞고, m_tStart가 초기화 되어 있으면 count 시작
				if (m_condStart.m_tStart == 0) {
					m_condStart.m_tStart = t;
				}
				// 조건이 맞고, duration 동안 조건이 유지되어 왔다면 start condition을 찾음.
				else if (t > m_condStart.m_tStart + m_condStart.duration) {
					m_condStart.m_tStart = 0;
					return true;
				}
			}
		}
		else {
			// 조건이 안 맞으면 m_tStart를 초기화
			m_condStart.m_tStart = 0;
		}
	}

	return false;
}

bool CEquip::isEndCondition()
{
	// 시작되지 않았으면 return false
	if (!m_bLogging) {
		m_condEnd.m_tStart = 0;
		return false;
	}

	// GSTECH은 condition check를 하지 않음.
	if (m_nLoggerType == LOGGER_GSTECH)
		return false;

	time_t t = time(NULL);
	if (m_nLogMethod == LOGTYPE_PERIODIC) {	// periodic
		// 시각에 이르렀으면 end condition을 찾은 것.
		if (time(NULL) > m_tEnd) {
			return true;
		}
	}
	else {					// detect
		if (COND_MATCH(m_condEnd)) {
			// duration이 지정되지 않았다면,
			if (m_condEnd.duration == 0) {
				return true;
			}
			// duration이 지정되었다면
			else {	
				if (m_condEnd.m_tStart == 0) {
					m_condEnd.m_tStart = t;
				}
				else if (t > m_condEnd.m_tStart + m_condEnd.duration) {
					m_condEnd.m_tStart = 0;
					return true;
				}
			}
		}
		else {
			m_condEnd.m_tStart = 0;
		}
	}

	return false;
}

bool CEquip::openLogFile()
{
	CString fn, s;

	// 처음에는 기존 파일에 추가하는 것으로 오픈
	m_sLogFilename.Format("%s\\%s.log", config.m_sLogPath, m_sID);
	if (!m_fileLog.Open(m_sLogFilename, CFile::modeNoTruncate | CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone)) {
		return false;
	}

	bool bReopen = false;
	ULONGLONG x = m_fileLog.GetLength();

	if (x > config.m_nServerLogFileSize) {
		// skip previous log messages
		while (m_fileLog.ReadString(s) && (x - m_fileLog.GetPosition()) > config.m_nServerLogFileSize)
			;
		bReopen = true;
	}

	if (x <= 0)
		return true;

	while (m_fileLog.ReadString(s)) {
		int pos = 0;

		CString tok = s.Tokenize(",", pos); 
		int e = atoi(tok);
		tok = s.Tokenize(",", pos);
		time_t t;
		config.s2t(tok, t);
		tok = s.Tokenize(",", pos);

		m_aLog.push_back(log_t(e, t, tok));
	}

	if (bReopen) {
		m_fileLog.Close();
		if (!m_fileLog.Open(m_sLogFilename, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone)) 
			return false;
		for (size_t i = 0; i < m_aLog.size(); i++) {
			s.Format("%d,%d,%s\n", m_aLog[i].e, m_aLog[i].t, m_aLog[i].msg);
			m_fileLog.WriteString(s);
		}
	}
	return true;
}

void CEquip::addLog(int e, const char* fmt, ...)
{
	va_list	ap;
	char buf[2048];
	time_t st = time(NULL);

	va_start(ap, fmt);
	vsprintf_s(buf, sizeof buf, fmt, ap);
	va_end(ap);

	m_sLastMsg = buf;

	if (m_aLog.size() > config.m_nLogMax) {
		m_aLog.erase(m_aLog.begin(), m_aLog.begin()+1);
	}
	m_aLog.push_back(log_t(e, st, buf));

	if (m_fileLog.m_hFile != INVALID_HANDLE_VALUE) {
		CString s;
		s.Format("%d,%s,", e, config.t2s(st));
		s += buf;
		s += '\n';
		m_fileLog.WriteString(s);
		m_fileLog.Flush();
	}
}

void CEquip::clearLogFile()
{
	m_aLog.clear();
	m_aTempLog.clear();
	m_fileLog.Close();
	DeleteFile(m_sLogFilename);
	openLogFile();
}

int CEquip::openTempFiles()
{
	CString dir;
	CString fn;
	time_t st = time(NULL);

	fn = config.t2s(st, "%Y-%m-%d_%H.%M.%S");
	dir.Format("%s\\%s", config.m_sLogPath, m_sID);
	if (!config.testDir(dir)) {
		addLog(LOG_ERROR, "can't create dir '%s'", dir);
		return LOG_ERROR;
	}

	m_sBinFilename.Format("%s\\%s.dat", dir, fn);
	m_sCsvFilename.Format("%s\\%s.csv", dir, fn);

	if (!m_fileCsv.Open(m_sCsvFilename, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone)) {
		addLog(LOG_ERROR, "csv file '%s' open error", m_sCsvFilename);
		return LOG_ERROR;
	}
	m_fileCsv.WriteString("DateTime, Temp\n");

	if (!m_fileBin.Open(m_sBinFilename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone)) {
		addLog(LOG_ERROR, "log file '%s' open error", m_sBinFilename);
		return LOG_ERROR;
	}

	m_bLogging = true;
	return LOG_OK;
}

void CEquip::closeTempFiles()
{
	m_bLogging = false;

	try {
		if (m_fileBin.m_hFile != INVALID_HANDLE_VALUE) {
			m_fileBin.Flush();
			m_fileBin.Close();
		}

		if (m_fileCsv.m_hFile != INVALID_HANDLE_VALUE) {
			m_fileCsv.Flush();
			m_fileCsv.Close();
		}
	}
	catch(...)
	{

	}
}

int CEquip::writeTemp(time_t t)
{
	double pv = m_fPV;

	if (!m_bLogging)
		return LOG_PEND;

	m_tLastLogged = t;
	m_nLogCount++;

	// log에 기록
	if (m_aTempLog.size() > config.m_nLogTempMax) {
		m_aTempLog.erase(m_aTempLog.begin(), m_aTempLog.begin()+1);
	}
	m_aTempLog.push_back(logfile_rec_t(m_tLastLogged, pv));

	// TODO : try catch가 error가 잡히는가?
	try {
		logfile_rec_t rec(m_tLastLogged, pv);

		if (m_bLogging) {
			m_fileBin.Write(&rec, sizeof(rec));
			m_fileBin.Flush();
		}
	}
	catch(...) // CFileException e)
	{
		addLog(LOG_ERROR, "data file '%s' write error", m_sBinFilename);
		return LOG_BIN_WRITE_ERROR;
	}

	try {
		CString s;
		s.Format("%s, %5.2f\n", config.t2s(m_tLastLogged), pv);
		if (m_bLogging) {
			m_fileCsv.WriteString(s);
			m_fileCsv.Flush();
		}
	}
	catch(...) // CFileException e)
	{
		addLog(LOG_ERROR, "csv file '%s' write error", m_sCsvFilename);
		return LOG_CSV_WRITE_ERROR;
	}
//	TRACE("%s: %s %.2f\n", __FUNCTION__, config.t2s(m_tLastLogged), pv);
	return LOG_OK;
}

int CEquip::run()
{
	time_t st = time(NULL);
	int ret;

	if (m_fileLog.m_hFile == INVALID_HANDLE_VALUE)
		openLogFile();

	// GSTECH logger는 데이터가 날아오지 않아도 체크하지 않음.
	// 다른 logger는 데이터가 일정 시간 동안 날아오지 않으면 에러로 처리
	if (m_nLoggerType != LOGGER_GSTECH && st > m_tLastUpdate + config.m_nLogTimeout) {
		if (m_bLogging) 
			closeTempFiles();
		if (!m_bErrorLogged) {
			addLog(LOG_ERROR, "server does not send temp data for %d secs", config.m_nLogTimeout);
		}
		return LOG_ERROR;
	}

	// lock되어 있으면 처리를 하지 않음
	if (m_bLock)
		return LOG_LOCKED;

	if (m_bLogging) {
		if (isEndCondition()) {
			closeTempFiles();
			return LOG_ENDED;
		}
		else {
			if (st < (m_tLastLogged + m_nInterval))
				return LOG_PEND;

			if ((ret = writeTemp(st)) != LOG_OK) {
				closeTempFiles();
				return ret;
			}
			return LOG_PROGRESS;
		}
	}
	else {
		if (isStartCondition()) {
			start();
			return LOG_STARTED;
		}
	}

	return LOG_PEND;
}

bool CEquip::stop(bool bForce)
{
	if (!m_bLogging)
		return false;

	closeTempFiles();
	if (bForce) {
		addLog(LOG_NORMAL, "'%s' log force stopped...", m_sBinFilename);
		setLock(true);
	}
	else
		addLog(LOG_NORMAL, "'%s' log stopped...", m_sBinFilename);
	return true;
}

bool CEquip::start()
{
	if (m_bLogging)
		return false;

	time_t st = time(NULL);
	m_tLogStart = st;
	m_nLogCount = 0;

	int ret = openTempFiles();
	if (ret != LOG_OK) {
		closeTempFiles();
		return false;
	}
	addLog(LOG_NORMAL, "'%s' log started...", m_sBinFilename);
	return true;
}

void CEquip::convert()
{
	// string to data
	m_nIP = inet_addr(m_sIP);
	m_nLoggerType = (m_sLoggerType == m_aLoggerType[0]) ? 0 : 1;
	m_nLogMethod = (m_sLogMethod == m_aLogMethod[0]) ? 0 : 1;
	m_fTolerance = atof(m_sTolerance);
	if (m_nInterval <= 0)
		m_nInterval = 1;

	m_condStart = s2cond(m_sStartCond);
	m_condEnd = s2cond(m_sEndCond);
	config.s2t(m_sStartTime, m_tStart);
	config.s2t(m_sEndTime, m_tEnd);

	// recalc for eye validation 
	struct sockaddr_in sa;
	sa.sin_addr.s_addr = (unsigned int)m_nIP;
	m_sIP = inet_ntoa(sa.sin_addr);
	m_sTolerance.Format("%.2f", m_fTolerance);
	m_sStartCond = cond2s(m_condStart);
	m_sEndCond = cond2s(m_condEnd);
	m_sStartTime = config.t2s(m_tStart);
	m_sEndTime = config.t2s(m_tEnd);
}