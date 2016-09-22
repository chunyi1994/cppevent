
#include "qq_crawler.h"

QQCrawler::QQCrawler(EventLoop *loop) :
    client_(loop),
    pattern_(PATTERN_STR)
{
    client_.setMessageCallback(
                std::bind(&QQCrawler::handleMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void QQCrawler::start()
{
    client_.get(URL, 80);
}

void QQCrawler::handleMessage(const HttpMessage &response, const string &content)
{
//    for(const auto& w : response)
//    {
//        cout<<w.first<<" : " << w.second<<endl;
//    }
    auto words_begin = std::sregex_iterator(content.begin(), content.end(), pattern_);
    auto words_end = std::sregex_iterator();
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string match_str = match[1];
        std::cout << match_str << '\n';
        std::string url = "http://" + match_str + ".jpg";
        log(url);
    }
}


static void regex_test(const string& content)
{
    std::regex pattern("src=\"http://(.*?)\\.jpg\"");

}
