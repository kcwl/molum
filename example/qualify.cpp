// qualify.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "aquarius/qualify.hpp"

using namespace aquarius;

struct B
{
    int c;
    int a;
};

struct A
{
    int a;
    long b;
    char c;
    std::vector<std::byte> d;
    std::string str;

    B a_;
};

struct ele_t
{
    int a;
    int b;
    int c;
    //std::vector<int> d;
};

int main()
{
    aquarius::iostream ios;

    A a = A{};
    a.a = 4;
    a.b = 5;
    a.c = 'c';
    a.a_.a = 3;
    a.a_.c = 7;
    a.str = "hello";
    a.d = {std::byte{1},std::byte{2},std::byte{3},std::byte{4}};

    //ios << a;
    //std::string json = "[{1,2,3},{4,5,6},{7,8,9}]";
    std::string json = "[1,23,456,7890]";
    //std::string json = "[{1,2,3,[4,5,6]},{1,3,5,[{7,8,9}]}]";
    ios << json;

    auto result = ios.from_json_value<std::vector<int>>();

	//std::vector<uint64_t> ui;

	//ios >> ui;

 //   auto result = ios.to_json_value<std::vector<uint64_t>>();

    //std::cout << result << std::endl;

    std::cin.get();
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
