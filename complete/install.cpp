#include <iostream>
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#elif __APPLE__
#include <TargetConditionals.h>
#endif

// 函数用于检查系统架构
int checkSystemArchitecture() {
    #ifdef _WIN32
        return 1;
    #endif
    #ifdef __linux__
        return 2;
    #endif
    #ifdef __APPLE__
        return 3;
    #endif
    return 0;
}

int installJavaLibrary() {
    #ifdef _WIN32
    std::cout << "### Windows Java安装指南

欢迎来到我们的Windows Java安装指南！如果您想在Windows系统上安装Java开发工具包（JDK），您来对地方了。请按照以下简单步骤进行操作：

#### 步骤一：下载Java JDK
1. 访问[Oracle官方网站](https://www.oracle.com/java/technologies/javase-jdk11-downloads.html)或[AdoptOpenJDK网站](https://adoptopenjdk.net/)。
2. 在网站上找到适合您系统的JDK版本，并下载安装程序。

#### 步骤二：安装Java JDK
1. 运行下载的安装程序。
2. 按照安装向导的指示完成安装过程。

#### 步骤三：设置Java环境变量
1. 打开“控制面板” -> “系统与安全” -> “系统” -> “高级系统设置”。
2. 在“系统属性”窗口中，点击“环境变量”按钮。
3. 在“系统变量”部分，点击“新建”。
4. 输入变量名：JAVA_HOME，变量值：您的Java安装路径（例如C:\Program Files\Java\jdk-11）。
5. 找到系统变量中的Path变量，点击“编辑”。
6. 在变量值的最后加上“;%JAVA_HOME%\bin”，然后点击“确定”保存更改。

现在，您已经成功安装并配置了Java JDK在您的Windows系统上。欢迎开始您的Java编程之旅吧！

**要了解更多关于Java的信息，请访问：**
- [Oracle Java官方网站](https://www.oracle.com/java/)
- [AdoptOpenJDK网站](https://adoptopenjdk.net/)

希望这个指南能帮助您顺利安装Java JDK。如有任何疑问，请随时与我们联系。祝您编程愉快！" << std::endl;
    #elif __linux__
    system("apt update && apt install openjdk-11-jdk");
    if(system("java -version") == 0){
        return -1;
    }
    return 0;
    #elif __APPLE__
    system("brew update && brew install openjdk@11");
    if(system("java -version") == 0){
        return -1;
    }
    return 0;
    #endif
    std::cout << "Java库安装完成" << std::endl;
}

int checkPermission(int s) {
    #ifdef _WIN32
        if(!IsUserAnAdmin()) {//Windows下，包含Windows.h
            std::cout << "需要管理员权限" << std::endl;
            return -1;
        }
    #elif __linux__ || __APPLE__ // Linux和macOS下
        if (geteuid() != 0) {
            std::cout << "需要root权限" << std::endl;
            return -1;
        }
    #endif
    return 0;
}


int main() {
    int sysArch=0;
    sysArch = checkSystemArchitecture(); // 检查系统架构
    if (sysArch == 0 && checkPermission(sysArch) != 0) {
        std::cout << "不支持的系统架构或无权限" << std::endl;
        return -1;
    }
    delete &sysArch;
    if (installJavaLibrary() != 0) {
        std::cout << "Java库安装失败" << std::endl;
        return -1;
    }

    // 将程序保存到程序运行目录的./xiaohei文件夹// 保存为可执行文件的步骤略，根据实际情况进行保存
// 程序运行结束，不会对系统（如环境变量）做任何影响
    return 0;
}
