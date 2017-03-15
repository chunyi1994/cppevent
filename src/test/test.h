#ifndef TEST_H
#define TEST_H
#include <iostream>
#include <sstream>
namespace test {

class SimpleDebugPrinter {
public:
    SimpleDebugPrinter() :ss_() { }
    ~SimpleDebugPrinter() { std::cout<<ss_.str()<<std::endl; }
    std::stringstream& stream() {return ss_;}
private:
      std::stringstream ss_;
};

#define DEBUG_INFO test::SimpleDebugPrinter().stream()<<"[DEBUG_INFO]"

#define PRINT_RESULT(x)                              \
    do {                                                              \
        if (x) {                                                       \
            std::cout<<"【OK】";                         \
        } else {                                                      \
             std::cout<<"【FAILED】";                  \
        }                                                                 \
    } while (0)

#define EXIT_IF_FALSE(x)                              \
    do {                                                              \
        if (!(x)) {                                                    \
              exit(0);                                               \
        }                                                               \
    } while (0)

#define STR(s)  #s

#define PRINT_INFO()                                   \
    do {                                                              \
        std::cout<<"In "<<__FILE__<<","<< __func__ <<"("<<__LINE__<<"):"; \
    }while(0) \


#define EXCEPT_TRUE(x)                                                                   \
    do {                                                                                                  \
        PRINT_RESULT(x);                                                                        \
        PRINT_INFO();                                                                               \
         std::cout<<"statement is '" << STR(x) << "'";                          \
        std::cout<<", except true" << std::endl;                                  \
        EXIT_IF_FALSE((x));                                                                    \
    }while(0)

#define EXCEPT_FALSE(x)                                                                   \
    do {                                                                                                    \
        PRINT_RESULT(!(x));                                                                         \
        PRINT_INFO();                                                                               \
        std::cout<<"statement is '" << STR(x) << "'";                             \
        std::cout<<", except false"<<std::endl;                                      \
        EXIT_IF_FALSE(!(x));                                                                      \
    }while(0)

#define EXCEPT_EQ(a, b)                                                                   \
    do {                                                                                                  \
        PRINT_RESULT((a) == (b));                                                         \
        PRINT_INFO();                                                                             \
        std::cout<<"except '" << STR(a) <<" == " <<STR(b) ;               \
        std::cout<<"'"<<std::endl;                                                          \
        EXIT_IF_FALSE((a) == (b));                                                           \
    }while(0)

#define EXCEPT_NE(a, b)                                                                   \
    do {                                                                                                  \
        PRINT_RESULT((a) != (b));                                                          \
        PRINT_INFO();                                                                             \
        std::cout<<"except '" << STR(a) <<" != " <<STR(b) ;                \
        std::cout<<"'"<<std::endl;                                                         \
        EXIT_IF_FALSE((a) != (b));                                                                      \
    }while(0)
} //namespace
#endif // TEST_H

