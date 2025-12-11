#include "pch.h"
#include "MFCNotepad.h"
#include "MFCNotepadDoc.h"
#include "MFCNotepadView.h"
#include "Resource.h"
#include <regex>

#define IDC_EDIT 1001
#define WM_THEMECHANGED (WM_USER + 100)

// ========== 查找替换对话框 ==========
class CFindReplaceDlgEx : public CDialogEx
{
public:
    CMFCNotepadView* m_pView;
    CFindReplaceDlgEx(CMFCNotepadView* pView, BOOL bReplace)
        : CDialogEx(IDD_FIND_REPLACE), m_pView(pView), m_bReplace(bReplace) {
    }
    BOOL m_bReplace;

protected:
    CEdit m_editFind;
    CEdit m_editReplace;
    CButton m_chkRegex;
    CButton m_chkCase;

    virtual BOOL OnInitDialog() {
        CDialogEx::OnInitDialog();
        m_editFind.SubclassDlgItem(IDC_EDIT_FIND, this);
        m_editReplace.SubclassDlgItem(IDC_EDIT_REPLACE, this);
        m_chkRegex.SubclassDlgItem(IDC_CHECK_REGEX, this);
        m_chkCase.SubclassDlgItem(IDC_CHECK_MATCHCASE, this);
        m_editFind.SetWindowText(m_pView->m_findOpt.findText);
        m_editReplace.SetWindowText(m_pView->m_findOpt.replaceText);
        m_chkRegex.SetCheck(m_pView->m_findOpt.useRegex);
        m_chkCase.SetCheck(m_pView->m_findOpt.matchCase);
        SetWindowText(m_bReplace ? _T("查找和替换") : _T("查找"));
        if (!m_bReplace) {
            GetDlgItem(IDC_EDIT_REPLACE)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_BTN_REPLACE)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_BTN_REPLACEALL)->ShowWindow(SW_HIDE);
        }
        return TRUE;
    }

    void UpdateOptions() {
        m_editFind.GetWindowText(m_pView->m_findOpt.findText);
        m_editReplace.GetWindowText(m_pView->m_findOpt.replaceText);
        m_pView->m_findOpt.useRegex = m_chkRegex.GetCheck();
        m_pView->m_findOpt.matchCase = m_chkCase.GetCheck();
    }

    DECLARE_MESSAGE_MAP()
    afx_msg void OnFindNext() {
        UpdateOptions();
        m_pView->DoFind(TRUE);
    }
    afx_msg void OnReplace() {
        UpdateOptions();
        int start, end;
        m_pView->GetEditCtrl().GetSel(start, end);
        if (start != end) {
            m_pView->GetEditCtrl().ReplaceSel(m_pView->m_findOpt.replaceText);
        }
        m_pView->DoFind(TRUE);
    }
    afx_msg void OnReplaceAll() {
        UpdateOptions();
        int count = m_pView->DoReplaceAll();
        CString msg;
        msg.Format(_T("已替换 %d 处"), count);
        SetDlgItemText(IDC_STATIC_STATUS, msg);
    }
};

BEGIN_MESSAGE_MAP(CFindReplaceDlgEx, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_FINDNEXT, &CFindReplaceDlgEx::OnFindNext)
    ON_BN_CLICKED(IDC_BTN_REPLACE, &CFindReplaceDlgEx::OnReplace)
    ON_BN_CLICKED(IDC_BTN_REPLACEALL, &CFindReplaceDlgEx::OnReplaceAll)
END_MESSAGE_MAP()

// ========== 视图类实现 ==========
IMPLEMENT_DYNCREATE(CMFCNotepadView, CView)

BEGIN_MESSAGE_MAP(CMFCNotepadView, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_CTLCOLOR()
    ON_MESSAGE(WM_THEMECHANGED, &CMFCNotepadView::OnThemeChanged)

    // ========== 标准编辑命令（新增）==========
    ON_COMMAND(ID_EDIT_COPY, &CMFCNotepadView::OnEditCopy)
    ON_COMMAND(ID_EDIT_CUT, &CMFCNotepadView::OnEditCut)
    ON_COMMAND(ID_EDIT_PASTE, &CMFCNotepadView::OnEditPaste)
    ON_COMMAND(ID_EDIT_SELECT_ALL, &CMFCNotepadView::OnEditSelectAll)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CMFCNotepadView::OnUpdateEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CMFCNotepadView::OnUpdateEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CMFCNotepadView::OnUpdateEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &CMFCNotepadView::OnUpdateEditSelectAll)

    // 自定义编辑命令
    ON_COMMAND(ID_EDIT_UNDO, &CMFCNotepadView::OnEditUndo)
    ON_COMMAND(ID_EDIT_REDO, &CMFCNotepadView::OnEditRedo)
    ON_COMMAND(ID_EDIT_FIND, &CMFCNotepadView::OnEditFind)
    ON_COMMAND(ID_EDIT_REPLACE, &CMFCNotepadView::OnEditReplace)
    ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CMFCNotepadView::OnUpdateEditUndo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CMFCNotepadView::OnUpdateEditRedo)

    // 视图命令
    ON_COMMAND(ID_VIEW_LINENUMBERS, &CMFCNotepadView::OnViewLineNumbers)
    ON_COMMAND(ID_VIEW_THEME_LIGHT, &CMFCNotepadView::OnViewThemeLight)
    ON_COMMAND(ID_VIEW_THEME_DARK, &CMFCNotepadView::OnViewThemeDark)
    ON_COMMAND(ID_VIEW_FONT_INCREASE, &CMFCNotepadView::OnViewFontIncrease)
    ON_COMMAND(ID_VIEW_FONT_DECREASE, &CMFCNotepadView::OnViewFontDecrease)
    ON_COMMAND(ID_VIEW_FONT_RESET, &CMFCNotepadView::OnViewFontReset)
    ON_UPDATE_COMMAND_UI(ID_VIEW_LINENUMBERS, &CMFCNotepadView::OnUpdateViewLineNumbers)
    ON_UPDATE_COMMAND_UI(ID_VIEW_THEME_LIGHT, &CMFCNotepadView::OnUpdateViewThemeLight)
    ON_UPDATE_COMMAND_UI(ID_VIEW_THEME_DARK, &CMFCNotepadView::OnUpdateViewThemeDark)

    ON_EN_CHANGE(IDC_EDIT, &CMFCNotepadView::OnEditChange)
END_MESSAGE_MAP()

CMFCNotepadView::CMFCNotepadView()
    : m_bShowLineNum(TRUE), m_nFontSize(12), m_nLineNumWidth(50), m_bSkipUndo(FALSE)
{
    m_findOpt.useRegex = FALSE;
    m_findOpt.matchCase = FALSE;
}

CMFCNotepadView::~CMFCNotepadView() {}

CMFCNotepadDoc* CMFCNotepadView::GetDocument() const
{
    return (CMFCNotepadDoc*)m_pDocument;
}

BOOL CMFCNotepadView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

// ========== PreTranslateMessage：只拦截自定义快捷键 ==========
BOOL CMFCNotepadView::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        BOOL bCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
        BOOL bShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;

        // 只拦截自定义快捷键，不拦截标准编辑快捷键（Ctrl+C/V/X/A）
        // 标准编辑快捷键让它们通过正常的命令路由

        // Ctrl+Z - 撤销（需要拦截，因为 CEdit 有自己的撤销）
        if (bCtrl && !bShift && pMsg->wParam == 'Z')
        {
            TRACE("PreTranslateMessage: Ctrl+Z 被拦截\n");
            OnEditUndo();
            return TRUE;
        }

        // Ctrl+Y 或 Ctrl+Shift+Z - 重做
        if ((bCtrl && !bShift && pMsg->wParam == 'Y') ||
            (bCtrl && bShift && pMsg->wParam == 'Z'))
        {
            TRACE("PreTranslateMessage: Ctrl+Y/Ctrl+Shift+Z 被拦截\n");
            OnEditRedo();
            return TRUE;
        }

        // Ctrl+F - 查找
        if (bCtrl && !bShift && pMsg->wParam == 'F')
        {
            OnEditFind();
            return TRUE;
        }

        // Ctrl+H - 替换
        if (bCtrl && !bShift && pMsg->wParam == 'H')
        {
            OnEditReplace();
            return TRUE;
        }

        // 不拦截其他快捷键（Ctrl+C/V/X/A 等），让它们正常处理
    }

    return CView::PreTranslateMessage(pMsg);
}

int CMFCNotepadView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rect;
    GetClientRect(&rect);

    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL |
        ES_WANTRETURN | ES_NOHIDESEL;

    if (!m_wndEdit.Create(dwStyle, rect, this, IDC_EDIT))
    {
        TRACE("创建编辑控件失败\n");
        return -1;
    }

    m_wndEdit.SetLimitText(0);
    TRACE("编辑控件创建成功，ID=%d\n", IDC_EDIT);

    UpdateFont();
    UpdateColors();
    return 0;
}

void CMFCNotepadView::OnInitialUpdate()
{
    CView::OnInitialUpdate();
    SyncFromDoc();
}

void CMFCNotepadView::UpdateFont()
{
    m_font.DeleteObject();
    m_font.CreatePointFont(m_nFontSize * 10, _T("Consolas"));
    m_wndEdit.SetFont(&m_font);
    m_nLineNumWidth = CalcLineNumWidth();
    UpdateLayout();
}

void CMFCNotepadView::UpdateColors()
{
    ThemeColors tc = theApp.GetThemeColors();
    m_brushBg.DeleteObject();
    m_brushBg.CreateSolidBrush(tc.bgColor);
    if (m_wndEdit.GetSafeHwnd()) m_wndEdit.Invalidate();
    Invalidate();
}

void CMFCNotepadView::UpdateLayout()
{
    if (!m_wndEdit.GetSafeHwnd()) return;
    CRect rect;
    GetClientRect(&rect);
    int left = m_bShowLineNum ? m_nLineNumWidth : 0;
    m_wndEdit.MoveWindow(left, 0, rect.Width() - left, rect.Height());
    Invalidate();
}

int CMFCNotepadView::CalcLineNumWidth()
{
    int lines = m_wndEdit.GetLineCount();
    CString str;
    str.Format(_T("%d"), max(lines, 100));
    CClientDC dc(this);
    dc.SelectObject(&m_font);
    return dc.GetTextExtent(str).cx + 20;
}

void CMFCNotepadView::SyncToDoc()
{
    CMFCNotepadDoc* pDoc = GetDocument();
    if (pDoc) {
        CString text;
        m_wndEdit.GetWindowText(text);
        pDoc->m_strContent = text;
    }
}

void CMFCNotepadView::SyncFromDoc()
{
    CMFCNotepadDoc* pDoc = GetDocument();
    if (pDoc) {
        m_bSkipUndo = TRUE;
        m_wndEdit.SetWindowText(pDoc->m_strContent);
        m_bSkipUndo = FALSE;
    }
}

void CMFCNotepadView::OnDraw(CDC* pDC)
{
    if (!m_bShowLineNum) return;

    ThemeColors tc = theApp.GetThemeColors();
    CRect rect;
    GetClientRect(&rect);
    rect.right = m_nLineNumWidth;

    pDC->FillSolidRect(&rect, tc.lineNumBg);
    pDC->FillSolidRect(m_nLineNumWidth - 1, 0, 1, rect.Height(), tc.lineNumText);

    CFont* pOldFont = pDC->SelectObject(&m_font);
    pDC->SetTextColor(tc.lineNumText);
    pDC->SetBkMode(TRANSPARENT);

    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    int lineHeight = tm.tmHeight;
    int firstLine = m_wndEdit.GetFirstVisibleLine();
    int totalLines = m_wndEdit.GetLineCount();
    int visibleLines = rect.Height() / lineHeight + 2;

    for (int i = 0; i < visibleLines && (firstLine + i) < totalLines; i++) {
        CString str;
        str.Format(_T("%d"), firstLine + i + 1);
        CRect lineRect(0, i * lineHeight, m_nLineNumWidth - 5, (i + 1) * lineHeight);
        pDC->DrawText(str, &lineRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    }
    pDC->SelectObject(pOldFont);
}

void CMFCNotepadView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    UpdateLayout();
}

void CMFCNotepadView::OnSetFocus(CWnd* pOldWnd)
{
    CView::OnSetFocus(pOldWnd);
    if (m_wndEdit.GetSafeHwnd())
        m_wndEdit.SetFocus();
}

HBRUSH CMFCNotepadView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (pWnd == &m_wndEdit) {
        ThemeColors tc = theApp.GetThemeColors();
        pDC->SetTextColor(tc.textColor);
        pDC->SetBkColor(tc.bgColor);
        return (HBRUSH)m_brushBg.GetSafeHandle();
    }
    return CView::OnCtlColor(pDC, pWnd, nCtlColor);
}

LRESULT CMFCNotepadView::OnThemeChanged(WPARAM, LPARAM)
{
    UpdateColors();
    return 0;
}

void CMFCNotepadView::OnEditChange()
{
    CMFCNotepadDoc* pDoc = GetDocument();
    if (!pDoc)
    {
        TRACE("OnEditChange: 文档为空!\n");
        return;
    }

    if (m_bSkipUndo)
    {
        TRACE("OnEditChange: 跳过撤销记录\n");
        return;
    }

    CString newText;
    m_wndEdit.GetWindowText(newText);

    int start, end;
    m_wndEdit.GetSel(start, end);

    if (pDoc->m_strContent != newText)
    {
        pDoc->SaveUndoState(pDoc->m_strContent, start, end);
        TRACE("OnEditChange: 保存撤销状态，栈大小=%d\n", pDoc->m_undoStack.size());
    }

    pDoc->m_strContent = newText;
    pDoc->SetModifiedFlag(TRUE);

    int newWidth = CalcLineNumWidth();
    if (newWidth != m_nLineNumWidth)
    {
        m_nLineNumWidth = newWidth;
        UpdateLayout();
    }
}

// ========== 标准编辑命令实现 ==========
void CMFCNotepadView::OnEditCopy()
{
    TRACE("OnEditCopy 被调用\n");
    m_wndEdit.Copy();
}

void CMFCNotepadView::OnEditCut()
{
    TRACE("OnEditCut 被调用\n");
    m_wndEdit.Cut();
}

void CMFCNotepadView::OnEditPaste()
{
    TRACE("OnEditPaste 被调用\n");
    m_wndEdit.Paste();
}

void CMFCNotepadView::OnEditSelectAll()
{
    TRACE("OnEditSelectAll 被调用\n");
    m_wndEdit.SetSel(0, -1);
}

void CMFCNotepadView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    int start, end;
    m_wndEdit.GetSel(start, end);
    BOOL bHasSelection = (start != end);
    pCmdUI->Enable(bHasSelection);
}

void CMFCNotepadView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
    int start, end;
    m_wndEdit.GetSel(start, end);
    BOOL bHasSelection = (start != end);
    pCmdUI->Enable(bHasSelection);
}

void CMFCNotepadView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    // 检查剪贴板是否有文本
    BOOL bCanPaste = IsClipboardFormatAvailable(CF_TEXT) ||
        IsClipboardFormatAvailable(CF_UNICODETEXT);
    pCmdUI->Enable(bCanPaste);
}

void CMFCNotepadView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
    // 有内容就可以全选
    BOOL bHasText = (m_wndEdit.GetWindowTextLength() > 0);
    pCmdUI->Enable(bHasText);
}

// ========== 自定义编辑命令 ==========
void CMFCNotepadView::OnEditUndo()
{
    CMFCNotepadDoc* pDoc = GetDocument();
    if (!pDoc)
    {
        TRACE("OnEditUndo: 文档为空\n");
        return;
    }

    if (!pDoc->CanUndo())
    {
        TRACE("OnEditUndo: 无法撤销，栈为空\n");
        return;
    }

    TRACE("OnEditUndo: 执行撤销，栈大小=%d\n", pDoc->m_undoStack.size());

    CString curText;
    m_wndEdit.GetWindowText(curText);
    int start, end;
    m_wndEdit.GetSel(start, end);
    pDoc->m_redoStack.push({ curText, start, end });

    UndoItem item = pDoc->m_undoStack.top();
    pDoc->m_undoStack.pop();

    m_bSkipUndo = TRUE;
    m_wndEdit.SetWindowText(item.text);
    m_wndEdit.SetSel(item.selStart, item.selEnd);
    m_wndEdit.SetFocus();
    pDoc->m_strContent = item.text;
    m_bSkipUndo = FALSE;

    TRACE("OnEditUndo: 撤销完成，剩余栈大小=%d\n", pDoc->m_undoStack.size());
}

void CMFCNotepadView::OnEditRedo()
{
    CMFCNotepadDoc* pDoc = GetDocument();
    if (!pDoc)
    {
        TRACE("OnEditRedo: 文档为空\n");
        return;
    }

    if (!pDoc->CanRedo())
    {
        TRACE("OnEditRedo: 无法重做，栈为空\n");
        return;
    }

    TRACE("OnEditRedo: 执行重做，栈大小=%d\n", pDoc->m_redoStack.size());

    CString curText;
    m_wndEdit.GetWindowText(curText);
    int start, end;
    m_wndEdit.GetSel(start, end);
    pDoc->m_undoStack.push({ curText, start, end });

    UndoItem item = pDoc->m_redoStack.top();
    pDoc->m_redoStack.pop();

    m_bSkipUndo = TRUE;
    m_wndEdit.SetWindowText(item.text);
    m_wndEdit.SetSel(item.selStart, item.selEnd);
    m_wndEdit.SetFocus();
    pDoc->m_strContent = item.text;
    m_bSkipUndo = FALSE;

    TRACE("OnEditRedo: 重做完成，剩余栈大小=%d\n", pDoc->m_redoStack.size());
}

void CMFCNotepadView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
    CMFCNotepadDoc* pDoc = GetDocument();
    BOOL bCanUndo = (pDoc && pDoc->CanUndo());
    pCmdUI->Enable(bCanUndo);
}

void CMFCNotepadView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
    CMFCNotepadDoc* pDoc = GetDocument();
    BOOL bCanRedo = (pDoc && pDoc->CanRedo());
    pCmdUI->Enable(bCanRedo);
}

// ========== 查找/替换 ==========
BOOL CMFCNotepadView::DoFind(BOOL bForward)
{
    if (m_findOpt.findText.IsEmpty()) return FALSE;

    CString text;
    m_wndEdit.GetWindowText(text);
    int start, end;
    m_wndEdit.GetSel(start, end);
    int searchStart = bForward ? end : start - 1;

    int foundPos = -1, foundLen = m_findOpt.findText.GetLength();

    if (m_findOpt.useRegex) {
        try {
            std::wstring ws((LPCTSTR)text);
            std::wstring pat((LPCTSTR)m_findOpt.findText);
            auto flags = std::regex_constants::ECMAScript;
            if (!m_findOpt.matchCase) flags |= std::regex_constants::icase;
            std::wregex re(pat, flags);
            std::wsmatch match;

            if (bForward) {
                auto it = ws.begin() + max(0, searchStart);
                std::wstring searchStr(it, ws.end());
                if (std::regex_search(searchStr, match, re)) {
                    foundPos = (int)(match[0].first - searchStr.begin() + searchStart);
                    foundLen = (int)match[0].length();
                }
            }
            else {
                auto it = ws.begin();
                auto searchEnd = ws.begin() + max(0, searchStart);
                std::wstring searchStr(it, searchEnd);
                std::wsregex_iterator regexIt(searchStr.begin(), searchStr.end(), re);
                std::wsregex_iterator regexEnd;
                for (; regexIt != regexEnd; ++regexIt) {
                    match = *regexIt;
                    foundPos = (int)(match[0].first - searchStr.begin());
                    foundLen = (int)match[0].length();
                }
            }
        }
        catch (...) {
            AfxMessageBox(_T("正则表达式错误"));
            return FALSE;
        }
    }
    else {
        CString searchText = text, findText = m_findOpt.findText;
        if (!m_findOpt.matchCase) {
            searchText.MakeLower();
            findText.MakeLower();
        }
        if (bForward) {
            foundPos = searchText.Find(findText, max(0, searchStart));
        }
        else {
            for (int i = max(0, searchStart) - 1; i >= 0; i--) {
                if (searchText.Mid(i, findText.GetLength()) == findText) {
                    foundPos = i;
                    break;
                }
            }
        }
    }

    if (foundPos >= 0) {
        m_wndEdit.SetSel(foundPos, foundPos + foundLen);
        m_wndEdit.SetFocus();
        return TRUE;
    }
    AfxMessageBox(_T("未找到"));
    return FALSE;
}

int CMFCNotepadView::DoReplaceAll()
{
    if (m_findOpt.findText.IsEmpty()) return 0;

    CString text;
    m_wndEdit.GetWindowText(text);
    CString newText;
    int count = 0;

    if (m_findOpt.useRegex) {
        try {
            std::wstring ws((LPCTSTR)text);
            std::wstring pat((LPCTSTR)m_findOpt.findText);
            std::wstring rep((LPCTSTR)m_findOpt.replaceText);
            auto flags = std::regex_constants::ECMAScript;
            if (!m_findOpt.matchCase) flags |= std::regex_constants::icase;
            std::wregex re(pat, flags);

            auto begin = std::wsregex_iterator(ws.begin(), ws.end(), re);
            count = (int)std::distance(begin, std::wsregex_iterator());

            if (count > 0) {
                newText = std::regex_replace(ws, re, rep).c_str();
            }
        }
        catch (...) {
            AfxMessageBox(_T("正则表达式错误"));
            return 0;
        }
    }
    else {
        newText = text;
        int pos = 0;
        CString findText = m_findOpt.findText;
        if (!m_findOpt.matchCase) {
            findText.MakeLower();
        }

        while (TRUE) {
            CString temp = newText;
            if (!m_findOpt.matchCase) temp.MakeLower();
            pos = temp.Find(findText, pos);
            if (pos < 0) break;
            newText = newText.Left(pos) + m_findOpt.replaceText +
                newText.Mid(pos + m_findOpt.findText.GetLength());
            pos += m_findOpt.replaceText.GetLength();
            count++;
        }
    }

    if (count > 0) {
        CMFCNotepadDoc* pDoc = GetDocument();
        if (pDoc) {
            int s, e;
            m_wndEdit.GetSel(s, e);
            pDoc->SaveUndoState(text, s, e);
        }
        m_bSkipUndo = TRUE;
        m_wndEdit.SetWindowText(newText);
        SyncToDoc();
        if (pDoc) pDoc->SetModifiedFlag(TRUE);
        m_bSkipUndo = FALSE;
    }
    return count;
}

void CMFCNotepadView::OnEditFind()
{
    CFindReplaceDlgEx dlg(this, FALSE);
    dlg.DoModal();
}

void CMFCNotepadView::OnEditReplace()
{
    CFindReplaceDlgEx dlg(this, TRUE);
    dlg.DoModal();
}

// ========== 视图命令 ==========
void CMFCNotepadView::OnViewLineNumbers() { m_bShowLineNum = !m_bShowLineNum; UpdateLayout(); }
void CMFCNotepadView::OnViewThemeLight() { theApp.SetTheme(THEME_LIGHT); }
void CMFCNotepadView::OnViewThemeDark() { theApp.SetTheme(THEME_DARK); }
void CMFCNotepadView::OnViewFontIncrease() { if (m_nFontSize < 72) { m_nFontSize += 2; UpdateFont(); } }
void CMFCNotepadView::OnViewFontDecrease() { if (m_nFontSize > 8) { m_nFontSize -= 2; UpdateFont(); } }
void CMFCNotepadView::OnViewFontReset() { m_nFontSize = 12; UpdateFont(); }
void CMFCNotepadView::OnUpdateViewLineNumbers(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_bShowLineNum); }
void CMFCNotepadView::OnUpdateViewThemeLight(CCmdUI* pCmdUI) { pCmdUI->SetRadio(theApp.m_theme == THEME_LIGHT); }
void CMFCNotepadView::OnUpdateViewThemeDark(CCmdUI* pCmdUI) { pCmdUI->SetRadio(theApp.m_theme == THEME_DARK); }