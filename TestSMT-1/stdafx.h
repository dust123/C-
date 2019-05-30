// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
//#define NO_WARN_MBCS_MFC_DEPRECATION   //如果字符集采用多字节方式，编译会报警，那么需要使用此处来去掉报警。

//出现这样的警告，原因是因为：
//由于历史原因，MFC同时支持 Unicode 和 MBCS（multi - byte character set），但 Unicode 日渐变得流行，
//而 MBCS 逐渐用得越来越少，所以，新版本的 MFC 已经把 MBCS 作为过时的方法，
//所以用新版本的 Visual Studio 编译 MBCS 程序的时候会出现这样的警告信息，建议大家在新的程序中废弃 MBCS 而采用 Unicode。

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



 
// TODO:  在此处引用程序需要的其他头文件
