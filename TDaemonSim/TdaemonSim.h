
// TdaemonSim.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CTdaemonSimApp:
// �� Ŭ������ ������ ���ؼ��� TdaemonSim.cpp�� �����Ͻʽÿ�.
//

class CTdaemonSimApp : public CWinApp
{
public:
	CTdaemonSimApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CTdaemonSimApp theApp;