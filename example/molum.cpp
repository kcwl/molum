﻿// qualify.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "molum.hpp"

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

#define varic_2(a,...) a
#define varic_1(b,...) b,varic_2(...)
#define varic(a,...) a,varic_1(...)

#define var(a,b,c) a,b,c

struct ele_t
{
    int a;
    int b;
    int c;
    //std::vector<int> d;
};

int main()
{
    molum::archive_stream ios;

    auto [var(a, b, c)] = ele_t{};

    ele_t ele{1,3,3};

    auto data = molum::serialize(ele);

    //int ele2;

    //auto ele2 =  molum::deserialize<ele_t>(data);
    ele_t ele2;
    molum::deserialize<ele_t>(data, ele2);


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
