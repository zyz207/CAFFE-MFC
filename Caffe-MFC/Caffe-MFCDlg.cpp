
// Caffe-MFCDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Caffe-MFC.h"
#include "Caffe-MFCDlg.h"
#include "afxdialogex.h"
#include "CvvImage.h"
#include <thread>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD dwExitCode = 0;//�̱߳��
#define WM_MY_MESSAGE (WM_USER+101)  

Mat g_frame;//����ͷ��ǰ֡
VideoCapture g_cap;//����ͷ

// CCaffeMFCDlg �Ի���



CCaffeMFCDlg::CCaffeMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCaffeMFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCaffeMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Camera, m_Begin_End);
	DDX_Control(pDX, IDC_PAUSE, m_Pause_Resume);
	DDX_Control(pDX, IDC_EDIT1, m_ShowMessage1);
}

BEGIN_MESSAGE_MAP(CCaffeMFCDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_MY_MESSAGE, &CCaffeMFCDlg::OnMyMessage)
	ON_BN_CLICKED(IDC_Camera, &CCaffeMFCDlg::OnBnClickedCamera)
	ON_BN_CLICKED(IDC_PAUSE, &CCaffeMFCDlg::OnBnClickedPause)
END_MESSAGE_MAP()


// CCaffeMFCDlg ��Ϣ�������

BOOL CCaffeMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	//�߳�
	m_Thread_State = none;

	//��ͼ
	pStc = NULL;
	pDC = NULL;
	pStc = (CStatic *)GetDlgItem(IDC_IMAGE);//��ʶ����ǵĿ��ƴ��ھ��
	pStc->GetClientRect(&rect);//��ȡ����������
	pDC = pStc->GetDC();//��ȡ�豸������
	hDC = pDC->GetSafeHdc();//��ȡ�豸�����ĵľ��

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCaffeMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCaffeMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCaffeMFCDlg::OnCancel()
{
	// TODO:  �ڴ����ר�ô����/����û���
	cv::destroyAllWindows();
	CDialogEx::OnCancel();
}

//�߳���������
UINT WorkThread(LPVOID pParam)
{
	ThreadParam *plParam = (ThreadParam*)pParam;
	int i = plParam->n;

	//�򿪵�һ������ͷ
	g_cap.open(0);
	//����Ƿ�ɹ���
	if (!g_cap.isOpened())
	{
		AfxMessageBox("����ͷ����ʧ��");
		return 0;
	}

	while (1)
	{
		//�� cap �ж�һ֡���浽 frame
		g_cap >> g_frame;
		i++;
		if (g_frame.empty())//���δ����ͼ��
			break;
		
		::PostMessage(plParam->hWnd, WM_MY_MESSAGE, (WPARAM)i, TRUE);
		waitKey(30);
	}
	return 0;
}

//��ʼ�߳�
void CCaffeMFCDlg::Thread_Begin()
{
	m_ThrandParam.hWnd = m_hWnd;
	m_ThrandParam.n = 0;
	m_ThrandParam.b = TRUE;
	m_Thread_State = start;
	m_MYpThread = AfxBeginThread(WorkThread, &m_ThrandParam, THREAD_PRIORITY_BELOW_NORMAL, 0, CREATE_SUSPENDED);
	m_MYpThread->m_bAutoDelete = FALSE;		///�߳̽������Զ�����
	m_MYpThread->ResumeThread();	///�ָ��߳�����	
}

afx_msg LRESULT CCaffeMFCDlg::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����ͷ
	CString str;
	CvvImage cvimg;
	cvimg.CopyOf(&IplImage(g_frame));
	cvimg.DrawToHDC(hDC, &rect);//��ʾ��ͼƬ�ؼ�
	//	ReleaseDC(pDC);//�ͷ�pDC���ڴ�

	str.Format("%d֡", (int)wParam);
	m_ShowMessage1.SetWindowTextA(str);
	return 0;
}
//��ͣ�߳�
void CCaffeMFCDlg::Thread_Pause()
{
	if (m_Thread_State != start)
	{
		return;
	}
	m_MYpThread->SuspendThread();
	m_Thread_State = pause;
}

//�����߳�
void CCaffeMFCDlg::Thread_Resume()
{
	if (m_Thread_State != pause)
	{
		return;
	}
	m_MYpThread->ResumeThread();
	m_Thread_State = start;
}

//�����߳�
void CCaffeMFCDlg::Thread_End()
{
	m_ThrandParam.b = FALSE;
	m_Thread_State = close;
	//	ReleaseDC(pDC);//�ͷ�pDC���ڴ�
	g_cap.release();
}


//������ͷ
void CCaffeMFCDlg::OnBnClickedCamera()
{
	int i;

	if (m_Thread_State < start)
	{
		Thread_Begin();
		m_Begin_End.SetWindowTextA("������ȡ����ͷ");
		m_Pause_Resume.EnableWindow(TRUE);
	}
	else
	{
		Thread_End();
		m_Begin_End.SetWindowTextA("��ʼ��ȡ����ͷ");
		m_Pause_Resume.EnableWindow(FALSE);
	}
}


void CCaffeMFCDlg::OnBnClickedPause()
{
	if (m_Thread_State == start)
	{
		Thread_Pause();
		m_Pause_Resume.SetWindowTextA("������ȡ����ͷ");
	}
	else if (m_Thread_State == pause)
	{
		Thread_Resume();
		m_Pause_Resume.SetWindowTextA("��ͣ��ȡ����ͷ");
	}
}
