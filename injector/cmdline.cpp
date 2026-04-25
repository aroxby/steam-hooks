#include <sstream>
#include <string>
#include <windows.h>

std::wstring Utf8ToWide(const char *s) {
    int needed = MultiByteToWideChar(CP_UTF8, 0, s, -1, nullptr, 0);
    if (needed <= 0) {
        return L"";
    }

    std::wstring out(static_cast<size_t>(needed), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s, -1, out.data(), needed);
    out.resize(static_cast<size_t>(needed - 1));
    return out;
}

std::wstring QuoteForWindowsCommandLine(const std::wstring &arg) {
    if (arg.empty()) {
        return L"\"\"";
    }

    bool needsQuotes = false;
    for (wchar_t ch : arg) {
        if (ch == L' ' || ch == L'\t' || ch == L'\n' || ch == L'\v' ||
            ch == L'"') {
            needsQuotes = true;
            break;
        }
    }

    if (!needsQuotes) {
        return arg;
    }

    std::wstring result;
    result.push_back(L'"');

    size_t backslashes = 0;
    for (wchar_t ch : arg) {
        if (ch == L'\\') {
            ++backslashes;
            continue;
        }

        if (ch == L'"') {
            result.append(backslashes * 2 + 1, L'\\');
            result.push_back(L'"');
            backslashes = 0;
            continue;
        }

        if (backslashes > 0) {
            result.append(backslashes, L'\\');
            backslashes = 0;
        }
        result.push_back(ch);
    }

    if (backslashes > 0) {
        result.append(backslashes * 2, L'\\');
    }

    result.push_back(L'"');
    return result;
}

std::wstring BuildCommandLine(int argc, char *argv[]) {
    std::wostringstream command;
    for (int i = 1; i < argc; ++i) {
        if (i > 1) {
            command << L' ';
        }
        command << QuoteForWindowsCommandLine(Utf8ToWide(argv[i]));
    }
    return command.str();
}
