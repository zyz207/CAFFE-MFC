
// Caffe-MFCDlg.h : ͷ�ļ�
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
	BOOL b;//�Ƿ�����߳�


}ThreadParam;

enum Enum_Thread_State
{
	none,close,start,  pause
};

// CCaffeMFCDlg �Ի���
class CCaffeMFCDlg : public CDialogEx
{
// ����
public:
	CCaffeMFCDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CAFFEMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();

public:
	//������ʾͼƬ
	CRect rect;
	CStatic* pStc;//��ʶPicture Control
	CDC* pDC;
	HDC hDC;

	//�߳�
	CWinThread*		m_MYpThread;
	ThreadParam m_ThrandParam;
	Enum_Thread_State m_Thread_State;//�߳�״̬ 
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
