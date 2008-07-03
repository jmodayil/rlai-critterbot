/*
 *  armio.c
 *
 *  Created by Michael Sokolsky on 6/28/08.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "./armio.h"

main(int argc, char *argv[]) {
	
	char *msg="This is the end, beautiful friend";
	char character = 'n';
	int temp;
	unsigned int temp2;
	
	if (argc != 3)
		return 1;
	
	sscanf(argv[1], "%d", &temp);
	sscanf(argv[2], "%u", &temp2);
	printf("second arg: %u\n", temp2);
	
	armprintf("Signed:   %d\n", temp);
	armprintf("Unsigned: %u\n", temp);
	armprintf("Hex:      %x\n", temp);
	armprintf("Signed:   %d\n", temp2);
	armprintf("Unsigned: %u\n", temp2);
	armprintf("Hex:      %x\n", temp2);
	return 0;
}

void armprintf(char *format, ...) {
	
	va_list ap;
	char buf[MAX_INT_PRINT_SIZE];
	char *p, *sval;
	int ival;
	unsigned int uival;
	
	va_start(ap, format);
	for(p = format; *p; p++) {
		if(*p != '%') {
			putchar(*p);
			continue;
		}
		switch(*++p) {
			case 'u':
				uival = va_arg(ap, unsigned int);
				itoa(uival, buf, 10, UNSIGNED);
				for(sval = buf; *sval; sval++)
					putchar(*sval);
				break;
			case 'd':
				ival = va_arg(ap, int);
				itoa(ival, buf, 10, SIGNED);
				for(sval = buf; *sval; sval++)
					putchar(*sval);
				break;
			case 'p':
			case 'x':
			case 'X':
				uival = va_arg(ap, unsigned int);
				itoa(uival, buf, 16, UNSIGNED);
				for(sval = buf; *sval; sval++)
					putchar(*sval);
				break;
			case 'o':
				uival = va_arg(ap, unsigned int);
				itoa(uival, buf, 8, UNSIGNED);
				for(sval = buf; *sval; sval++)
					putchar(*sval);
				break;
			case 'b':
			case 'B':
				uival = va_arg(ap, unsigned int);
				itoa(uival, buf, 2, UNSIGNED);
				for(sval = buf; *sval; sval++)
					putchar(*sval);
				break;
			case 'c':
				*sval = (char)va_arg(ap, int);
				putchar(*sval);
				break;
			case 's':
				for(sval = va_arg(ap, char *); *sval; sval++)
					putchar(*sval);
				break;
			default:
				putchar(*p);
				break;
		}
	}
	
}

void itoa(int val, char str[], int base, int valsigned) {
	
	int i, sign;
	unsigned int tmp, uval;
	
	if(base > 36 || base < 2) {
		str[0] = '\0';
		return;
	}
	
	if((sign = val) < 0 && valsigned)
		uval = -val;
	else
		uval = val;
	
	i = 0;
	do {
		tmp = uval % base;
		str[i++] = tmp + (tmp > 9 ? 'A' - 10 : '0');
	} while((uval /= base) > 0);
	
	if(sign < 0 && valsigned)
		str[i++] = '-';
	str[i] = '\0';
	strrev(str);
}

void strrev(char *str) {
	
	char *tail, tmp;
	
	tail = str + strlen(str);
	while( tail > str ) {
 		tmp = *--tail;
		*tail = *str;
		*str++ = tmp;
	}
	
}
