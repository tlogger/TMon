#pragma once

#include "compat.h"
#include <vector>

using namespace std;

// TOD ���� ���Ϸ� ����

#pragma pack(push, 1)

typedef struct LogFileRecordStruct 
{
	DWORD		t;				// win32 time_t�� 8byte. ���� 4byte�� ���
	short		temp;			// �µ��� /100�� ��

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
	// run() �Լ��� ���ϰ�
	enum {
		LOG_OK = 1000,				
		LOG_STARTED,			// �α� ��� ����
		LOG_ENDED,				// �α� ��� ����
		LOG_PROGRESS,			// ���۵Ǿ���, �αװ� ���Ϸ� ��ϵǸ�
		LOG_PEND,				// ������ �Ǿ����� ����� �� �Ǿ��� ��
		LOG_BIN_WRITE_ERROR,	// bin file write error
		LOG_CSV_WRITE_ERROR,	// csv file write error
		LOG_LOCKED,				// ���� ����Ǿ� lock�� �ɷ���.
	};

	enum {
		COND_EQUAL = 0,
		COND_BIG,
		COND_LESS
	};

	typedef struct condStruct 
	{
		int			op;			// 0='=', 1='>', 2='<'
		double		temp;		// �µ�
		time_t		duration;	// ��� ������ �����Ǿ�� �ϴ� �ð�.
		time_t		m_tStart;		// ������ ó�� ��Ī�� �� ��ϵǴ� �ð�.

		condStruct() {
			op = 0;
			temp = 25.0;
			duration = 0;
			m_tStart = 0;
		}
	} cond_t;

	// ȭ�� ��¿� �α� ����
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
	CString		m_sPrefix;			// �µ� �������� �ι���. �� �� ���� ���� üũ.
	int			m_nInterval;		// �µ� ���� ���� (secs)

	CString		m_sLogMethod;
	int			m_nLogMethod;		// 1=Detect, 0=Periodic

	// log method ����
	CString		m_sTolerance;
	double		m_fTolerance;		// m_nLogMethod=LOGTYPE_DETECT�� �� �µ��� ��� ����
	CString		m_sStartCond;		// m_nLogMethod=LOGTYPE_DETECT�� ���� ��ȿ
	CString		m_sEndCond;		
	cond_t		m_condStart;		// m_nLogMethod=LOGTYPE_DETECT�� ���� ��ȿ
	cond_t		m_condEnd;

	CString		m_sStartTime;
	CString		m_sEndTime;
	time_t		m_tStart;			// m_nLogMethod=LOGTYPE_PERIODIC�� ���� ��ȿ
	time_t		m_tEnd;				// m_nLogMethod=LOGTYPE_PERIODIC�� ���� ��ȿ

	// log ����
	double		m_fPV;				// ���������� ������Ʈ�� �µ�
	time_t		m_tLastUpdate;		// packet���κ��� ������ ������Ʈ �� �ð�
	bool		m_bErrorLogged;		// ������ ������Ʈ�� �ʰ� �� ��� �α׸� �� ���� ����ϱ� ���� ����ϴ� flag

	bool		m_bLogging;			// log ��� ��
									// m_bLogging=true: ������ �ð����� log�� ���ƿ��� ������ error
									// m_bLogging=false: ������ �ð����� log�� ���ƿ��� ������ warning
	time_t		m_tLogStart;		// log�� ���۵� �ð�
	time_t		m_tLastLogged;		// log�� ���������� ��ϵ� �ð�
	DWORD		m_nLogCount;		// log ���� ���� ��ϵ� log ����

	bool		m_bLock;			// Lock�� �ɷ������� ���� �ȵ�. ���� �ߴ��� ��� lock�� �ɾ� ����.

	// log files (��ϸ� ��)
	CFile		m_fileBin;		
	CStdioFile	m_fileCsv;
	CStdioFile	m_fileLog;			// ��� �α� 

	CString		m_sBinFilename;
	CString		m_sCsvFilename;
	CString		m_sLogFilename;

	// ȭ�� ��¿� data
	CString		m_sLastMsg;
	vector<logfile_rec_t>	m_aTempLog;	// temp log�� ���Ͽ� ������� ����
	vector<log_t>			m_aLog;		// log message�� ���Ͽ� ���
	HWND		m_hWnd;

	static const char*	m_aLoggerType[];
	static const char*	m_aLogMethod[];
};
