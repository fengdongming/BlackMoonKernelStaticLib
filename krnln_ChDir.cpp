#include "stdafx.h"

//磁盘操作 - 改变目录
/*
    调用格式： 〈逻辑型〉 改变目录 （文本型 欲改变到的目录） - 系统核心支持库->磁盘操作
    英文名称：ChDir
    改变当前的目录，但不会改变缺省驱动器位置。成功返回真，失败返回假。本命令为初级命令。
    参数<1>的名称为“欲改变到的目录”，类型为“文本型（text）”。参数值指明哪个目录将成为新的缺省目录，可能会包含驱动器。如果没有指定驱动器，则在当前的驱动器上改变缺省目录。
*/
LIBAPI(BOOL, krnln_ChDir)
{
	return SetCurrentDirectory(ArgInf.m_pText);
}
