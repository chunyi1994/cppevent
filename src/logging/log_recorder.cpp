#include "log_recorder.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h> //for mkdir
#include <thread>
#include <sstream>
#include<stdio.h> //rename
#include <unistd.h>
#include <vector>

#include "singleton.h"
namespace logging{

const char* LOG_PATH_FOLDER = "log";
const int MAX_LOG_FILES_NUM  = 10;
const int MAX_LOG_FILE_SIZE = 1024 * 10;

LogRecorder::LogRecorder():
    queue_(),
    quit_(false)
{
    create_dir_and_files();
    std::thread t(std::bind(&LogRecorder::customer_thread_function, this));
    t.detach();
}

void LogRecorder::submit(std::string msg) {
    queue_.push(msg);
}

Logger::OutputCallback LogRecorder::callback() {
    return [](std::string msg) {
        Singleton<LogRecorder>::get_instance().submit(msg);
    };
}

template<class T>
int to_int(const T& value) {
    std::stringstream ss;
    ss << value;
    int i = 0;
    ss >> i;
    return i;
}

template<class T>
std::string to_string(const T& value) {
    std::stringstream ss;
    ss << value;
    std::string str;
    ss >> str;
    return str;
}

int get_filesize(const std::string& path) {
    struct stat buf;
    if (stat(path.c_str(), &buf) < 0) {
        //printf("Mesg: %s\n", strerror(errno));
        return -1;
    }
    //    printf("%s's size      is %-4d      bytes\n", path.c_str(), buf.st_size);
    //    printf("%s's t_blksize is %-4d bytes\n", path.c_str(),      buf.st_blksize);
    //    printf("%s's blocks    is %-4d blocks\n",      path.c_str(), buf.st_blocks);
    return buf.st_size;
}

void LogRecorder::create_dir_and_files() {
    int isCreate = mkdir(LOG_PATH_FOLDER ,
                         S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    if (isCreate != 0) {
        //std::cout<<"创建文件夹失败"<<std::endl;
        return;
    }
    std::string filename = LOG_PATH_FOLDER;
    if (std::ifstream(filename + "/log0.txt")) {
        //std::cout<<"已存在文件，不用新创建"<<std::endl;
        return;
    }
    //创建好几个文件，log0.txt log1.txt log2.txt log3.txt .....
    for (int i = 0; i < MAX_LOG_FILES_NUM; ++i) {
        filename = LOG_PATH_FOLDER;
        filename = filename + "/log" + to_string(i) + ".txt";
        std::ofstream ofs(filename, std::ios::app);
        if (!ofs) continue;
        ofs<<"log"<<std::endl;
    }
}

void LogRecorder::customer_thread_function() {
    while (!quit_) {
        std::string log_msg = queue_.pop();
        std::string filename = LOG_PATH_FOLDER + std::string("/log0.txt");
        if (get_filesize(filename) >= MAX_LOG_FILE_SIZE) {
            adjust_filename();
        }
        std::ofstream ofs(filename, std::ios::app);
        if (ofs) {
            std::cout<<log_msg<<std::endl;
            ofs << log_msg << std::endl;
        } else {
           // std::cout<<filename<<"写入失败"<<std::endl;
        }
    }//while
}

void LogRecorder::adjust_filename() {
    if (MAX_LOG_FILES_NUM < 2) {
        throw;
    }
    std::string filename;
    std::vector<std::string> filenames;
    filenames.reserve(MAX_LOG_FILES_NUM);
    for (int i = 0; i < MAX_LOG_FILES_NUM; ++i) {
        filename = LOG_PATH_FOLDER + std::string("/log") + to_string(i) + ".txt";
        filenames.push_back(filename);
    }

    if (remove(filename.c_str()) != 0) {
       // std::cout<<"删除文件失败："<<filename<<std::endl;
    }

    for (int i = MAX_LOG_FILES_NUM - 2; i >= 0; --i) {
        if (rename(filenames[i].c_str(), filenames[i + 1].c_str()) != 0) {  //oldname, newname
            //std::cout<<"修改文件名失败："<<filenames[i]<<","<<filenames[i + 1]<<std::endl;
        }
    }
}


}

