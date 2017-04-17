
// Caffe-MFCDlg.cpp : 实现文件
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

DWORD dwExitCode = 0;//线程标记
#define WM_MY_MESSAGE (WM_USER+101)  

Mat g_frame;//摄像头当前帧
VideoCapture g_cap;//摄像头

// CCaffeMFCDlg 对话框



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


// CCaffeMFCDlg 消息处理程序

BOOL CCaffeMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	//线程
	m_Thread_State = none;

	//绘图
	pStc = NULL;
	pDC = NULL;
	pStc = (CStatic *)GetDlgItem(IDC_IMAGE);//标识所标记的控制窗口句柄
	pStc->GetClientRect(&rect);//获取并传递坐标
	pDC = pStc->GetDC();//获取设备上下文
	hDC = pDC->GetSafeHdc();//获取设备上下文的句柄

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCaffeMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCaffeMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCaffeMFCDlg::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类
	cv::destroyAllWindows();
	CDialogEx::OnCancel();
}

//线程运行内容
UINT WorkThread(LPVOID pParam)
{
	ThreadParam *plParam = (ThreadParam*)pParam;
	int i = plParam->n;

	//打开第一个摄像头
	g_cap.open(0);
	//检查是否成功打开
	if (!g_cap.isOpened())
	{
		AfxMessageBox("摄像头开启失败");
		return 0;
	}

	while (1)
	{
		//从 cap 中读一帧，存到 frame
		g_cap >> g_frame;
		i++;
		if (g_frame.empty())//如果未读到图像
			break;
		
		::PostMessage(plParam->hWnd, WM_MY_MESSAGE, (WPARAM)i, TRUE);
		waitKey(30);
	}
	return 0;
}

//开始线程
void CCaffeMFCDlg::Thread_Begin()
{
	m_ThrandParam.hWnd = m_hWnd;
	m_ThrandParam.n = 0;
	m_ThrandParam.b = TRUE;
	m_Thread_State = start;
	m_MYpThread = AfxBeginThread(WorkThread, &m_ThrandParam, THREAD_PRIORITY_BELOW_NORMAL, 0, CREATE_SUSPENDED);
	m_MYpThread->m_bAutoDelete = FALSE;		///线程结束不自动撤销
	m_MYpThread->ResumeThread();	///恢复线程运行	
}

afx_msg LRESULT CCaffeMFCDlg::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	//读取摄像头
	CString str;
	CvvImage cvimg;
	cvimg.CopyOf(&IplImage(g_frame));
	cvimg.DrawToHDC(hDC, &rect);//显示在图片控件
	//	ReleaseDC(pDC);//释放pDC的内存

	str.Format("%d帧", (int)wParam);
	m_ShowMessage1.SetWindowTextA(str);
	return 0;
}
//暂停线程
void CCaffeMFCDlg::Thread_Pause()
{
	if (m_Thread_State != start)
	{
		return;
	}
	m_MYpThread->SuspendThread();
	m_Thread_State = pause;
}

//继续线程
void CCaffeMFCDlg::Thread_Resume()
{
	if (m_Thread_State != pause)
	{
		return;
	}
	m_MYpThread->ResumeThread();
	m_Thread_State = start;
}

//结束线程
void CCaffeMFCDlg::Thread_End()
{
	m_ThrandParam.b = FALSE;
	m_Thread_State = close;
	//	ReleaseDC(pDC);//释放pDC的内存
	g_cap.release();
}


//打开摄像头
void CCaffeMFCDlg::OnBnClickedCamera()
{
	int i;

	if (m_Thread_State < start)
	{
		Thread_Begin();
		m_Begin_End.SetWindowTextA("结束读取摄像头");
		m_Pause_Resume.EnableWindow(TRUE);
	}
	else
	{
		Thread_End();
		m_Begin_End.SetWindowTextA("开始读取摄像头");
		m_Pause_Resume.EnableWindow(FALSE);
	}
}


void CCaffeMFCDlg::OnBnClickedPause()
{
	if (m_Thread_State == start)
	{
		Thread_Pause();
		m_Pause_Resume.SetWindowTextA("继续读取摄像头");
	}
	else if (m_Thread_State == pause)
	{
		Thread_Resume();
		m_Pause_Resume.SetWindowTextA("暂停读取摄像头");
	}
}
