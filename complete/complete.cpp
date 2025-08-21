#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sys/stat.h>

// 函数原型声明
bool downloadFile(const std::string& filePath);
bool extractFile(const std::string& filePath);
bool compileJavaFile(const std::string& filePath);
void setEnvironmentVariable();
void unsetEnvironmentVariable();
void deleteFiles(const std::string& filePath);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./program <Java源文件下载URL>" << std::endl;
        return 1;
    }

    std::string javaFilePath = argv[1];

    // 防止误操作：禁止危险路径和空参数
    if (javaFilePath.empty() || javaFilePath == "/" || javaFilePath == "." || javaFilePath == "..") {
        std::cerr << "参数非法，操作已终止。" << std::endl;
        return 1;
    }


    // 清理历史残留
    deleteFiles(javaFilePath);

    // 下载文件
    if (!downloadFile(javaFilePath)) return 1;

    // 解压文件
    if (!extractFile("downloaded_file")) {
        deleteFiles(javaFilePath);
        return 1;
    }

    // 配置环境变量
    setEnvironmentVariable();

    // 查找解压目录下的Java文件
    std::string extractedJavaFile = "";
    FILE* fp = popen("find extracted_dir -name '*.java' | head -n 1", "r");
    if (fp) {
        char buf[512];
        if (fgets(buf, sizeof(buf), fp)) {
            extractedJavaFile = std::string(buf);
            // 去除换行符
            extractedJavaFile.erase(extractedJavaFile.find_last_not_of("\n\r") + 1);
        }
        pclose(fp);
    }
    if (extractedJavaFile.empty()) {
        std::cerr << "未找到Java源文件，操作终止。" << std::endl;
        unsetEnvironmentVariable();
        deleteFiles(javaFilePath);
        return 1;
    }

    // 编译 Java 代码
    if (!compileJavaFile(extractedJavaFile)) {
        unsetEnvironmentVariable();
        deleteFiles(javaFilePath);
        return 1;
    }

    // 存放可执行文件
    std::string executablePath = "build_dir";

    // 撤销环境变量更改
    unsetEnvironmentVariable();

    // 删除下载和解压的文件
    deleteFiles(javaFilePath);

    std::cout << "操作完成。编译产物在: " << executablePath << std::endl;
    return 0;
}

bool downloadFile(const std::string& filePath) {
    // 防止覆盖本地重要文件
    // 检查URL格式
    if (filePath.find("http://") != 0 && filePath.find("https://") != 0) {
        std::cerr << "下载地址必须以http://或https://开头。" << std::endl;
        return false;
    }
    // 再次确认本地无同名文件
    struct stat st;
    if (stat("downloaded_file", &st) == 0) {
        std::cerr << "downloaded_file已存在，操作终止以防误覆盖。" << std::endl;
        return false;
    }
    std::string command = "curl -fsSL -o downloaded_file --max-filesize 1048576 '" + filePath + "'";
    int ret = std::system(command.c_str());
    std::ifstream checkFile("downloaded_file");
    if (ret != 0 || !checkFile.good()) {
        std::cerr << "下载文件失败: " << filePath << std::endl;
        if (ret != 0) std::cerr << "curl返回码: " << ret << std::endl;
        return false;
    }
    return true;
}

bool extractFile(const std::string& filePath) {
    // 防止覆盖本地重要目录
    // 检查zip文件是否存在
    struct stat st;
    if (stat(filePath.c_str(), &st) != 0) {
        std::cerr << "待解压文件不存在: " << filePath << std::endl;
        return false;
    }
    // 再次确认本地无同名目录
    if (stat("extracted_dir", &st) == 0 && S_ISDIR(st.st_mode)) {
        std::cerr << "extracted_dir已存在，操作终止以防误覆盖。" << std::endl;
        return false;
    }
    std::string command = "unzip -o '" + filePath + "' -d extracted_dir";
    int ret = std::system(command.c_str());
    if (ret != 0) {
        std::cerr << "解压文件失败: " << filePath << std::endl;
        std::cerr << "unzip返回码: " << ret << std::endl;
        return false;
    }
    // 检查是否有.java文件
    bool foundJava = false;
    FILE* fp = popen("find extracted_dir -name '*.java' -print -quit", "r");
    if (fp) {
        char buf[256];
        if (fgets(buf, sizeof(buf), fp)) foundJava = true;
        pclose(fp);
    }
    if (!foundJava) {
        std::cerr << "解压后未发现Java源文件。" << std::endl;
        return false;
    }
    return true;
}

bool compileJavaFile(const std::string& filePath) {
    // 防止覆盖本地重要目录
    struct stat st;
    if (stat(filePath.c_str(), &st) != 0) {
        std::cerr << "待编译Java文件不存在: " << filePath << std::endl;
        return false;
    }
    if (stat("build_dir", &st) == 0 && S_ISDIR(st.st_mode)) {
        std::cerr << "build_dir已存在，操作终止以防误覆盖。" << std::endl;
        return false;
    }
    std::string command = "javac --enable-preview --release 21 -d build_dir '" + filePath + "'";
    int ret = std::system(command.c_str());
    if (ret != 0) {
        std::cerr << "编译Java文件失败: " << filePath << std::endl;
        std::cerr << "javac返回码: " << ret << std::endl;
        return false;
    }
    // 检查build_dir下是否有class文件
    bool foundClass = false;
    FILE* fp = popen("find build_dir -name '*.class' -print -quit", "r");
    if (fp) {
        char buf[256];
        if (fgets(buf, sizeof(buf), fp)) foundClass = true;
        pclose(fp);
    }
    if (!foundClass) {
        std::cerr << "编译后未发现class文件。" << std::endl;
        return false;
    }
    return true;
}

void setEnvironmentVariable() {
    // 设置GRAALVM_HOME和PATH
    // 假设GRAALVM安装在/opt/graalvm
    setenv("GRAALVM_HOME", "/opt/graalvm", 1);
    const char* oldPath = std::getenv("PATH");
    std::string path = oldPath ? oldPath : "";
    if (path.find("/opt/graalvm/bin:") != 0) {
        path = std::string("/opt/graalvm/bin:") + path;
        setenv("PATH", path.c_str(), 1);
    }
}

void unsetEnvironmentVariable() {
    // 恢复PATH，取消GRAALVM_HOME
    unsetenv("GRAALVM_HOME");
    const char* oldPath = std::getenv("PATH");
    if (!oldPath) return;
    std::string path = oldPath;
    std::string prefix = "/opt/graalvm/bin:";
    if (path.find(prefix) == 0) {
        path = path.substr(prefix.length());
        setenv("PATH", path.c_str(), 1);
    }
}

void deleteFiles(const std::string& filePath) {
    // 删除下载和解压的文件，防止误删
    struct stat st;
    if (stat("downloaded_file", &st) == 0 && S_ISREG(st.st_mode)) {
        std::system("rm -f downloaded_file");
    }
    if (stat("extracted_dir", &st) == 0 && S_ISDIR(st.st_mode)) {
        std::system("rm -rf extracted_dir");
    }
    if (stat("build_dir", &st) == 0 && S_ISDIR(st.st_mode)) {
        std::system("rm -rf build_dir");
    }
}
