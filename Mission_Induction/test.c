#include <stdio.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <malloc.h>

int main()
{
	wchar_t *chinese_str = L"ATK-SIM800C中英文短信发送测试";
	unsigned int *p = (wchar_t *)chinese_str;
	
	char* data = (char*)malloc(4 * wcslen(chinese_str));
	int i;
	int j;
	for (i = 0, j = 0; i < wcslen(chinese_str); i++)
	{
		j += sprintf(data + j, "%.4x", p[i]);
	}

	for(i = 0; i < 4 * wcslen(chinese_str); ++i)
	{
		printf("%c", data[i]);
		if((i + 1) % 4 == 0)
		{
			printf(" ");
		}
	}
	printf("\n");

	free(data);

	return 0 ;
}
