// Sample-Test1.cpp - MFC 多文档记事本单元测试
// 
// 测试核心逻辑（不依赖 MFC）
//

#include "pch.h"
#include "gtest/gtest.h"
#include <stack>
#include <string>
#include <algorithm>
#include <regex>

// ============================================================================
// 测试用数据结构（模拟实际程序中的结构）
// ============================================================================

// 撤销/重做操作记录
struct UndoItem {
    std::wstring text;
    int selStart;
    int selEnd;
};

// ============================================================================
// 测试套件 1: 撤销/重做栈测试
// ============================================================================

class UndoRedoTest : public ::testing::Test {
protected:
    std::wstring m_strContent;
    std::stack<UndoItem> m_undoStack;
    std::stack<UndoItem> m_redoStack;

    void SetUp() override {
        m_strContent = L"";
        while (!m_undoStack.empty()) m_undoStack.pop();
        while (!m_redoStack.empty()) m_redoStack.pop();
    }

    void SaveUndoState() {
        UndoItem item = { m_strContent, 0, 0 };
        m_undoStack.push(item);
        while (!m_redoStack.empty()) m_redoStack.pop();
    }

    bool Undo() {
        if (m_undoStack.empty()) return false;
        m_redoStack.push({ m_strContent, 0, 0 });
        m_strContent = m_undoStack.top().text;
        m_undoStack.pop();
        return true;
    }

    bool Redo() {
        if (m_redoStack.empty()) return false;
        m_undoStack.push({ m_strContent, 0, 0 });
        m_strContent = m_redoStack.top().text;
        m_redoStack.pop();
        return true;
    }
};

// 测试 1.1: 初始状态
TEST_F(UndoRedoTest, InitialState) {
    EXPECT_TRUE(m_undoStack.empty()) << "初始撤销栈应为空";
    EXPECT_TRUE(m_redoStack.empty()) << "初始重做栈应为空";
}

// 测试 1.2: 保存撤销状态
TEST_F(UndoRedoTest, SaveUndoState) {
    SaveUndoState();
    m_strContent = L"Hello";
    EXPECT_EQ(m_undoStack.size(), 1);
}

// 测试 1.3: 执行撤销
TEST_F(UndoRedoTest, PerformUndo) {
    SaveUndoState();  // 保存空状态
    m_strContent = L"Hello";

    EXPECT_TRUE(Undo());
    EXPECT_EQ(m_strContent, L"");
    EXPECT_TRUE(m_undoStack.empty());
    EXPECT_EQ(m_redoStack.size(), 1);
}

// 测试 1.4: 执行重做
TEST_F(UndoRedoTest, PerformRedo) {
    SaveUndoState();
    m_strContent = L"Hello";
    Undo();

    EXPECT_TRUE(Redo());
    EXPECT_EQ(m_strContent, L"Hello");
}

// 测试 1.5: 连续撤销
TEST_F(UndoRedoTest, ConsecutiveUndo) {
    SaveUndoState(); m_strContent = L"A";
    SaveUndoState(); m_strContent = L"AB";
    SaveUndoState(); m_strContent = L"ABC";

    Undo(); EXPECT_EQ(m_strContent, L"AB");
    Undo(); EXPECT_EQ(m_strContent, L"A");
    Undo(); EXPECT_EQ(m_strContent, L"");
    EXPECT_FALSE(Undo()) << "空栈撤销应返回false";
}

// 测试 1.6: 新操作清空重做栈
TEST_F(UndoRedoTest, NewOperationClearsRedoStack) {
    SaveUndoState(); m_strContent = L"Hello";
    Undo();
    EXPECT_FALSE(m_redoStack.empty());

    SaveUndoState();  // 新操作
    EXPECT_TRUE(m_redoStack.empty()) << "新操作应清空重做栈";
}

// ============================================================================
// 测试套件 2: 主题颜色测试
// ============================================================================

enum Theme { THEME_LIGHT, THEME_DARK };

class ThemeTest : public ::testing::Test {
protected:
    Theme m_theme = THEME_LIGHT;

    struct Colors {
        unsigned int bg, text, lineNumBg, lineNumText;
    };

    Colors GetColors() {
        if (m_theme == THEME_LIGHT)
            return { 0xFFFFFF, 0x000000, 0xF0F0F0, 0x808080 };
        else
            return { 0x1E1E1E, 0xD4D4D4, 0x282828, 0x858585 };
    }
};

// 测试 2.1: 默认亮色主题
TEST_F(ThemeTest, DefaultTheme) {
    EXPECT_EQ(m_theme, THEME_LIGHT);
}

// 测试 2.2: 亮色主题颜色
TEST_F(ThemeTest, LightThemeColors) {
    m_theme = THEME_LIGHT;
    auto c = GetColors();
    EXPECT_EQ(c.bg, 0xFFFFFF);
    EXPECT_EQ(c.text, 0x000000);
}

// 测试 2.3: 暗色主题颜色
TEST_F(ThemeTest, DarkThemeColors) {
    m_theme = THEME_DARK;
    auto c = GetColors();
    EXPECT_EQ(c.bg, 0x1E1E1E);
    EXPECT_NE(c.text, 0x000000);
}

// ============================================================================
// 测试套件 3: 查找功能测试
// ============================================================================

class FindTest : public ::testing::Test {
protected:
    int Find(const std::wstring& text, const std::wstring& pattern,
        int startPos, bool matchCase) {
        std::wstring t = text, p = pattern;
        if (!matchCase) {
            std::transform(t.begin(), t.end(), t.begin(), ::towlower);
            std::transform(p.begin(), p.end(), p.begin(), ::towlower);
        }
        size_t pos = t.find(p, startPos);
        return (pos != std::wstring::npos) ? (int)pos : -1;
    }
};

// 测试 3.1: 基本查找
TEST_F(FindTest, BasicFind) {
    EXPECT_EQ(Find(L"Hello World", L"World", 0, true), 6);
}

// 测试 3.2: 大小写敏感
TEST_F(FindTest, CaseSensitive) {
    EXPECT_EQ(Find(L"Hello World", L"world", 0, true), -1);
    EXPECT_EQ(Find(L"Hello World", L"world", 0, false), 6);
}

// 测试 3.3: 从指定位置查找
TEST_F(FindTest, FindFromPosition) {
    std::wstring text = L"Hello Hello Hello";
    EXPECT_EQ(Find(text, L"Hello", 0, true), 0);
    EXPECT_EQ(Find(text, L"Hello", 1, true), 6);
    EXPECT_EQ(Find(text, L"Hello", 7, true), 12);
}

// 测试 3.4: 查找不存在
TEST_F(FindTest, NotFound) {
    EXPECT_EQ(Find(L"Hello World", L"xyz", 0, true), -1);
}

// ============================================================================
// 测试套件 4: 替换功能测试
// ============================================================================

class ReplaceTest : public ::testing::Test {
protected:
    std::pair<std::wstring, int> ReplaceAll(
        const std::wstring& text,
        const std::wstring& pattern,
        const std::wstring& replacement) {

        std::wstring result = text;
        int count = 0;
        size_t pos = 0;

        while ((pos = result.find(pattern, pos)) != std::wstring::npos) {
            result = result.substr(0, pos) + replacement +
                result.substr(pos + pattern.length());
            pos += replacement.length();
            count++;
        }
        return { result, count };
    }
};


// ============================================================================
// 测试套件 5: 字体大小测试
// ============================================================================

class FontSizeTest : public ::testing::Test {
protected:
    int m_fontSize = 12;
    const int MIN_SIZE = 8;
    const int MAX_SIZE = 72;

    void Increase() { if (m_fontSize < MAX_SIZE) m_fontSize += 2; }
    void Decrease() { if (m_fontSize > MIN_SIZE) m_fontSize -= 2; }
    void Reset() { m_fontSize = 12; }
};

// 测试 5.1: 默认字体大小
TEST_F(FontSizeTest, DefaultSize) {
    EXPECT_EQ(m_fontSize, 12);
}

// 测试 5.2: 增大字体
TEST_F(FontSizeTest, Increase) {
    Increase();
    EXPECT_EQ(m_fontSize, 14);
}

// 测试 5.3: 减小字体
TEST_F(FontSizeTest, Decrease) {
    Decrease();
    EXPECT_EQ(m_fontSize, 10);
}

// 测试 5.4: 上限测试
TEST_F(FontSizeTest, UpperLimit) {
    m_fontSize = 72;
    Increase();
    EXPECT_EQ(m_fontSize, 72);
}

// 测试 5.5: 下限测试
TEST_F(FontSizeTest, LowerLimit) {
    m_fontSize = 8;
    Decrease();
    EXPECT_EQ(m_fontSize, 8);
}

// ============================================================================
// 测试套件 6: 行号计算测试
// ============================================================================

class LineNumberTest : public ::testing::Test {
protected:
    int CountLines(const std::wstring& text) {
        if (text.empty()) return 1;
        int count = 1;
        for (wchar_t c : text)
            if (c == L'\n') count++;
        return count;
    }

    int GetLineFromPos(const std::wstring& text, int pos) {
        int line = 0;
        for (int i = 0; i < pos && i < (int)text.length(); i++)
            if (text[i] == L'\n') line++;
        return line;
    }
};

// 测试 6.1: 空文本
TEST_F(LineNumberTest, EmptyText) {
    EXPECT_EQ(CountLines(L""), 1);
}

// 测试 6.2: 单行
TEST_F(LineNumberTest, SingleLine) {
    EXPECT_EQ(CountLines(L"Hello"), 1);
}

// 测试 6.3: 多行
TEST_F(LineNumberTest, MultiLine) {
    EXPECT_EQ(CountLines(L"A\nB\nC"), 3);
}

// 测试 6.4: 位置转行号
TEST_F(LineNumberTest, PosToLine) {
    std::wstring text = L"Line1\nLine2\nLine3";
    EXPECT_EQ(GetLineFromPos(text, 0), 0);
    EXPECT_EQ(GetLineFromPos(text, 6), 1);
    EXPECT_EQ(GetLineFromPos(text, 12), 2);
}

// ============================================================================
// 测试套件 7: 文件修改状态测试
// ============================================================================

class ModifiedFlagTest : public ::testing::Test {
protected:
    bool m_bModified = false;
    std::wstring m_strContent;
    std::wstring m_strOriginal;

    void SetContent(const std::wstring& text) {
        m_strContent = text;
        m_bModified = (m_strContent != m_strOriginal);
    }

    void Save() {
        m_strOriginal = m_strContent;
        m_bModified = false;
    }

    void NewDocument() {
        m_strContent = L"";
        m_strOriginal = L"";
        m_bModified = false;
    }
};

// 测试 7.1: 新文档未修改
TEST_F(ModifiedFlagTest, NewDocumentNotModified) {
    NewDocument();
    EXPECT_FALSE(m_bModified);
}

// 测试 7.2: 编辑后已修改
TEST_F(ModifiedFlagTest, ModifiedAfterEdit) {
    NewDocument();
    SetContent(L"Hello");
    EXPECT_TRUE(m_bModified);
}

// 测试 7.3: 保存后未修改
TEST_F(ModifiedFlagTest, NotModifiedAfterSave) {
    NewDocument();
    SetContent(L"Hello");
    Save();
    EXPECT_FALSE(m_bModified);
}

// 测试 7.4: 恢复原内容后未修改
TEST_F(ModifiedFlagTest, NotModifiedAfterRevert) {
    NewDocument();
    SetContent(L"Hello");
    SetContent(L"");  // 恢复为空
    EXPECT_FALSE(m_bModified);
}