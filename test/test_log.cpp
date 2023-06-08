/*
 * @Author: w 2652289370@qq.com
 * @Date: 2023-06-06 09:27:36
 * @LastEditors: w 2652289370@qq.com
 * @LastEditTime: 2023-06-06 11:03:52
 * @FilePath: /cilent/test/test_log.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <iostream>
#include "Log.h"

using namespace w;

int main(int argc, char** argv)
{
    LogFormatter fmt("sssssss%m{si{}isi}ss   d%dc         ssd");
    
    std::shared_ptr<int>(new int);
    return 0;
}
