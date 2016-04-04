#include "stdarg.h"
#include "ml_print.h"
#include "lcd.h"

static char buf[12];

void ml_printNatural(uint16_t num, uint16_t base)
{
	/* Add the prefix for octal and hex; exit if not supported base */
	switch(base)
	{
		case 2:
		case 10:
			break;
		case 8:
			display_char('0');
			break;
		case 16:
			display_char('0');
			display_char('x');
			break;
		default:
			return;
	}

	char const digits[] = "0123456789ABCDEF";

	/* We're using a 16b int, so we know that num < 65536 */
	uint32_t div_max = base;
	while(div_max * base < 65536) 
	{
		div_max = div_max * base;
	}
	uint16_t i = (uint16_t) div_max;
	/* Remove leading 0s */
	while(i != 1 && (num / i) % base == 0)
	{
		i = i / base;
	}
	/* Write the number */
	while(i != 1)
	{
		display_char(digits[(num / i) % base]);
		i = i / base;
	}
	display_char(digits[num % base]);
}

void ml_printf(char *str, ...)
{
	va_list ap;
	va_start(ap,str);
	char c;
	while((c = *(str++)))
	{
		if(c != '%')
			display_char(c);
		else
		{
			uint16_t num;
			c = *(str++);
			char sc;
			char *s = buf;
			switch(c)
			{
				case 0:
					break;
				case 'u':
				case 'd':
					num = va_arg(ap, uint16_t);
					if(c == 'd' && (int16_t)num < 0)
					{
						num = -(int16_t)num;
						display_char('-');
					}
					ml_printNatural(num, 10);
					break;
				case 'x':
					num = va_arg(ap, uint16_t);
					ml_printNatural(num, 16);
					break;
				case 'b':
					num = va_arg(ap, uint16_t);
					ml_printNatural(num, 2);
					break;
				case 'o':
					num = va_arg(ap, uint16_t);
					ml_printNatural(num, 8);
					break;
				case 'c':
					display_char((char) va_arg(ap, int));
					break;
				case 's':
					s = va_arg(ap, char*);
					while((sc = *(s++)))
					{
						display_char(sc);
					}
					break;
				case '%':
					display_char('%');
					break;
				default:
					break;
			}
		}
	}
	display_char('\n');
}
