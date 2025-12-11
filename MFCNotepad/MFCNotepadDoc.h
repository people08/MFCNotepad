#pragma once

#include <stack>

// 撤销/重做操作记录
struct UndoItem {
    CString text;     // 操作前/后的完整文本
    int selStart;     // 光标位置
    int selEnd;
};

class CMFCNotepadDoc : public CDocument
{
    DECLARE_DYNCREATE(CMFCNotepadDoc)
protected:
    CMFCNotepadDoc();
public:
    virtual ~CMFCNotepadDoc();

    // 文档内容
    CString m_strContent;

    // 撤销/重做栈
    std::stack<UndoItem> m_undoStack;
    std::stack<UndoItem> m_redoStack;

    void SaveUndoState(const CString& text, int selStart, int selEnd);
    bool CanUndo() { return !m_undoStack.empty(); }
    bool CanRedo() { return !m_redoStack.empty(); }

    virtual BOOL OnNewDocument();
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
    virtual void SetModifiedFlag(BOOL bModified = TRUE);

    DECLARE_MESSAGE_MAP()
};