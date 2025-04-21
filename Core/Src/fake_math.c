
// ================================================================================
// Fake math functions C-file
// ================================================================================

#include "fake_math.h"


int math_round(float num)
{
	int returnNum = num;
	float flCompNum = num * 10;
	int compNum = flCompNum;
	if (compNum < 0)
	{
		compNum *= -1;
	}

	if (compNum % 10 >= 5)
	{
		if (num < 0)
		{
			returnNum--;
		}
		else
		{
			returnNum++;
		}
	}
	return returnNum;
}

int math_getRoot(int num)
{
	if (num <= 0)
	{
		return 0;
	}

	int firstRoot, secondRoot;
	for (int i = 1; i <= num; i++)
	{
		if (i * i >= num)
		{
			secondRoot = i;
			firstRoot = i - 1;
		}
	}

	if ((num - firstRoot * firstRoot) > (secondRoot * secondRoot - num))
	{
		return secondRoot;
	}
	else
	{
		return firstRoot;
	}
}


int min(int num1, int num2)
{
	return num1 < num2 ? num1 : num2;
}

int max(int num1, int num2)
{
	return num1 < num2 ? num2 : num1;
}
