
// VoltMeterDlg.h: 头文件
//

#pragma once
#include <vector>
#include <CSerialPort/SerialPort.h>
#include <CSerialPort/SerialPortInfo.h>
#include "VoltMeterSession.h"
#include <memory>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
using namespace itas109;

// CVoltMeterDlg 对话框
class CVoltMeterDlg : public CDialogEx
{
// 构造
public:
	CVoltMeterDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VOLTMETER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnDropdownCombo1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	void LoadVoltData();
	void UpdateVoltVal();
	
	std::fstream fs;
	std::vector<SerialPortInfo> availableDevices;
	CSerialPort meterPort;
	std::unique_ptr<VoltMeterSession> pMeterSession;

	bool isFreeze = false;
	int curSelDev = -1;
	int meterMode = 4;
	uint16_t rawValue = 0;
	double convertVal = 0;

	CComboBox ComboDevice;
	CProgressCtrl ProgBarVolt;
	CListCtrl ListVoltData;
	CEdit EditBoxVolt;
	CEdit EditBoxMsg;
};
