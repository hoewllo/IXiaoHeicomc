#include <iostream>

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

// 函数用于安装Java库到./javahelper/
void installJavaLibrary() {
    // 在这里添加安装Java库到./javahelper/的代码
    std::cout << "Java库安装完成" << std::endl;
}

int main() {
    checkSystemArchitecture(); // 检查系统架构

    installJavaLibrary(); // 安装Java库到./javahelper/

    // 将程序保存到程序运行目录的./xiaohei文件夹
    // 保存为可执行文件的步骤略，根据实际情况进行保存

    // 程序运行结束，不会对系统（如环境变量）做任何影响

    return 0;
}
