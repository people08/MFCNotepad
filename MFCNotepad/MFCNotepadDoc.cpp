#include "pch.h"
#include "MFCNotepad.h"
#include "MFCNotepadDoc.h"

IMPLEMENT_DYNCREATE(CMFCNotepadDoc, CDocument)

BEGIN_MESSAGE_MAP(CMFCNotepadDoc, CDocument)
END_MESSAGE_MAP()

CMFCNotepadDoc::CMFCNotepadDoc() {}
CMFCNotepadDoc::~CMFCNotepadDoc() {}

BOOL CMFCNotepadDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    m_strContent.Empty();
    while (!m_undoStack.empty()) m_undoStack.pop();
    while (!m_redoStack.empty()) m_redoStack.pop();

    return TRUE;
}

BOOL CMFCNotepadDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    // 读取文件
    CFile file;
    if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite))
        return FALSE;

    ULONGLONG len = file.GetLength();
    if (len > 0) {
        // 尝试UTF-8读取
        char* buf = new char[len + 1];
        file.Read(buf, (UINT)len);
        buf[len] = 0;
        file.Close();

        // 检测BOM并转换
        int offset = 0;
        if (len >= 3 && (BYTE)buf[0] == 0xEF && (BYTE)buf[1] == 0xBB && (BYTE)buf[2] == 0xBF)
            offset = 3;

        int wlen = MultiByteToWideChar(CP_UTF8, 0, buf + offset, -1, NULL, 0);
        if (wlen > 0) {
            m_strContent.GetBufferSetLength(wlen);
            MultiByteToWideChar(CP_UTF8, 0, buf + offset, -1, m_strContent.GetBuffer(), wlen);
            m_strContent.ReleaseBuffer();
        }
        delete[] buf;
    }
    else {
        file.Close();
        m_strContent.Empty();
    }

    while (!m_undoStack.empty()) m_undoStack.pop();
    while (!m_redoStack.empty()) m_redoStack.pop();
    SetModifiedFlag(FALSE);

    return TRUE;
}

BOOL CMFCNotepadDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    CFile file;
    if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeWrite))
        return FALSE;

    // 保存为UTF-8
    int len = WideCharToMultiByte(CP_UTF8, 0, m_strContent, -1, NULL, 0, NULL, NULL);
    if (len > 0) {
        char* buf = new char[len];
        WideCharToMultiByte(CP_UTF8, 0, m_strContent, -1, buf, len, NULL, NULL);
        file.Write(buf, len - 1);  // 不写入null终止符
        delete[] buf;
    }
    file.Close();
    SetModifiedFlag(FALSE);

    return TRUE;
}

void CMFCNotepadDoc::SaveUndoState(const CString& text, int selStart, int selEnd)
{
    UndoItem item = { text, selStart, selEnd };
    m_undoStack.push(item);
    // 新操作清空重做栈
    while (!m_redoStack.empty()) m_redoStack.pop();
}

void CMFCNotepadDoc::SetModifiedFlag(BOOL bModified)
{
    CDocument::SetModifiedFlag(bModified);

    // 更新标题显示 * 号
    CString title = GetTitle();
    if (bModified && title.Right(1) != _T("*"))
        SetTitle(title + _T("*"));
    else if (!bModified && title.Right(1) == _T("*"))
        SetTitle(title.Left(title.GetLength() - 1));
}