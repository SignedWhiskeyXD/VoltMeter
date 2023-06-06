
// VoltMeterDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "VoltMeter.h"
#include "VoltMeterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVoltMeterDlg 对话框



CVoltMeterDlg::CVoltMeterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VOLTMETER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVoltMeterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, ComboDevice);
	DDX_Control(pDX, IDC_LIST1, ListVoltData);
	DDX_Control(pDX, IDC_EDIT4, EditBoxMsg);
	DDX_Control(pDX, IDC_PROGRESS1, ProgBarVolt);
	DDX_Control(pDX, IDC_EDIT2, EditBoxVolt);
}

BEGIN_MESSAGE_MAP(CVoltMeterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CVoltMeterDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON4, &CVoltMeterDlg::OnBnClickedButton4)
	ON_CBN_DROPDOWN(IDC_COMBO1, &CVoltMeterDlg::OnCbnDropdownCombo1)
	ON_BN_CLICKED(IDC_BUTTON5, &CVoltMeterDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON1, &CVoltMeterDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CVoltMeterDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CVoltMeterDlg 消息处理程序

BOOL CVoltMeterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	ComboDevice.SetWindowTextW(L"请选择串口设备");

	ListVoltData.SetExtendedStyle(ListVoltData.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	ListVoltData.InsertColumn(0, L"序号", LVCFMT_LEFT, 60);
	ListVoltData.InsertColumn(1, L"测量值", LVCFMT_LEFT, 150);
	ListVoltData.InsertColumn(2, L"设备名", LVCFMT_LEFT, 115);
	LoadVoltData();

	std::thread threadProgBar(&CVoltMeterDlg::UpdateVoltVal, this);
	threadProgBar.detach();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CVoltMeterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVoltMeterDlg::OnPaint()
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
HCURSOR CVoltMeterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVoltMeterDlg::LoadVoltData()
{
	ListVoltData.DeleteAllItems();
	fs.open("log.txt", std::ios::in);
	std::string line;
	int i = 0;
	while (std::getline(fs, line)) {
		std::stringstream ss(line);
		std::string id, val, device;
		ss >> id >> val >> device;
		ListVoltData.InsertItem(i, CString(id.c_str()));
		ListVoltData.SetItemText(i, 1, CString(val.c_str()));
		ListVoltData.SetItemText(i++, 2, CString(device.c_str()));
	}
	fs.close();
}

void CVoltMeterDlg::UpdateVoltVal()
{
	const static double rangeTab[5] = {39.0625, 156.25, 625.0, 2500.0, 5000.0};

	while(true){
		if (pMeterSession) {
			meterMode = pMeterSession->getRange();
			rawValue = pMeterSession->getRawValue();
		}

		convertVal = rangeTab[meterMode] * rawValue / 65535;
		CString voltStr;
		voltStr.Format(L"测量值：%.3lfmV 原始值：%d@L%d", convertVal, rawValue, meterMode);
		ProgBarVolt.SetPos(((double)rawValue / 65535) * 100);
		EditBoxVolt.SetWindowTextW(voltStr);

		
		if (isFreeze) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			isFreeze = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}



void CVoltMeterDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	curSelDev = ComboDevice.GetCurSel();
	EditBoxMsg.SetWindowTextW(L"已选择设备!");
}


void CVoltMeterDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	if (curSelDev >= 0 && curSelDev < availableDevices.size()) {
		meterPort.init(
			availableDevices[curSelDev].portName,
			BaudRate9600, // baudrate
			ParityNone,   // parity
			DataBits8,    // data bit
			StopOne,      // stop bit
			FlowNone,     // flow
			4096
		);
		meterPort.setReadIntervalTimeout(0);
		meterPort.open();
		meterPort.writeData("4", 1);

		meterPort.disconnectReadEvent();
		pMeterSession = std::make_unique<VoltMeterSession>(VoltMeterSession(&meterPort));
		meterPort.connectReadEvent(pMeterSession.get());

		EditBoxMsg.SetWindowTextW(L"已成功连接至设备!");
	}
	else {
		EditBoxMsg.SetWindowTextW(L"连接失败!");
	}
}


void CVoltMeterDlg::OnCbnDropdownCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	EditBoxMsg.SetWindowTextW(L"请选择要连接的设备");
	this->availableDevices = CSerialPortInfo::availablePortInfos();
	ComboDevice.ResetContent();
	for (const auto& dev : availableDevices) {
		CString deviceInfo(dev.portName);
		deviceInfo.AppendChar(' ');
		deviceInfo.Append(CString(dev.description));
		ComboDevice.AddString(deviceInfo);
	}
}


void CVoltMeterDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	isFreeze = true;
	EditBoxMsg.SetWindowTextW(L"示数已冻结");
}


void CVoltMeterDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (curSelDev < 0 || curSelDev >= availableDevices.size()) {
		EditBoxMsg.SetWindowTextW(L"未连接设备！");
		return;
	}

	int id = ListVoltData.GetItemCount() + 1;
	CString tempStr;
	tempStr.Format(L"%d", id);
	ListVoltData.InsertItem(id - 1, tempStr);
	tempStr.Format(L"%.3lfmV", convertVal);
	ListVoltData.SetItemText(id - 1, 1, tempStr);
	ListVoltData.SetItemText(id - 1, 2, CString(availableDevices[curSelDev].portName));
	
	fs.open("log.txt", std::ios::out | std::ios::app);
	std::stringstream ss;
	ss << id << '\t' << convertVal << "mV\t" << availableDevices[curSelDev].portName;
	fs << ss.str() << std::endl;
	fs.close();

	EditBoxMsg.SetWindowTextW(L"已保存测量值");
}


void CVoltMeterDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	ListVoltData.DeleteAllItems();
	fs.open("log.txt", std::ios::out);
	fs.close();
	EditBoxMsg.SetWindowTextW(L"已清空记录值");
}
