#pragma once

#include "compat.h"
#include <vector>

using namespace std;

// TOD 별도 파일로 빼기

#pragma pack(push, 1)

typedef struct LogFileRecordStruct 
{
	DWORD		t;				// win32 time_t는 8byte. 이중 4byte만 사용
	short		temp;			// 온도는 /100한 값

	LogFileRecordStruct() {  t = (DWORD)time(NULL); temp = 0; }
	LogFileRecordStruct(time_t tt, double x) { t = (DWORD)tt; temp = short(x * 100.0); }

} logfile_rec_t;

#pragma pack(pop, 1)

class CEquip
{
public:
	CEquip(void);
	virtual ~CEquip(void);

	enum {
		LOGGER_GSTECH = 0,
		LOGGER_OTHER = 1
	};

	enum {
		LOGTYPE_PERIODIC = 0,
		LOGTYPE_DETECT,
	};
	
	enum {
		LOG_GSTECH_START = WM_USER + 105,
		LOG_GSTECH_END,
	};
	// run() 함수의 리턴값
	enum {
		LOG_OK = 1000,				
		LOG_STARTED,			// 로그 기록 시작
		LOG_ENDED,				// 로그 기록 종료
		LOG_PROGRESS,			// 시작되었고, 로그가 파일로 기록되면
		LOG_PEND,				// 시작은 되었으나 기록이 안 되었을 때
		LOG_BIN_WRITE_ERROR,	// bin file write error
		LOG_CSV_WRITE_ERROR,	// csv file write error
		LOG_LOCKED,				// 강제 종료되어 lock이 걸렸음.
	};

	enum {
		COND_EQUAL = 0,
		COND_BIG,
		COND_LESS
	};

	typedef struct condStruct 
	{
		int			op;			// 0='=', 1='>', 2='<'
		double		temp;		// 온도
		time_t		duration;	// 상기 조건이 유지되어야 하는 시간.
		time_t		m_tStart;		// 조건이 처음 매칭될 때 기록되는 시각.

		condStruct() {
			op = 0;
			temp = 25.0;
			duration = 0;
			m_tStart = 0;
		}
	} cond_t;

	// 화면 출력용 로그 구조
	typedef struct LogStruct {
		int			e;			// error type 
		time_t		t;			// timestamp
		CString		msg;		// log message

		LogStruct(int ee, time_t tt, const char* s)
		{
			e = ee;
			t = tt;
			msg = s;
		}
	} log_t;

public:
	int			run();
	bool		start();
	bool		stop(bool bForce = false);
	inline void	setLock(bool x) { m_bLock = x; }
	void		putTemp(double f);
	void		clearLogFile();

	void		convert();
	cond_t&		s2cond(const char* s);
	CString&	cond2s(cond_t& cond);

private:
	void		closeTempFiles();
	int			openTempFiles();
	int			writeTemp(time_t t);

	bool		openLogFile();
	void		addLog(int e, const char* fmt, ...);

	bool		isStartCondition();
	bool		isEndCondition();

	inline void	post(DWORD msg, WPARAM arg1, LPARAM arg2) { if (m_hWnd) ::PostMessage(m_hWnd, msg, arg1, arg2); }

public:
	int			m_nIndex;
	CString		m_sID;				// equipment id
	CString		m_sLoggerType;
	int			m_nLoggerType;		// 0=GSTECH, 1=Logger
	CString		m_sIP;				// remote ip address
	DWORD		m_nIP;
	CString		m_sPrefix;			// 온도 데이터의 두문자. 비교 후 수신 여부 체크.
	int			m_nInterval;		// 온도 저장 간격 (secs)

	CString		m_sLogMethod;
	int			m_nLogMethod;		// 1=Detect, 0=Periodic

	// log method 관련
	CString		m_sTolerance;
	double		m_fTolerance;		// m_nLogMethod=LOGTYPE_DETECT일 때 온도의 허용 오차
	CString		m_sStartCond;		// m_nLogMethod=LOGTYPE_DETECT일 때만 유효
	CString		m_sEndCond;		
	cond_t		m_condStart;		// m_nLogMethod=LOGTYPE_DETECT일 때만 유효
	cond_t		m_condEnd;

	CString		m_sStartTime;
	CString		m_sEndTime;
	time_t		m_tStart;			// m_nLogMethod=LOGTYPE_PERIODIC일 때만 유효
	time_t		m_tEnd;				// m_nLogMethod=LOGTYPE_PERIODIC일 때만 유효

	// log 관련
	double		m_fPV;				// 마지막으로 업데이트된 온도
	time_t		m_tLastUpdate;		// packet으로부터 마지막 업데이트 된 시각
	bool		m_bErrorLogged;		// 마지막 업데이트가 늦게 올 경우 로그를 한 번만 기록하기 위해 사용하는 flag

	bool		m_bLogging;			// log 기록 중
									// m_bLogging=true: 지정한 시간동안 log가 날아오지 않으면 error
									// m_bLogging=false: 지정한 시간동안 log가 날아오지 않으면 warning
	time_t		m_tLogStart;		// log가 시작된 시각
	time_t		m_tLastLogged;		// log에 마지막으로 기록된 시각
	DWORD		m_nLogCount;		// log 시작 이후 기록된 log 갯수

	bool		m_bLock;			// Lock이 걸려있으면 시작 안됨. 강제 중단할 경우 lock을 걸어 놓음.

	// log files (기록만 함)
	CFile		m_fileBin;		
	CStdioFile	m_fileCsv;
	CStdioFile	m_fileLog;			// 장비 로그 

	CString		m_sBinFilename;
	CString		m_sCsvFilename;
	CString		m_sLogFilename;

	// 화면 출력용 data
	CString		m_sLastMsg;
	vector<logfile_rec_t>	m_aTempLog;	// temp log는 파일에 기록하지 않음
	vector<log_t>			m_aLog;		// log message는 파일에 기록
	HWND		m_hWnd;

	static const char*	m_aLoggerType[];
	static const char*	m_aLogMethod[];
};
