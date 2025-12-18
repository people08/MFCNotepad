// MFCNotepadView.h - 修复行号实时更新
#pragma once

#include "MFCNotepadDoc.h"

// 查找选项
struct FindOptions {
    CString findText;
    CString replaceText;
    BOOL useRegex;
    BOOL matchCase;
};

// ========== 自定义编辑控件（用于捕获滚动消息）==========
class CLineNumEdit : public CEdit
{
public:
    CLineNumEdit() : m_pParentView(NULL) {}
    void SetParentView(CView* pView) { m_pParentView = pView; }

protected:
    CView* m_pParentView;

    // 重写窗口过程以捕获滚动消息
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};

// ========== 视图类 ==========
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

    // 刷新行号区域（供子类化的CEdit调用）
    void InvalidateLineNumArea();

protected:
    CLineNumEdit m_wndEdit; // 使用自定义编辑控件
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
    // 提供编辑控件的访问接口
    const CEdit& GetEditCtrl() const { return m_wndEdit; }
    CEdit& GetEditCtrl() { return m_wndEdit; }

    // 查找替换
    BOOL DoFind(BOOL bForward);
    int DoReplaceAll();

    virtual void OnDraw(CDC* pDC);
    virtual void OnInitialUpdate();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg LRESULT OnThemeChanged(WPARAM, LPARAM);

    // 标准编辑命令
    afx_msg void OnEditCopy();
    afx_msg void OnEditCut();
    afx_msg void OnEditPaste();
    afx_msg void OnEditSelectAll();
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);

    // 自定义编辑命令
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