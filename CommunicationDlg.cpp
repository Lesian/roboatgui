
// CommunicationDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Communication.h"
#include "CommunicationDlg.h"
#include "afxdialogex.h"

CString c;

CString test;         //�����������ַ���
CString data;         //���巢���ַ���
CString speed;        //����ͨ������
CString motion;       //�����˶�ģ̬
int nIndex;           //��ȡCombox��������м����

#define STARTBYTE 0xAA
#define ENDBYTE 0xFC
#define ROBOTID 0x01

char UART_Command[20];
char COMMANDMODE;
char DATAAREA[4];
char ROBOTSPEED=0x01;
char ROBOTTURNSPEED=0x00;
int Command_Length = 20;

char  FBSpeed;//forward backward speed parameter
char  FBDirection;//forward backward direction parameter
char  LRSpeed; //left right speed parameter
char  LRDirection; //Left right direction parameter
char  SpanningSpeed;
char  SpanningDirection;


BYTE Comm_Packet_Len;    //��Ҫ���յ������ݸ���
BYTE Comm_DataBuffer[20];   //���δ�Ŵ��ڽ��յ�������
long Comm_Packet_Index;    //ÿ���յ����ݸ���������־����Ӧ����ȫ������λ�õ�����
char temp4byte[4];
char ReceiveStateCount = 0;
float StatesData[4];
FILE *Statefile; //���ڼ�¼ÿ�β������γ����ݿ�
CFile RobotState;

unsigned char* Send_float_to_byte(float temp);
float Receive_byte_to_float(char* temp);
unsigned char Send[4];
unsigned char* Send_float_to_byte(float temp)
{
	union UFLOAT
	{
		float f;
		unsigned char byte[4];
	};
	union UFLOAT data;
	int i;
	data.f = temp;
	for (i = 0; i<4; i++)
		Send[i] = data.byte[i];
	return Send;
}

float Receive_byte_to_float(char* temp)
{
	union UFLOAT
	{
		float f;
		char byte[4];
	};
	union UFLOAT data;
	char i;
	for (i = 0; i<4; i++)
		data.byte[i] = *(temp + i);
	return data.f;
}


//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
//void MSCommSend(unsigned char * data, unsigned int len)
//{
//	CByteArray array; //����һ���ֽڼ� 
//	array.RemoveAll(); //������� 
//	if (0 != len)
//	{
//		array.SetSize(len); //�趨���鳤�� 
//							//����������д�뵽��ʱ���� 
//		for (UINT i = 0; i<len; i++)
//		{
//			array.SetAt(i, *data++);
//		}
//		//����mscomm�������ݣ�COleVariant���Բ�ٶȰ� 
//	//	CCommunicationDlg::m_mscomm.put_Output(COleVariant(array));
//		m_mscomm.put_Output(COleVariant(array));
//}






#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCommunicationDlg �Ի���




CCommunicationDlg::CCommunicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCommunicationDlg::IDD, pParent)
	, m_receive_edit(_T(""))
	, m_strRXData(_T(""))
	, ForwardBackwardSpeed(_T(""))
	, m_forwardbackspeed(_T(""))
	, m_forwardbackdirection(_T(""))
	, m_LeftRightSpeed(_T(""))
	, m_LeftRightDirection(_T(""))
	, m_CWCounterCWSpeed(_T(""))
	, m_CWCounterCWDirection(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCommunicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PORT_COMBO, m_port_combo);
	DDX_Control(pDX, IDC_BAUD_COMBO, m_baud_combo);
	DDX_Control(pDX, IDC_JIAOYAN_COMBO, m_jiaoyan_combo);
	DDX_Control(pDX, IDC_DATABIT_COMBO, m_databit_combo);
	DDX_Control(pDX, IDC_STOPBIT_COMBO, m_stopbit_combo);
	DDX_Control(pDX, IDC_SPEED_COMBO, m_speed_combo);
	DDX_Control(pDX, IDC_ZAIBO_COMBO, m_zaibo_combo);
	DDX_Control(pDX, IDC_MSCOMM, m_mscomm);
	DDX_Text(pDX, IDC_RECEIVE_EDIT, m_receive_edit);
	DDX_Control(pDX, IDC_OPEN_BUTTON, m_open_button);
	DDX_Control(pDX, IDC_CLOSE_BUTTON, m_close_button);
	DDX_Control(pDX, IDC_FORWARD_BUTTON, m_forward_button);
	DDX_Control(pDX, IDC_BACKWARD_BUTTON, m_backward_button);
	DDX_Control(pDX, IDC_LEFT_BUTTON, m_left_button);
	DDX_Control(pDX, IDC_RIGHT_BUTTON, m_right_button);
	DDX_Control(pDX, IDC_RISE_BUTTON, m_rise_button);
	DDX_Control(pDX, IDC_DOWN_BUTTON, m_down_button);
	//  DDX_Control(pDX, IDC_EDIT_RXDATA, m_ctrlComm);
	DDX_Text(pDX, IDC_EDIT_RXDATA, m_strRXData);
	DDX_Control(pDX, IDC_SLIDER_SPEED, m_slider_speed);
	DDX_Control(pDX, IDC_EDIT_RXDATA, m_EditView);
	DDX_Text(pDX, IDC_EDIT_FORWARD, m_forwardbackspeed);
	DDX_Text(pDX, IDC_EDIT_FORWARDBACKWARD_DIRECTION, m_forwardbackdirection);
	DDX_Text(pDX, IDC_EDIT_LEFTRIGHTSPEED, m_LeftRightSpeed);
	DDX_Text(pDX, IDC_EDIT2, m_LeftRightDirection);
	DDX_Text(pDX, IDC_EDIT3, m_CWCounterCWSpeed);
	DDX_Text(pDX, IDC_EDIT4, m_CWCounterCWDirection);
}

BEGIN_MESSAGE_MAP(CCommunicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CCommunicationDlg::OnBnClickedClearButton)
	ON_BN_CLICKED(IDC_OPEN_BUTTON, &CCommunicationDlg::OnBnClickedOpenButton)
	ON_BN_CLICKED(IDC_SET_BUTTON, &CCommunicationDlg::OnBnClickedSetButton)
	ON_BN_CLICKED(IDC_CLOSE_BUTTON, &CCommunicationDlg::OnBnClickedCloseButton)
	ON_BN_CLICKED(IDC_FORWARD_BUTTON, &CCommunicationDlg::OnBnClickedForwardButton)
	ON_BN_CLICKED(IDC_BACKWARD_BUTTON, &CCommunicationDlg::OnBnClickedBackwardButton)
	ON_BN_CLICKED(IDC_LEFT_BUTTON, &CCommunicationDlg::OnBnClickedLeftButton)
	ON_BN_CLICKED(IDC_RIGHT_BUTTON, &CCommunicationDlg::OnBnClickedRightButton)
	ON_BN_CLICKED(IDC_RISE_BUTTON, &CCommunicationDlg::OnBnClickedRiseButton)
	ON_BN_CLICKED(IDC_DOWN_BUTTON, &CCommunicationDlg::OnBnClickedDownButton)
	ON_BN_CLICKED(IDC_BUTTON_TURNLEFTCIRCLE, &CCommunicationDlg::OnBnClickedButtonTurnleftcircle)
	ON_BN_CLICKED(IDC_BUTTON_TurnRightCircle, &CCommunicationDlg::OnBnClickedButtonTurnrightcircle)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CCommunicationDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_Direction_SET, &CCommunicationDlg::OnBnClickedButtonDirectionSet)
//	ON_EN_CHANGE(IDC_EDIT_RXDATA, &CCommunicationDlg::OnChangeEditRxdata)
//ON_EN_CHANGE(IDC_EDIT_RXDATA, &CCommunicationDlg::OnChangeEditRxdata)
ON_BN_CLICKED(IDC_BUTTON_Left_INC, &CCommunicationDlg::OnBnClickedButtonLeftInc)
ON_BN_CLICKED(IDC_BUTTON_Left_DEC, &CCommunicationDlg::OnBnClickedButtonLeftDec)
ON_BN_CLICKED(IDC_BUTTON_TIGHT_INC, &CCommunicationDlg::OnBnClickedButtonTightInc)
ON_BN_CLICKED(IDC_BUTTON_RIGHT_DEC, &CCommunicationDlg::OnBnClickedButtonRightDec)
ON_BN_CLICKED(IDC_BUTTON_FRONT_INC, &CCommunicationDlg::OnBnClickedButtonFrontInc)
ON_BN_CLICKED(IDC_BUTTON_FRONT_DEC, &CCommunicationDlg::OnBnClickedButtonFrontDec)
ON_BN_CLICKED(IDC_BUTTON_REAR_INC, &CCommunicationDlg::OnBnClickedButtonRearInc)
ON_BN_CLICKED(IDC_BUTTON_REAR_DEC, &CCommunicationDlg::OnBnClickedButtonRearDec)
ON_WM_HSCROLL()
ON_BN_CLICKED(IDC_BUTTON_Lateral, &CCommunicationDlg::OnBnClickedButtonLateral)
ON_BN_CLICKED(IDC_BUTTON_InitialPosition, &CCommunicationDlg::OnBnClickedButtonInitialposition)
ON_BN_CLICKED(IDC_BUTTON_LEFTSERVOINC, &CCommunicationDlg::OnBnClickedButtonLeftservoinc)
ON_BN_CLICKED(IDC_BUTTON_LEFTSERVODEC, &CCommunicationDlg::OnBnClickedButtonLeftservodec)
ON_BN_CLICKED(IDC_BUTTON_RIGHTSERVOINC, &CCommunicationDlg::OnBnClickedButtonRightservoinc)
ON_BN_CLICKED(IDC_BUTTON_RIGHTSERVODEC, &CCommunicationDlg::OnBnClickedButtonRightservodec)
ON_EN_CHANGE(IDC_EDIT_RXDATA, &CCommunicationDlg::OnEnChangeEditRxdata)
ON_BN_CLICKED(IDC_BUTTON_FBSPEED_INC, &CCommunicationDlg::OnBnClickedButtonFbspeedInc)
ON_BN_CLICKED(IDC_BUTTON_FBSPEED_DEC, &CCommunicationDlg::OnBnClickedButtonFbspeedDec)
ON_BN_CLICKED(IDC_BUTTON_FBDIRECTION_INC, &CCommunicationDlg::OnBnClickedButtonFbdirectionInc)
ON_BN_CLICKED(IDC_BUTTON_FBDIRECTIONDEC, &CCommunicationDlg::OnBnClickedButtonFbdirectiondec)
ON_BN_CLICKED(IDC_BUTTON_LFSPEED_INC, &CCommunicationDlg::OnBnClickedButtonLfspeedInc)
ON_BN_CLICKED(IDC_BUTTON_LFSPEED_DES, &CCommunicationDlg::OnBnClickedButtonLfspeedDes)
ON_BN_CLICKED(IDC_BUTTON_LFDIRECTION_INC, &CCommunicationDlg::OnBnClickedButtonLfdirectionInc)
ON_BN_CLICKED(IDC_BUTTON_LFDIRECTION_DEC, &CCommunicationDlg::OnBnClickedButtonLfdirectionDec)
ON_BN_CLICKED(IDC_BUTTON_CW_ANTICW_SPEED_INC, &CCommunicationDlg::OnBnClickedButtonCwAnticwSpeedInc)
ON_BN_CLICKED(IDC_BUTTON_CW_ANTICW_SPEEDDEC, &CCommunicationDlg::OnBnClickedButtonCwAnticwSpeeddec)
ON_BN_CLICKED(IDC_BUTTON_CW_ANTICW_DIRECTION_INC, &CCommunicationDlg::OnBnClickedButtonCwAnticwDirectionInc)
ON_BN_CLICKED(IDC_BUTTON_CW_ANTICW_DIRECTION_DEC, &CCommunicationDlg::OnBnClickedButtonCwAnticwDirectionDec)
ON_BN_CLICKED(IDC_BUTTON_FBSPEEDSET, &CCommunicationDlg::OnBnClickedButtonFbspeedset)
ON_BN_CLICKED(IDC_BUTTON_LRSPEEDDIRECTION_SET, &CCommunicationDlg::OnBnClickedButtonLrspeeddirectionSet)
ON_BN_CLICKED(IDC_BUTTON_SPANNINGSPEEDSET, &CCommunicationDlg::OnBnClickedButtonSpanningspeedset)
ON_BN_CLICKED(IDC_BUTTON_LOGSTART, &CCommunicationDlg::OnBnClickedButtonLogstart)
ON_BN_CLICKED(IDC_BUTTON_STOPRECORDING, &CCommunicationDlg::OnBnClickedButtonStoprecording)
END_MESSAGE_MAP()


// CCommunicationDlg ��Ϣ�������

BOOL CCommunicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: ��ʼ���ٶȻ�����
	m_slider_speed.SetRange(0, 15);//���û�����Χ

	m_slider_speed.SetTicFreq(1);//ÿ1����λ��һ�̶�

	//������ʾm_xxxx_combo.SetCurSel(x)����,COMBOX�ؼ�;
	m_port_combo.SetCurSel(2);      //��ʼ���˿���ʾ��COM3��
	m_baud_combo.SetCurSel(4);      //��ʼ����������ʾ��4800��
	m_jiaoyan_combo.SetCurSel(1);   //��ʼ��У��λ��ʾ���ޡ�
	m_databit_combo.SetCurSel(1);   //��ʼ������λ��ʾ��8λ��
	m_stopbit_combo.SetCurSel(0);   //��ʼ��ֹͣλ��ʾ��1λ��

	m_speed_combo.SetCurSel(1);     //��ʼ��ͨ��������ʾ��1200bps��
	m_zaibo_combo.SetCurSel(0);     //��ʼ���ز�Ƶ����ʾ��50KHz��





	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCommunicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCommunicationDlg::OnPaint()
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
HCURSOR CCommunicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/**********************************************************/
/***********************�ٶ�����***********************/
/**********************************************************/


void CCommunicationDlg::OnBnClickedSetButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CByteArray array; //����һ���ֽڼ� 
	char *p = &UART_Command[0];
	nIndex = m_speed_combo.GetCurSel(); 
	switch(nIndex)
	{	
		case 0:
			ROBOTSPEED = 0x00;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for(int i=7;i<19;i++)
			UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			
			array.RemoveAll(); //�������
		
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤�� 
											   //����������д�뵽��ʱ���� 
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			
			break;
		case 1:
			ROBOTSPEED = 0x01;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;
 
			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤�� 

											   //����������д�뵽��ʱ���� 
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
		
			break;
		case 2:
			ROBOTSPEED = 10;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 3:
			ROBOTSPEED = 11;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 4:
			ROBOTSPEED = 12;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 5:
			ROBOTSPEED = 13;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 6:
			ROBOTSPEED = 14;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 7:
			ROBOTSPEED = 15;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 8:
			ROBOTSPEED = 0x02;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 9:
			ROBOTSPEED = 0x03;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 10:
			ROBOTSPEED = 0x04;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 11:
			ROBOTSPEED = 0x05;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 12:
			ROBOTSPEED = 0x06;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 13:
			ROBOTSPEED = 0x07;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 14:
			ROBOTSPEED = 0x08;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		case 15:
			ROBOTSPEED = 0x09;
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			array.RemoveAll(); //�������
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤��  
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
			break;
		default:
			break;
	}

}





void CCommunicationDlg::OnBnClickedClearButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	UpdateData(TRUE); //�ӱ༭���ȡ��ֵ
	//m_receive_edit.Empty(); //���ձ༭�����
	data = m_receive_edit.GetString();
	//m_mscomm.put_Output(COleVariant(_TEXT("T")));
	m_mscomm.put_Output(COleVariant(data));
	//m_mscomm.put_Output(COleVariant(_TEXT("TTTT")));
}


/**********************************************************/
/************************���ڳ�ʼ��************************/
/**********************************************************/


void CCommunicationDlg::OnBnClickedOpenButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//m_open_button.SetState(1);   //�򿪴��ڰ�ť������ʾ
	//m_close_button.SetState(0);  //�رմ��ڰ�ť��������ʾ


	//��ӶԴ��ڳ�ʼ�������
	if(m_mscomm.get_PortOpen())  //������ִ��ڱ����Ǵ򿪵ģ���رմ���
	{
		m_mscomm.put_PortOpen(FALSE);	
	}

	m_mscomm.put_InputMode(1); //���뷽ʽΪ������
	m_mscomm.put_InBufferSize(1024); //�������뻺����
	m_mscomm.put_OutBufferSize(512); //�������������

	nIndex = m_port_combo.GetCurSel(); //m_port_combo���б�����ΪCOM1,COM2,COM3
	                                       //COM4,COM5.��ȡ�����ݱ������Ϊ0��1��2��
	                                       //3��4.
	switch(nIndex)
	{
		case 0:
			m_mscomm.put_CommPort(1); //ѡ��COM1�˿�
		    break;
		case 1:
			m_mscomm.put_CommPort(2); //ѡ��COM2�˿�
			break;
		case 2:
			m_mscomm.put_CommPort(3); //ѡ��COM3�˿�
			break;
		case 3:
			m_mscomm.put_CommPort(4); //ѡ��COM4�˿�
			break;
		case 4:
			m_mscomm.put_CommPort(5); //ѡ��COM5�˿�
			break;
		default:
			break;
	}

	nIndex = m_baud_combo.GetCurSel(); //m_baud_combo���б�����Ϊ115200,19200,9600
	                                       //��ȡ�����ݱ������Ϊ0��1��2.

	switch(nIndex)
	{
		case 0:
			m_mscomm.put_Settings(TEXT("115200,n,8,1")); //������115200����У�飬8������λ��1��ֹͣλ
			m_mscomm.put_Settings(_T("115200,n,8,1"));
			break;
		case 1:
			m_mscomm.put_Settings(TEXT("19200,n,8,1")); //������19200����У�飬8������λ��1��ֹͣλ
			m_mscomm.put_Settings(_T("19200,n,8,1"));
			break;
		case 2:
			m_mscomm.put_Settings(TEXT("2400,n,8,1")); //������2400����У�飬8������λ��1��ֹͣλ
			m_mscomm.put_Settings(_T("2400,n,8,1"));
			break;
		case 3:
			m_mscomm.put_Settings(TEXT("4800,n,8,1")); //������4800����У�飬8������λ��1��ֹͣλ
			m_mscomm.put_Settings(_T("4800,n,8,1"));

			break;
		case 4:
			m_mscomm.put_Settings(TEXT("57600,n,8,1")); //������57600����У�飬8������λ��1��ֹͣλ
			m_mscomm.put_Settings(_T("57600,n,8,1"));
			break;
		case 5:
			m_mscomm.put_Settings(TEXT("9600,n,8,1")); //������9600����У�飬8������λ��1��ֹͣλ
			m_mscomm.put_Settings(_T("9600,n,8,1"));
			break;
		default:
			break;

	}

	if(!m_mscomm.get_PortOpen())
	{
		m_mscomm.put_PortOpen(TRUE); //�򿪴���
		m_mscomm.put_RThreshold(1);  //ÿ�����ջ�������1���ַ�����մ�������
		m_mscomm.put_InputLen(0);   //���õ�ǰ����������Ϊ0
		m_mscomm.get_Input();       //Ԥ���������������������

		char string[20];
		itoa(FBSpeed, string, 10);
		m_forwardbackspeed =*string;
		UpdateData(FALSE);

	    m_forwardbackdirection= *string;
		UpdateData(FALSE);

		m_LeftRightSpeed = *string;
		UpdateData(FALSE);

		m_LeftRightDirection = *string;
		UpdateData(FALSE);

		m_CWCounterCWSpeed = *string;
		UpdateData(FALSE);

		m_CWCounterCWDirection = *string;
		UpdateData(FALSE);



	}
	else
	{
		AfxMessageBox(TEXT("�򿪶˿�ʧ��!"),MB_ICONSTOP,0);
		AfxMessageBox(_T("�򿪶˿�ʧ��!"),MB_ICONSTOP,0);
	}

}
BEGIN_EVENTSINK_MAP(CCommunicationDlg, CDialogEx)
	ON_EVENT(CCommunicationDlg, IDC_MSCOMM, 1, CCommunicationDlg::OnCommMscomm, VTS_NONE)
END_EVENTSINK_MAP()



void CCommunicationDlg::OnBnClickedCloseButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//m_open_button.SetState(0);   //�򿪴��ڰ�ť��������ʾ
	//m_close_button.SetState(1);  //�رմ��ڰ�ť������ʾ
	m_mscomm.put_PortOpen(FALSE);
}

void CCommunicationDlg::OnCommMscomm()
{
	// TODO: �ڴ˴������Ϣ����������

	//�Ӵ��ڽ������ݣ�����ʾ�ڱ༭����
	VARIANT variant_inp;
	COleSafeArray safearry_inp;
	
	long len=0, k=0;
	byte rxdata[512]; //����BYTE����
	CString strtemp;
	int temp = 0;        //��ʱ����
	if(m_mscomm.get_CommEvent()==2) //ֵΪ2��ʾ���ջ����������ַ�
	{
		variant_inp=m_mscomm.get_Input(); //����������Ϣ
		safearry_inp=variant_inp; //����ת��
		len=safearry_inp.GetOneDimSize(); //�õ���Ч�����ݳ���
		for(k=0;k<len;k++)
		{
			safearry_inp.GetElement(&k,rxdata+k);
		}
		//CFile RobotState(_T("BoatStates.txt "), CFile::modeWrite | CFile::modeCreate);
		//RobotState.Write(rxdata, 20);
		//RobotState.Flush();
		//RobotState.Close();
		//Statefile = fopen("BoatStates.txt", "wt+");
		//printf("\n\n\n A new PSO experiment...\n");
		//fprintf(Statefile, "\n\n\n A new PSO experiment...\n");
		//fprintf(Statefile, "%x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x, %x \n",
		//	rxdata[0], rxdata[1], rxdata[2], rxdata[3], rxdata[4], rxdata[5], rxdata[6], rxdata[7], rxdata[8], rxdata[9], rxdata[10], rxdata[11],
		//	rxdata[12], rxdata[13], rxdata[14], rxdata[15], rxdata[16], rxdata[17], rxdata[18], rxdata[19]);
		//fclose(Statefile);

		//Receive the UART Data from microcontroller and save it.
		//if (len = Command_Length)
		//{
		//	for (temp = 0; temp < len; temp++)
		//	{
		//		Comm_DataBuffer[temp] = rxdata[temp];
		//	}
		//	if (Comm_DataBuffer[0] == STARTBYTE&&Comm_DataBuffer[19] == ENDBYTE&&Comm_DataBuffer[1] == ROBOTID &&Comm_DataBuffer[2] == 0x32)
		//	{
		//		for (ReceiveStateCount = 0; ReceiveStateCount < 4; ReceiveStateCount++)
		//		{
		//			temp4byte[3] = Comm_DataBuffer[ReceiveStateCount * 4 + 3];
		//			temp4byte[2] = Comm_DataBuffer[ReceiveStateCount * 4 + 4];
		//			temp4byte[1] = Comm_DataBuffer[ReceiveStateCount * 4 + 5];
		//			temp4byte[0] = Comm_DataBuffer[ReceiveStateCount * 4 + 6];
		//			StatesData[ReceiveStateCount] = Receive_byte_to_float(temp4byte);
		//			Statefile = fopen("BoatStates.txt", "a");
		//			fprintf(Statefile, "%f, %f, %f, %f \n", Comm_DataBuffer[0], Comm_DataBuffer[1], Comm_DataBuffer[2], Comm_DataBuffer[3]);
		//			fclose(Statefile);
		//		}
		//	}
		//}

//Visulize on the window.
		for(k=0;k<len;k++) //������ת��ΪCString�ͱ���
		{
			char bt=*(char*)(rxdata+k); //�ַ���
			strtemp.Format(TEXT("%c"),bt); //���ַ�������ʱ����strtemp���
			strtemp.Format(_T("%c"),bt);
			m_strRXData +=strtemp; //������ձ༭���Ӧ�ַ���
		}
	}
	//CString temp=(TEXT("\n")); //��ʾ��ɺ�Ҫ�Զ�����
	//m_strRXData =temp;
	UpdateData(FALSE); //���±༭������
}


/**********************************************************/
/***********************�˶�ģ̬����***********************/
/**********************************************************/

void CCommunicationDlg::OnBnClickedForwardButton()  //ֱ��ģ̬ Z
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_forward_button.SetState(1);
	//m_backward_button.SetState(0);
	//m_left_button.SetState(0);
	//m_right_button.SetState(0);
	//m_rise_button.SetState(0);
	//m_down_button.SetState(0);
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�

	COMMANDMODE = 0x04;
	ROBOTSPEED = 0x01;
	DATAAREA[0] = ROBOTSPEED;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

							//����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}

void CCommunicationDlg::OnBnClickedBackwardButton()  //����ģ̬  B
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_forward_button.SetState(0);
	//m_backward_button.SetState(1);
	//m_left_button.SetState(0);
	//m_right_button.SetState(0);
	//m_rise_button.SetState(0);
	//m_down_button.SetState(0);
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�

	COMMANDMODE = 0x05;
	ROBOTSPEED = 0x01;
	DATAAREA[0] = ROBOTSPEED;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}

void CCommunicationDlg::OnBnClickedLeftButton()    //����ģ̬  L
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_forward_button.SetState(0);
	//m_backward_button.SetState(0);
	//m_left_button.SetState(1);
	//m_right_button.SetState(0);
	//m_rise_button.SetState(0);
	//m_down_button.SetState(0);
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�

	COMMANDMODE = 0x06;
	ROBOTSPEED = 0x01;
	DATAAREA[0] = ROBOTSPEED;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedRightButton()   //����ģ̬  R
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_forward_button.SetState(0);
	//m_backward_button.SetState(0);
	//m_left_button.SetState(0);
	//m_right_button.SetState(1);
	//m_rise_button.SetState(0);
	//m_down_button.SetState(0);
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�

	COMMANDMODE = 0x07;
	ROBOTSPEED = 0x01;
	DATAAREA[0] = ROBOTSPEED;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedRiseButton()   //ԭ��˳ʱ��ת��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_forward_button.SetState(0);
	//m_backward_button.SetState(0);
	//m_left_button.SetState(0);
	//m_right_button.SetState(0);
	//m_rise_button.SetState(1);
	//m_down_button.SetState(0);
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�

	COMMANDMODE = 0x08;
	ROBOTSPEED = 0x01;
	DATAAREA[0] = ROBOTSPEED;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedDownButton()   //ԭ����ʱ��ת��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_forward_button.SetState(0);
	//m_backward_button.SetState(0);
	//m_left_button.SetState(0);
	//m_right_button.SetState(0);
	//m_rise_button.SetState(0);
	//m_down_button.SetState(1);
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�
	//MFC ���ڷ���Ĭ�����ַ�����ʽ�����ﺯ�����������ô��ڷ���ʮ���������ݡ�

	COMMANDMODE = 0x09;
	ROBOTSPEED = 0x01;
	DATAAREA[0] = ROBOTSPEED;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}



void CCommunicationDlg::OnBnClickedButtonTurnleftcircle()//��������Բ������ʽ��ת��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 11;
	ROBOTTURNSPEED = 0x03;
	DATAAREA[0] = ROBOTTURNSPEED;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonTurnrightcircle()//��������Բ������ʽ��ת��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 10;
	ROBOTTURNSPEED = 0x03;
	DATAAREA[0] = ROBOTTURNSPEED;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x00; //stop the robot


	FBSpeed = 0;
	FBDirection = 0;
	LRSpeed=0; //left right speed parameter
	LRDirection=0; //Left right direction parameter
	SpanningSpeed=0;
	SpanningDirection=0;


	char string[20];
	itoa(FBDirection, string, 10);
	m_forwardbackspeed = CString(string, 20);
	m_forwardbackdirection = CString(string, 20);
	m_LeftRightSpeed = CString(string, 20);
	m_LeftRightDirection = CString(string, 20);
	m_CWCounterCWSpeed = CString(string, 20);
	m_CWCounterCWDirection = CString(string, 20);
	UpdateData(FALSE);

	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}

/**********************************************************/
/***********************�����˷�������***********************/
/**********************************************************/
void CCommunicationDlg::OnBnClickedButtonDirectionSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CByteArray array; //����һ���ֽڼ� 
	char *p = &UART_Command[0];
	nIndex = m_zaibo_combo.GetCurSel();
	switch (nIndex)
	{
	case 0:
		ROBOTTURNSPEED = 0x00;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;


		array.RemoveAll(); //�������

		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤�� 
										   //����������д�뵽��ʱ���� 
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}

		break;
	case 1:
		ROBOTTURNSPEED = 0x01;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤�� 

										   //����������д�뵽��ʱ���� 
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}

		break;
	case 2:
		ROBOTTURNSPEED = 10;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 3:
		ROBOTTURNSPEED = 11;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 4:
		ROBOTTURNSPEED = 12;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 5:
		ROBOTTURNSPEED = 13;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 6:
		ROBOTTURNSPEED = 14;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 7:
		ROBOTTURNSPEED = 15;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 8:
		ROBOTTURNSPEED = 0x02;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 9:
		ROBOTTURNSPEED = 0x03;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 10:
		ROBOTTURNSPEED = 0x04;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 11:
		ROBOTTURNSPEED = 0x05;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 12:
		ROBOTTURNSPEED = 0x06;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 13:
		ROBOTTURNSPEED = 0x07;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 14:
		ROBOTTURNSPEED = 0x08;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	case 15:
		ROBOTTURNSPEED = 0x09;
		DATAAREA[0] = ROBOTTURNSPEED;
		UART_Command[0] = STARTBYTE;
		UART_Command[1] = ROBOTID;
		UART_Command[2] = COMMANDMODE;
		UART_Command[3] = DATAAREA[0];
		UART_Command[4] = DATAAREA[1];
		UART_Command[5] = DATAAREA[2];
		UART_Command[6] = DATAAREA[3];
		for (int i = 7; i<19; i++)
			UART_Command[i] = 0x00;
		UART_Command[19] = ENDBYTE;

		array.RemoveAll(); //�������
		if (0 != Command_Length)
		{
			array.SetSize(Command_Length); //�趨���鳤��  
			for (UINT i = 0; i < Command_Length; i++)
			{
				array.SetAt(i, *p++);
			}
			//����mscomm��������
			m_mscomm.put_Output(COleVariant(array));
		}
		break;
	default:
		break;
	}

}


void CCommunicationDlg::OnBnClickedButtonLeftInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 12;
	DATAAREA[0] = 0x00;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}

}


void CCommunicationDlg::OnBnClickedButtonLeftDec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 12;
	DATAAREA[0] = 0x01;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}

}


void CCommunicationDlg::OnBnClickedButtonTightInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 13;
	DATAAREA[0] = 0x00;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}

}


void CCommunicationDlg::OnBnClickedButtonRightDec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 13;
	DATAAREA[0] = 0x01;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}

}


void CCommunicationDlg::OnBnClickedButtonFrontInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 14;
	DATAAREA[0] = 0x00;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}

}


void CCommunicationDlg::OnBnClickedButtonFrontDec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 14;
	DATAAREA[0] = 0x01;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}

}


void CCommunicationDlg::OnBnClickedButtonRearInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 15;
	DATAAREA[0] = 0x00;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}

}


void CCommunicationDlg::OnBnClickedButtonRearDec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 15;
	DATAAREA[0] = 0x01;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}

}


void CCommunicationDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Robot SPeed Slider Control

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	CSliderCtrl *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SPEED);
	ROBOTSPEED = pSlidCtrl->GetPos();//ȡ�õ�ǰλ��ֵ

	if (COMMANDMODE >3&& COMMANDMODE <10)
	{
			DATAAREA[0] = ROBOTSPEED;
			UART_Command[0] = STARTBYTE;
			UART_Command[1] = ROBOTID;
			UART_Command[2] = COMMANDMODE;
			UART_Command[3] = DATAAREA[0];
			UART_Command[4] = DATAAREA[1];
			UART_Command[5] = DATAAREA[2];
			UART_Command[6] = DATAAREA[3];
			for (int i = 7; i<19; i++)
				UART_Command[i] = 0x00;
			UART_Command[19] = ENDBYTE;

			CByteArray array; //����һ���ֽڼ� 
			array.RemoveAll(); //�������
			char *p = &UART_Command[0];
			if (0 != Command_Length)
			{
				array.SetSize(Command_Length); //�趨���鳤�� 

											   //����������д�뵽��ʱ���� 
				for (UINT i = 0; i < Command_Length; i++)
				{
					array.SetAt(i, *p++);
				}
				//����mscomm��������
				m_mscomm.put_Output(COleVariant(array));
			}
	}
}


void CCommunicationDlg::OnBnClickedButtonLateral()
{
	// TODO: ServoMotor Turns to the Lateral Position
	COMMANDMODE = 02;
	DATAAREA[0] = 0x00;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonInitialposition()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 2;
	DATAAREA[0] = 0x01;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonLeftservoinc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 2;
	DATAAREA[0] = 0x02;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonLeftservodec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 02;
	DATAAREA[0] = 0x03;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonRightservoinc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 02;
	DATAAREA[0] = 0x04;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonRightservodec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 02;
	DATAAREA[0] = 0x05;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnEnChangeEditRxdata()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	//this->SendDlgItemMessage(IDC_EDIT_RXDATA, WM_VSCROLL, SB_BOTTOM, 0);
	//GetDlgItem(IDC_EDIT_RXDATA)->PostMessage(WS_VSCROLL, SB_BOTTOM);
	//CEdit* pedit = (CEdit*)GetDlgItem(IDC_EDIT_RXDATA);
	//pedit->LineScroll(pedit->GetLineCount());// ����ôʵ�ֵģ�ֱ�ӣ�ʵ��
	//m_EditView.SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
	//PostMessage(WM_VSCROLL, SB_BOTTOM, 0); //����ȽϺ��ã�
	//m_EditView.ReplaceSel(sNewString+)
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_EditView.LineScroll(m_EditView.GetLineCount(), 0);
	m_EditView.SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
	//m_EditView.SetSel(-1, -1); //�Զ�����
	UpdateData(true);
}


void CCommunicationDlg::OnBnClickedButtonFbspeedInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	COMMANDMODE = 0x12;
	FBSpeed = FBSpeed + 1;
	char string[20];
	itoa(FBSpeed, string, 10);
	m_forwardbackspeed= CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = FBSpeed;
	DATAAREA[1] = FBDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonFbspeedDec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x12;
	FBSpeed = FBSpeed - 1;
	char string[20];
	itoa(FBSpeed, string, 10);
	m_forwardbackspeed = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = FBSpeed;
	DATAAREA[1] = FBDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonFbdirectionInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x13;
	FBDirection = FBDirection + 1;
	char string[20];
	itoa(FBDirection, string, 10);
	m_forwardbackdirection = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = FBSpeed;
	DATAAREA[1] = FBDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonFbdirectiondec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x13;
	FBDirection = FBDirection - 1;
	char string[20];
	itoa(FBDirection, string, 10);
	m_forwardbackdirection = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = FBSpeed;
	DATAAREA[1] = FBDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonLfspeedInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x14;
	LRSpeed = LRSpeed + 1;
	char string[20];
	itoa(LRSpeed, string, 10);
	m_LeftRightSpeed = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = LRSpeed;
	DATAAREA[1] = LRDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonLfspeedDes()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x14;
	LRSpeed = LRSpeed - 1;
	char string[20];
	itoa(LRSpeed, string, 10);
	m_LeftRightSpeed = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = LRSpeed;
	DATAAREA[1] = LRDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonLfdirectionInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x15;
	LRDirection = LRDirection +1;
	char string[20];
	itoa(LRDirection, string, 10);
	m_LeftRightDirection = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = LRSpeed;
	DATAAREA[1] = LRDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonLfdirectionDec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x15;
	LRDirection = LRDirection - 1;
	char string[20];
	itoa(LRDirection, string, 10);
	m_LeftRightDirection = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = LRSpeed;
	DATAAREA[1] = LRDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonCwAnticwSpeedInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x16;
	SpanningSpeed = SpanningSpeed + 1;
	char string[20];
	itoa(SpanningSpeed, string, 10);
	m_CWCounterCWSpeed = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = SpanningSpeed;
	DATAAREA[1] = SpanningDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonCwAnticwSpeeddec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x16;
	SpanningSpeed = SpanningSpeed - 1;
	char string[20];
	itoa(SpanningSpeed, string, 10);
	m_CWCounterCWSpeed = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = SpanningSpeed;
	DATAAREA[1] = SpanningDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonCwAnticwDirectionInc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x17;
	SpanningDirection = SpanningDirection + 1;
	char string[20];
	itoa(SpanningDirection, string, 10);
	m_CWCounterCWDirection = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = SpanningSpeed;
	DATAAREA[1] = SpanningDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonCwAnticwDirectionDec()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COMMANDMODE = 0x17;
	SpanningDirection = SpanningDirection - 1;
	char string[20];
	itoa(SpanningDirection, string, 10);
	m_CWCounterCWDirection = CString(string, 20);
	UpdateData(FALSE);

	DATAAREA[0] = SpanningSpeed;
	DATAAREA[1] = SpanningDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonFbspeedset()
{
	// TODO: Send the set speed and direction to the robot
	UpdateData(TRUE);
	CString   buf1 = m_forwardbackspeed;
	char string1[20];
	strcpy_s(string1, CStringA(buf1).GetString());
	FBSpeed=atoi(string1);
    
	CString   buf2 = m_forwardbackdirection;
	char string2[20];
	strcpy_s(string2, CStringA(buf2).GetString());
	FBDirection = atoi(string2);


	COMMANDMODE = 0x18;
	DATAAREA[0] = FBSpeed;
	DATAAREA[1] = FBDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonLrspeeddirectionSet()
{
	// TODO: Send the set speed and direction to the robot
	UpdateData(TRUE);
	CString   buf1 = m_LeftRightSpeed;
	char string1[20];
	strcpy_s(string1, CStringA(buf1).GetString());
	LRSpeed = atoi(string1);

	CString   buf2 = m_LeftRightDirection;
	char string2[20];
	strcpy_s(string2, CStringA(buf2).GetString());
	LRDirection = atoi(string2);

	COMMANDMODE = 0x19;
	DATAAREA[0] = LRSpeed;
	DATAAREA[1] = LRDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonSpanningspeedset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO: Send the set speed and direction to the robot
	UpdateData(TRUE);
	CString   buf1 = m_CWCounterCWSpeed;
	char string1[20];
	strcpy_s(string1, CStringA(buf1).GetString());
	SpanningSpeed = atoi(string1);

	CString   buf2 = m_CWCounterCWDirection;
	char string2[20];
	strcpy_s(string2, CStringA(buf2).GetString());
	SpanningDirection = atoi(string2);


	COMMANDMODE = 0x1a;
	DATAAREA[0] = SpanningSpeed;
	DATAAREA[1] = SpanningDirection;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonLogstart()
{
	// TODO: Start Data Recording
	COMMANDMODE = 0x1b;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}


void CCommunicationDlg::OnBnClickedButtonStoprecording()
{
	// TODO: Stop Data Recording
	COMMANDMODE = 0x1c;
	UART_Command[0] = STARTBYTE;
	UART_Command[1] = ROBOTID;
	UART_Command[2] = COMMANDMODE;
	UART_Command[3] = DATAAREA[0];
	UART_Command[4] = DATAAREA[1];
	UART_Command[5] = DATAAREA[2];
	UART_Command[6] = DATAAREA[3];
	for (int i = 7; i<19; i++)
		UART_Command[i] = 0x00;
	UART_Command[19] = ENDBYTE;

	CByteArray array; //����һ���ֽڼ� 
	array.RemoveAll(); //�������
	char *p = &UART_Command[0];
	if (0 != Command_Length)
	{
		array.SetSize(Command_Length); //�趨���鳤�� 

									   //����������д�뵽��ʱ���� 
		for (UINT i = 0; i < Command_Length; i++)
		{
			array.SetAt(i, *p++);
		}
		//����mscomm��������
		m_mscomm.put_Output(COleVariant(array));
	}
}
