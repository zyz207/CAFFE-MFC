
// Caffe-MFC.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCaffeMFCApp: 
// �йش����ʵ�֣������ Caffe-MFC.cpp
//

class CCaffeMFCApp : public CWinApp
{
public:
	CCaffeMFCApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCaffeMFCApp theApp;