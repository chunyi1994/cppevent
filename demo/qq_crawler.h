#ifndef QQ_CRAWLER_H
#define QQ_CRAWLER_H
#include <regex>
#include "../httpclient.h"
#include "../eventloop.h"
#include "../connection.h"
#include "../utils.h"

using namespace cppevent;
using namespace std;
//demo 作用: 将www.qq.com的所有图片地址都输出到命令行


const std::string URL = "http://www.qq.com";
const std::string PATTERN_STR = "src=\"http://(.*?)\\.jpg\"";

class QQCrawler
{
public:
    QQCrawler(EventLoop* loop);
    void start();
private:
    void handleMessage(const HttpMessage& response, const std::string& content);

    HttpClient client_;
    std::regex pattern_;
};

#endif // QQ_CRAWLER_H

