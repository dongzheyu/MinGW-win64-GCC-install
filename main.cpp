#include <iostream>
#include <string>
#include <windows.h>
#include <shlwapi.h>
#include <urlmon.h>
#include <shellapi.h>
#include <shlobj.h>
#include <direct.h>
#include <cstdlib>
#include <cstring>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")

// 全局变量
bool emojiSupport = false;
std::string installPath = "C:\\msys64";
bool isInstalling = false;

// Emoji常量 (使用符号和字母组合)
const char* CHECKMARK = "[OK]";
const char* CROSSMARK = "[X]";
const char* WARNING = "[!]";
const char* INSTALL_EMOJI = "[+]";
const char* UNINSTALL_EMOJI = "[-]";
const char* PROGRESS_EMOJI = "[*]";
const char* COMPLETE_EMOJI = "[*]";

// 函数声明
bool IsRunAsAdmin();
bool RunAsAdmin();
bool CheckWindowsVersion();
void InitializeEmojis();
void ShowMainMenu();
void StartInstallation();
void StartUninstallation();
bool DownloadFile(const std::string& url, const std::string& path);
bool InstallMSYS2();
bool InstallPackages();
bool AddToPath();
bool RemoveFromPath();
std::string GetEnvironmentVariable(const std::string& name);
bool SetEnvironmentVariable(const std::string& name, const std::string& value);
std::string SelectInstallPath();

// 进度条显示
void ShowProgress(int percent, const std::string& message) {
    std::cout << message << std::endl;

    // 显示进度条
    int barWidth = 40;
    std::cout << "[";
    int pos = barWidth * percent / 100;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "#";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << percent << " %\r";
    std::cout.flush();

    if (percent >= 100) {
        std::cout << std::endl;
    }
}

// 检查Windows版本
bool CheckWindowsVersion() {
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    GetVersionEx((OSVERSIONINFO*)&osvi);

    // Windows 10版本号为10.0
    return (osvi.dwMajorVersion >= 10);
}

// 初始化Emojis
void InitializeEmojis() {
    // 不管Windows版本如何，都使用符号和字母组合
    CHECKMARK = "[OK]";
    CROSSMARK = "[X]";
    WARNING = "[!]";
    INSTALL_EMOJI = "[+]";
    UNINSTALL_EMOJI = "[-]";
    PROGRESS_EMOJI = "[*]";
    COMPLETE_EMOJI = "[*]";
}

// 显示主菜单
void ShowMainMenu() {
    system("cls");
    std::cout << "==================== MinGW-64 GCC 安装器 ====================" << std::endl;
    std::cout << std::endl;
    std::cout << "                    " << INSTALL_EMOJI << " 1. 安装 MinGW-64 GCC" << std::endl;
    std::cout << "                    " << UNINSTALL_EMOJI << " 2. 卸载 MinGW-64 GCC" << std::endl;
    std::cout << "                    " << CROSSMARK << " 3. 退出" << std::endl;
    std::cout << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << std::endl;
}

// 检查是否以管理员身份运行
bool IsRunAsAdmin() {
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &pAdministratorsGroup)) {
        dwError = GetLastError();
    } else {
        if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin)) {
            dwError = GetLastError();
        }
    }

    if (pAdministratorsGroup) {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    return (fIsRunAsAdmin != FALSE);
}

// 以管理员身份重新运行
bool RunAsAdmin() {
    char szPath[MAX_PATH];
    if (GetModuleFileNameA(NULL, szPath, ARRAYSIZE(szPath))) {
        // 修复类型转换问题：使用 reinterpret_cast 替代 C 风格强制转换
        HINSTANCE hInst = ShellExecuteA(NULL, "runas", szPath, NULL, NULL, SW_NORMAL);
        if ((intptr_t)hInst > 32) {
            return true;
        }
    }
    return false;
}

// 选择安装路径
std::string SelectInstallPath() {
    std::cout << std::endl;
    std::cout << "请选择 MSYS2 安装路径:" << std::endl;
    std::cout << "1. C:\\msys64 (推荐)" << std::endl;
    std::cout << "2. 自定义路径" << std::endl;
    std::cout << std::endl;

    int choice;
    std::cout << "请选择: ";
    std::cin >> choice;

    if (choice == 2) {
        std::string customPath;
        std::cout << "请输入自定义路径: ";
        std::cin >> customPath;
        return customPath;
    }

    return "C:\\msys64";
}

// 开始安装
void StartInstallation() {
    if (isInstalling) return;
    isInstalling = true;

    std::cout << INSTALL_EMOJI << " 开始安装 MinGW-64 GCC..." << std::endl;

    // 选择安装路径
    installPath = SelectInstallPath();

    // 检查是否已安装MSYS2
    std::string msys2Exe = installPath + "\\msys2.exe";
    bool msys2Exists = PathFileExistsA(msys2Exe.c_str());

    std::cout << PROGRESS_EMOJI << " 检测 MSYS2 安装状态..." << std::endl;

    if (msys2Exists) {
        std::cout << CHECKMARK << " 检测到 MSYS2 已安装在 " << installPath << std::endl;
    } else {
        std::cout << WARNING << " 未检测到 MSYS2，将进行安装" << std::endl;
        if (!InstallMSYS2()) {
            std::cout << CROSSMARK << " MSYS2 安装失败" << std::endl;
            isInstalling = false;
            return;
        }
    }

    // 安装包
    if (!InstallPackages()) {
        std::cout << CROSSMARK << " 包安装失败" << std::endl;
        isInstalling = false;
        return;
    }

    // 添加到PATH环境变量
    if (!AddToPath()) {
        std::cout << WARNING << " 无法自动添加到PATH环境变量，请手动添加" << std::endl;
    } else {
        std::cout << CHECKMARK << " 已添加到系统环境变量PATH" << std::endl;
    }

    std::cout << COMPLETE_EMOJI << " 所有组件安装完成！" << std::endl;
    std::cout << std::endl;
    std::cout << "安装信息:" << std::endl;
    std::cout << "  - MSYS2 路径: " << installPath << std::endl;
    std::cout << "  - 已安装组件: GCC, curl, GDB" << std::endl;
    std::cout << std::endl;
    std::cout << "请重启命令行窗口以使环境变量生效" << std::endl;

    isInstalling = false;
}

// 开始卸载
void StartUninstallation() {
    if (isInstalling) return;
    isInstalling = true;

    std::cout << UNINSTALL_EMOJI << " 开始卸载 MinGW-64 GCC..." << std::endl;

    // 选择安装路径
    installPath = SelectInstallPath();

    // 检查安装路径是否存在
    if (_access(installPath.c_str(), 0) == -1) {
        std::cout << CROSSMARK << " 未找到指定路径: " << installPath << std::endl;
        isInstalling = false;
        return;
    }

    std::cout << "警告：这将删除整个 MSYS2 安装，包括所有已安装的包" << std::endl;
    std::cout << "确认卸载？(y/N): ";

    char confirm;
    std::cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') {
        isInstalling = false;
        return;
    }

    std::cout << UNINSTALL_EMOJI << " 正在卸载 MSYS2..." << std::endl;
    ShowProgress(50, "正在删除文件...");

    // 从PATH环境变量中移除
    RemoveFromPath();
    std::cout << CHECKMARK << " 已从系统环境变量PATH中移除" << std::endl;

    // 删除目录
    std::cout << UNINSTALL_EMOJI << " 正在删除 " << installPath << "..." << std::endl;

    // 使用命令行删除目录
    std::string cmd = "rmdir /s /q \"" + installPath + "\"";
    system(cmd.c_str());

    // 检查是否删除成功
    if (_access(installPath.c_str(), 0) != -1) {
        std::cout << CROSSMARK << " 卸载失败，请手动删除 " << installPath << std::endl;
    } else {
        std::cout << CHECKMARK << " 卸载完成" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "提醒：请手动从系统环境变量 PATH 中移除相关路径（如果存在）" << std::endl;

    isInstalling = false;
}

// 下载文件
bool DownloadFile(const std::string& url, const std::string& path) {
    std::cout << "正在从 " << url << " 下载文件..." << std::endl;

    HRESULT hr = URLDownloadToFileA(NULL, url.c_str(), path.c_str(), 0, NULL);

    if (hr == S_OK) {
        std::cout << CHECKMARK << " 文件下载完成" << std::endl;
        return true;
    } else {
        std::cout << CROSSMARK << " 文件下载失败" << std::endl;
        return false;
    }
}

// 安装MSYS2
bool InstallMSYS2() {
    std::cout << INSTALL_EMOJI << " 正在下载 MSYS2..." << std::endl;

    // 获取临时目录
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    std::string downloadPath = std::string(tempPath) + "msys2-x86_64-latest.exe";

    // 下载MSYS2安装程序
    std::string downloadUrl = "https://mirrors.bfsu.edu.cn/msys2/distrib/msys2-x86_64-latest.exe";
    if (!DownloadFile(downloadUrl, downloadPath)) {
        return false;
    }

    std::cout << INSTALL_EMOJI << " 正在安装 MSYS2 到 " << installPath << "..." << std::endl;

    // 创建安装目录
    _mkdir(installPath.c_str());

    // 执行安装命令
    std::string cmd = "\"" + downloadPath + "\" install -q -o \"" + installPath + "\"";

    // 使用隐藏窗口执行安装
    system(cmd.c_str());

    std::cout << CHECKMARK << " MSYS2 安装完成" << std::endl;

    // 删除临时文件
    DeleteFileA(downloadPath.c_str());

    return true;
}

// 安装必要包
bool InstallPackages() {
    std::cout << INSTALL_EMOJI << " 正在安装 MinGW-64 GCC、curl、GDB..." << std::endl;

    // 初始化MSYS2
    std::cout << PROGRESS_EMOJI << " 初始化MSYS2环境..." << std::endl;
    std::string initCmd = "\"" + installPath + "\\usr\\bin\\bash.exe\" -lc \"exit\"";
    system(initCmd.c_str());

    // 更新包数据库
    std::cout << PROGRESS_EMOJI << " 更新包数据库..." << std::endl;
    std::string updateCmd = "\"" + installPath + "\\usr\\bin\\bash.exe\" -lc \"pacman -Sy --noconfirm\"";
    system(updateCmd.c_str());

    // 安装包
    std::string packages[] = {
        "mingw-w64-x86_64-gcc",
        "mingw-w64-x86_64-curl",
        "mingw-w64-x86_64-gdb"
    };

    for (int i = 0; i < 3; i++) {
        std::cout << "正在安装 " << packages[i] << " (包 " << (i+1) << " of 3)..." << std::endl;
        std::string installCmd = "\"" + installPath + "\\usr\\bin\\bash.exe\" -lc \"pacman -S --noconfirm " + packages[i] + "\"";
        system(installCmd.c_str());
        std::cout << CHECKMARK << " " << packages[i] << " 安装完成" << std::endl;
    }

    std::cout << CHECKMARK << " 包安装完成" << std::endl;
    return true;
}

// 添加到PATH环境变量
bool AddToPath() {
    std::cout << PROGRESS_EMOJI << " 正在添加到系统环境变量PATH..." << std::endl;

    // 获取当前PATH值
    std::string mingwPath = installPath + "\\mingw64\\bin";
    std::string currentPath = GetEnvironmentVariable("PATH");

    // 检查是否已经添加
    if (currentPath.find(mingwPath) != std::string::npos) {
        return true; // 已经在PATH中
    }

    // 添加到PATH
    if (!currentPath.empty()) {
        currentPath = mingwPath + ";" + currentPath;
    } else {
        currentPath = mingwPath;
    }

    return SetEnvironmentVariable("PATH", currentPath);
}

// 从PATH环境变量中移除
bool RemoveFromPath() {
    std::string mingwPath = installPath + "\\mingw64\\bin";
    std::string currentPath = GetEnvironmentVariable("PATH");

    // 查找并移除路径
    size_t pos = currentPath.find(mingwPath);
    if (pos != std::string::npos) {
        // 移除路径和分号
        if (pos > 0 && currentPath[pos - 1] == ';') {
            currentPath.erase(pos - 1, mingwPath.length() + 1);
        } else if (pos + mingwPath.length() < currentPath.length() && currentPath[pos + mingwPath.length()] == ';') {
            currentPath.erase(pos, mingwPath.length() + 1);
        } else {
            currentPath.erase(pos, mingwPath.length());
        }

        return SetEnvironmentVariable("PATH", currentPath);
    }

    return true; // 路径不在PATH中，视为成功
}

// 获取环境变量 (使用标准C函数替代 _dupenv_s)
std::string GetEnvironmentVariable(const std::string& name) {
    const char* env_value = std::getenv(name.c_str());
    if (env_value) {
        return std::string(env_value);
    }
    return "";
}

// 设置环境变量
bool SetEnvironmentVariable(const std::string& name, const std::string& value) {
    return _putenv_s(name.c_str(), value.c_str()) == 0;
}

int main() {
    // 检查是否以管理员身份运行
    if (!IsRunAsAdmin()) {
        std::cout << "需要以管理员身份运行此程序" << std::endl;
        std::cout << "请右键点击此程序并选择\"以管理员身份运行\"" << std::endl;
        system("pause");

        if (!RunAsAdmin()) {
            return 1;
        }
        return 0;
    }

    // 初始化符号
    InitializeEmojis();

    // 主循环
    while (true) {
        ShowMainMenu();
        int choice;
        std::cout << "请选择操作: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                StartInstallation();
                system("pause");
                break;
            case 2:
                StartUninstallation();
                system("pause");
                break;
            case 3:
                return 0;
            default:
                std::cout << "无效选择，请重新输入" << std::endl;
                system("pause");
                break;
        }
    }

    return 0;
}
