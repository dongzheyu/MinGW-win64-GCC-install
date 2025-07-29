# MinGW-w64 GCC Installer for Windows 🖥️  

![Windows](https://img.shields.io/badge/Windows-10+-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)  
一个简单易用的Windows平台MinGW-w64 GCC编译器套件安装工具。🚀  
软件下载:[仓库发布页面](https://github.com/dongzheyu/MinGW-win64-GCC-install/releases/)  
GitHub加速：[Watt Toolkit](https://steampp.net/)  

## 📋 简介

这个工具可以帮助你在Windows系统上快速安装和配置MinGW-w64 GCC编译器环境。它会自动下载并安装MSYS2、GCC编译器、curl和GDB调试器，并将它们添加到系统PATH环境变量中，让你可以立即开始C/C++开发。

## 🌟 特性

- ✅ 一键安装MinGW-w64 GCC工具链
- 📦 自动下载和安装MSYS2环境
- 🔧 安装常用开发工具 (GCC, curl, GDB)
- 🛠️ 自动配置系统环境变量
- 🗑️ 提供完整的卸载功能
- 🎯 支持自定义安装路径
- 🔐 自动请求管理员权限运行

## 🛠️ 安装的组件

- **MSYS2**: 轻量级的POSIX-like环境
- **GCC**: GNU编译器集合 (C/C++)
- **curl**: 命令行URL工具
- **GDB**: GNU调试器

## 📖 使用方法

1. 下载或编译本程序
2. 右键点击程序，选择"以管理员身份运行"
3. 在菜单中选择安装选项
4. 选择安装路径（默认为`C:\msys64`）
5. 等待安装完成
6. 重启命令提示符或PowerShell即可使用gcc等工具

## ⚙️ 系统要求

- Windows 10 或更高版本
- 管理员权限
- 网络连接（用于下载组件）

## 📁 安装目录结构

默认安装在 `C:\msys64` 目录下，包含以下重要子目录：
- `mingw64\bin`: GCC编译器和其他工具
- `usr\bin`: MSYS2基础工具

## 🗑️ 卸载说明

1. 运行程序并选择卸载选项
2. 确认要删除的安装目录
3. 程序会自动从PATH环境变量中移除相关路径
4. 手动检查系统环境变量确保完全清理

## 📄 许可证

本项目采用MIT许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## ⚠️ 注意事项

- 安装过程中请勿关闭程序
- 首次运行必须以管理员身份执行
- 安装完成后建议重启命令行环境
- 如果之前手动安装过MinGW，请先卸载避免冲突

## 🤝 贡献

欢迎提交Issue和Pull Request来改进这个工具。

## 📞 支持

如有问题，请提交GitHub Issue。
