#ifndef TEST_UTILS_H
#define TEST_UTILS_H
#include "test.h"
#include "../utils.h"
namespace test {

static void test_utils() {
    std::vector<std::string> vec = utils::split("/path//", '/');
    DEBUG_INFO<<vec.size();
    vec = utils::split2("/path//", "/");
    DEBUG_INFO<<vec.size();
}

} //namespace
#endif // TEST_UTILS_H

