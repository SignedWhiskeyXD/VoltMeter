
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
public:
	CComboBox ComboDevice;
	CListCtrl ListVoltData;

private:
	void UpdateVoltVal();
	
	std::vector<SerialPortInfo> availableDevices;
	CSerialPort meterPort;
	std::unique_ptr<VoltMeterSession> pMeterSession;
	int curSelDev = -1;

	int meterMode = 4;
	uint16_t rawValue = 0;
	
public:
	afx_msg void OnCbnSelchangeCombo1();
	CEdit EditBoxMsg;
	afx_msg void OnBnClickedButton4();
	CProgressCtrl ProgBarVolt;
};
