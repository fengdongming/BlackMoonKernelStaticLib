#include "stdafx.h"
#include <math.h>
//算术运算 - 求余数
/*
    调用格式： 〈双精度小数型〉 求余数 （双精度小数型 被除数，双精度小数型 除数，... ） - 系统核心支持库->算术运算
    英文名称：mod
    求出两个数值的商，并返回余数部分，运算符号为“%”或“Mod”。本命令为初级命令。命令参数表中最后一个参数可以被重复添加。
    参数<1>的名称为“被除数”，类型为“双精度小数型（double）”。
    参数<2>的名称为“除数”，类型为“双精度小数型（double）”。
*/
LIBAPI(void, krnln_mod)
{

	PMDATA_INF pArg = &ArgInf;

	for(int i=1;i< nArgCount;i++)
	{
		ArgInf.m_double = fmod(ArgInf.m_double, pArg[i].m_double);
	}

	__asm
	{
		mov eax,ArgInf.m_unit.m_dwFormID;
		mov edx,ArgInf.m_unit.m_dwUnitID;
	}
}
