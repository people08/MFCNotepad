#pragma once

class CMainFrame : public CMDIFrameWndEx
{
    DECLARE_DYNAMIC(CMainFrame)
public:
    CMainFrame();
    virtual ~CMainFrame();

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
    CMFCMenuBar   m_wndMenuBar;
    CMFCToolBar   m_wndToolBar;
    CMFCStatusBar m_wndStatusBar;

    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};