#include "pch.h"
#include "MFCNotepad.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "MFCNotepadDoc.h"
#include "MFCNotepadView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMFCNotepadApp theApp;

BEGIN_MESSAGE_MAP(CMFCNotepadApp, CWinAppEx)
    ON_COMMAND(ID_APP_ABOUT, &CMFCNotepadApp::OnAppAbout)
    ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

CMFCNotepadApp::CMFCNotepadApp()
{
    m_theme = THEME_LIGHT;
}

ThemeColors CMFCNotepadApp::GetThemeColors()
{
    ThemeColors tc;
    if (m_theme == THEME_LIGHT) {
        tc.bgColor = RGB(255, 255, 255);
        tc.textColor = RGB(0, 0, 0);
        tc.lineNumBg = RGB(240, 240, 240);
        tc.lineNumText = RGB(128, 128, 128);
    }
    else {
        tc.bgColor = RGB(30, 30, 30);
        tc.textColor = RGB(212, 212, 212);
        tc.lineNumBg = RGB(40, 40, 40);
        tc.lineNumText = RGB(133, 133, 133);
    }
    return tc;
}

void CMFCNotepadApp::SetTheme(Theme t)
{
    m_theme = t;
    // 通知所有视图更新
    if (m_pMainWnd) {
        m_pMainWnd->SendMessageToDescendants(WM_THEMECHANGED, 0, 0);
        m_pMainWnd->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
    }
}

BOOL CMFCNotepadApp::InitInstance()
{
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);

    CWinAppEx::InitInstance();
    AfxOleInit();
    AfxEnableControlContainer();

    SetRegistryKey(_T("MFCNotepad"));
    LoadStdProfileSettings(8);

    // 注册文档模板
    CMultiDocTemplate* pDocTemplate = new CMultiDocTemplate(
        IDR_MFCNOTEPADTYPE,
        RUNTIME_CLASS(CMFCNotepadDoc),
        RUNTIME_CLASS(CChildFrame),
        RUNTIME_CLASS(CMFCNotepadView));
    AddDocTemplate(pDocTemplate);

    // 创建主框架
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;
    m_pMainWnd = pMainFrame;

    // 启用MDI标签页
    CMDITabInfo mdiTabParams;
    mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE;
    mdiTabParams.m_bActiveTabCloseButton = TRUE;
    mdiTabParams.m_bTabIcons = FALSE;
    mdiTabParams.m_bAutoColor = FALSE;
    mdiTabParams.m_bDocumentMenu = TRUE;
    pMainFrame->EnableMDITabbedGroups(TRUE, mdiTabParams);

    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();

    // 创建初始文档
    OnFileNew();

    return TRUE;
}

int CMFCNotepadApp::ExitInstance()
{
    return CWinAppEx::ExitInstance();
}

// 关于对话框
class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}
};

void CMFCNotepadApp::OnAppAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}