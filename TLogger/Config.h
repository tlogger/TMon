#pragma once

#include <vector>
#include "Equip.h"

using namespace std;

// 아래 상수는 GSTECH에서 시작 조건과 끝 조건을 판단하는 온도
// 이 값은 온도 데이터에 사용할 수 없고 시작/끝 조건에만 사용.

#define LOGGER_GSTECH_START_COND	(456.00)
#define LOGGER_GSTECH_END_COND		(789.00)

class CConfig
{
public:
	CConfig(void);
	virtual ~CConfig(void);

public:
	bool		load(const char* fn);
	bool		save();

	// 범용 함수군
	bool		testDir(const char* dir);
	CString&	convertValidFilename(CString& s, bool bPath = false);
	char*		t2s(time_t t, const char* fmt = NULL);
	void		s2t(const char* s, time_t& t);
	char*		d2s(time_t tm);
	time_t		s2d(const char* s);

public:
	vector<CEquip*>	m_aEquip;				// 장비 리스트
	CString		m_sLogPath;					// 로그 디렉토리 root
	CString		m_sConfigFn;				// config 파일명

	int			m_nPort;					// UDP Server 포트 번호
	size_t		m_nServerLogFileSize;		// 서버 로그 파일 크기
	size_t		m_nLogFileSize;				// 장비별 로그 파일 크기
	size_t		m_nLogMax;					// 화면에 출력할 장비별 로그 갯수
	size_t		m_nLogTempMax;				// 화면에 출력할 온도 로그 갯수
	time_t		m_nLogTimeout;				// 온도 데이터 timeout 값
};
