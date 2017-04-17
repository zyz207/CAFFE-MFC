
// Caffe-MFCDlg.h : 头文件
//

#pragma once
#include <opencv2/opencv.hpp>
#include "afxwin.h"
using namespace std;
using namespace cv;

typedef	struct	__ThreadParam__
{
	HWND	hWnd;
	int n;
	BOOL b;//是否继续线程


}ThreadParam;

enum Enum_Thread_State
{
	none,close,start,  pause
};

// CCaffeMFCDlg 对话框
class CCaffeMFCDlg : public CDialogEx
{
// 构造
public:
	CCaffeMFCDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CAFFEMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();

public:
	//用于显示图片
	CRect rect;
	CStatic* pStc;//标识Picture Control
	CDC* pDC;
	HDC hDC;

	//线程
	CWinThread*		m_MYpThread;
	ThreadParam m_ThrandParam;
	Enum_Thread_State m_Thread_State;//线程状态 
	void Thread_Begin();
	void Thread_Pause();
	void Thread_Resume();
	void Thread_End();
protected:
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedCamera();
	CButton m_Begin_End;
	CButton m_Pause_Resume;
	afx_msg void OnBnClickedPause();
	CEdit m_ShowMessage1;
};
