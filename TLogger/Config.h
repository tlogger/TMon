#pragma once

#include <vector>
#include "Equip.h"

using namespace std;

// �Ʒ� ����� GSTECH���� ���� ���ǰ� �� ������ �Ǵ��ϴ� �µ�
// �� ���� �µ� �����Ϳ� ����� �� ���� ����/�� ���ǿ��� ���.

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

	// ���� �Լ���
	bool		testDir(const char* dir);
	CString&	convertValidFilename(CString& s, bool bPath = false);
	char*		t2s(time_t t, const char* fmt = NULL);
	void		s2t(const char* s, time_t& t);
	char*		d2s(time_t tm);
	time_t		s2d(const char* s);

public:
	vector<CEquip*>	m_aEquip;				// ��� ����Ʈ
	CString		m_sLogPath;					// �α� ���丮 root
	CString		m_sConfigFn;				// config ���ϸ�

	int			m_nPort;					// UDP Server ��Ʈ ��ȣ
	size_t		m_nServerLogFileSize;		// ���� �α� ���� ũ��
	size_t		m_nLogFileSize;				// ��� �α� ���� ũ��
	size_t		m_nLogMax;					// ȭ�鿡 ����� ��� �α� ����
	size_t		m_nLogTempMax;				// ȭ�鿡 ����� �µ� �α� ����
	time_t		m_nLogTimeout;				// �µ� ������ timeout ��
};
