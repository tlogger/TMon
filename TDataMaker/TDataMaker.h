#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.

class CTdaemonSimApp : public CWinApp
{
public:
	CTdaemonSimApp();
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CTdaemonSimApp theApp;