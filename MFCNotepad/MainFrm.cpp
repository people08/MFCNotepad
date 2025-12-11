#include "pch.h"
#include "MFCNotepad.h"
#include "MainFrm.h"
#include "Resource.h"

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
    ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] = {
    ID_SEPARATOR,
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
};

CMainFrame::CMainFrame() {}
CMainFrame::~CMainFrame() {}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CMDIFrameWndEx::PreCreateWindow(cs))
        return FALSE;
    return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    // 菜单栏
    if (!m_wndMenuBar.Create(this)) {
        TRACE0("未能创建菜单栏\n");
        return -1;
    }
    m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() |
        CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

    // 工具栏
    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,
        WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
        TRACE0("未能创建工具栏\n");
        return -1;
    }

    // 状态栏
    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT))) {
        TRACE0("未能创建状态栏\n");
        return -1;
    }

    // 允许停靠
    m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndMenuBar);
    DockPane(&m_wndToolBar);

    // 设置视觉样式
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

    return 0;
}