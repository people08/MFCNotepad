#include "pch.h"
#include "MFCNotepad.h"
#include "ChildFrm.h"
#include "Resource.h"

IMPLEMENT_DYNAMIC(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
END_MESSAGE_MAP()

CChildFrame::CChildFrame() {}

CChildFrame::~CChildFrame() {}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CMDIChildWndEx::PreCreateWindow(cs))
        return FALSE;
    return TRUE;
}