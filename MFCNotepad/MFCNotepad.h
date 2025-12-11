#pragma once

#include "Resource.h"

// 主题枚举
enum Theme { THEME_LIGHT, THEME_DARK };

// 全局主题颜色
struct ThemeColors {
    COLORREF bgColor;       // 背景色
    COLORREF textColor;     // 文字色
    COLORREF lineNumBg;     // 行号背景
    COLORREF lineNumText;   // 行号文字
};

// 应用程序类
class CMFCNotepadApp : public CWinAppEx
{
public:
    CMFCNotepadApp();

    // 主题管理
    Theme m_theme;
    ThemeColors GetThemeColors();
    void SetTheme(Theme t);

    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
    afx_msg void OnAppAbout();
};

extern CMFCNotepadApp theApp;