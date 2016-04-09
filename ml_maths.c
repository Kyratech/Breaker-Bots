#include <stdio.h>
#include <stdint.h>
#include "ml_maths.h"

uint16_t sin_table_1000[91] = {
0,
17,
35,
52, 
70,
87,
105, 
122, 
139,
156,
174,
191, 
208, 
225, 
242, 
259, 
276, 
292,
309, 
326, 
342, 
358, 
375, 
391, 
407,
423, 
438, 
454, 
469, 
485, 
500, 
515, 
530, 
545, 
559, 
574,
588, 
602,
616, 
629, 
643, 
656, 
669, 
682, 
695,
707, 
719, 
731, 
743, 
755, 
766, 
777,
788, 
799, 
809, 
819, 
829, 
839, 
848, 
857, 
866, 
875, 
883, 
891, 
899, 
906,
914, 
921, 
927, 
934, 
940, 
946, 
951, 
956, 
961, 
966, 
970, 
974, 
978, 
982,
985, 
988, 
990,
993, 
995, 
996, 
998, 
999, 
999, 
1000, 
1000}; 



/*
int main(void)
{
	unsigned int i;
	for(i = 0; i < 360; i++)
	{
		printf("sin(%u) = %d, \t cos(%u) = %d\n", i, ml_sin(i), i, ml_cos(i));
	}
	return 0;
}
*/
int16_t ml_sin(uint16_t x)
{
	uint8_t fraction = x % 90;
	uint8_t sector = (x % 360) / 90;

	if(sector == 0)
		return sin_table_1000[fraction];
	else if(sector == 1)
		return sin_table_1000[90 - fraction];
	else if(sector == 2)
		return -sin_table_1000[fraction];
	else
		return -sin_table_1000[90 - fraction];
}

int16_t ml_cos(uint16_t x)
{
	return ml_sin(x + 90);
}

/* Find the rounded integer value of the SQRT of a number (no sqrt hardware needed) */
uint16_t ml_sqrt(uint16_t n)
{
	uint16_t input = n;
	uint16_t result = 0;
	uint16_t one = 1u << 14;

	while(one > input)
	{
		one >>= 2;
	}

	while(one != 0)
	{
		if(input >= result + one)
		{
			input = input - (result + one);
			result = result + 2 * one;
		}
		result >>= 1;
		one >>= 2;
	}

	if(input > result)
	{
		result++;
	}

	return result;
}

/* Take a guess what these two functions do...*/
uint16_t ml_max(uint16_t a, uint16_t b)
{
	if(a > b)
		return a;
	else
		return b;
}

uint16_t ml_min(uint16_t a, uint16_t b)
{
	if(a > b)
		return b;
	else
		return a;
}

/* Limit a value to a range */
int16_t ml_clamp(int16_t low, int16_t high, int16_t value)
{
	if(value < low)
		return low;
	else if(value > high)
		return high;
	else
		return value;
}
