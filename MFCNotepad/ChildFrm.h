#pragma once

class CChildFrame : public CMDIChildWndEx
{
    DECLARE_DYNCREATE(CChildFrame)
public:
    CChildFrame();
    virtual ~CChildFrame();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    DECLARE_MESSAGE_MAP()
};