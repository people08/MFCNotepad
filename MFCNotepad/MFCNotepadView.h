#pragma once

#include "MFCNotepadDoc.h"

// 查找选项
struct FindOptions {
    CString findText;
    CString replaceText;
    BOOL useRegex;
    BOOL matchCase;
};

class CMFCNotepadView : public CView
{
    DECLARE_DYNCREATE(CMFCNotepadView)
protected:
    CMFCNotepadView();
public:
    virtual ~CMFCNotepadView();

    CMFCNotepadDoc* GetDocument() const;

    // 设置
    BOOL m_bShowLineNum;    // 显示行号
    int m_nFontSize;        // 字体大小 (8-72)

    // 查找
    FindOptions m_findOpt;

protected:
    CEdit m_wndEdit;        // 编辑控件
    CFont m_font;           // 当前字体
    CBrush m_brushBg;       // 背景画刷
    int m_nLineNumWidth;    // 行号区宽度
    BOOL m_bSkipUndo;       // 跳过撤销记录

    void UpdateFont();
    void UpdateColors();
    void UpdateLayout();
    int CalcLineNumWidth();
    void SyncToDoc();
    void SyncFromDoc();

public:
    // 提供编辑控件的只读访问接口
    const CEdit& GetEditCtrl() const { return m_wndEdit; }
    CEdit& GetEditCtrl() { return m_wndEdit; }

    // 查找替换
    BOOL DoFind(BOOL bForward);
    int DoReplaceAll();

    virtual void OnDraw(CDC* pDC);
    virtual void OnInitialUpdate();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg LRESULT OnThemeChanged(WPARAM, LPARAM);

    // 编辑命令
    afx_msg void OnEditUndo();
    afx_msg void OnEditRedo();
    afx_msg void OnEditFind();
    afx_msg void OnEditReplace();
    afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);

    // 视图命令
    afx_msg void OnViewLineNumbers();
    afx_msg void OnViewThemeLight();
    afx_msg void OnViewThemeDark();
    afx_msg void OnViewFontIncrease();
    afx_msg void OnViewFontDecrease();
    afx_msg void OnViewFontReset();
    afx_msg void OnUpdateViewLineNumbers(CCmdUI* pCmdUI);
    afx_msg void OnUpdateViewThemeLight(CCmdUI* pCmdUI);
    afx_msg void OnUpdateViewThemeDark(CCmdUI* pCmdUI);

    // 编辑控件通知
    afx_msg void OnEditChange();
};