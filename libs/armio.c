/*
 *  armio.c
 *
 *  Created by Michael Sokolsky on 6/28/08.
 *
 */

#include "./armio.h"

int armsscanf(char *source, char *format, ...) {

  va_list ap;
  int pnum, ret;
  char *sval;
  int *ival;
  unsigned int *uival;
  
  va_start(ap, format);
  pnum = 0;
  while(*format != '\0') {
    while(*source == ' ' || *source == '\t' || *source == '\n')
      source++;
    if(*source == '\0')
      return pnum;
    if(*format == ' ' || *format == '\t' || *format == '\n') {
      format++;
      continue;
    }
    if(*format != '%') {
      if(*format != *source)
        return pnum;
      format++;
      source++;
      continue;
    }
    switch(*++format) {
      case 'd':
        ival = va_arg(ap,int *); 
        if(ret = armatoi(source, ival))
          pnum++;
        else
          return pnum;
        source += ret;
        break;
      case 'u':
        uival = va_arg(ap,unsigned int *);
        if(ret = armatoi(source, (int*)uival))
          pnum++;
        else
          return pnum;
        source += ret;
        break;
      case 'c':
        sval = va_arg(ap,char *);
        *sval = *source++;
        pnum++;
        break;
      case 's':
        sval = va_arg(ap,char *);
        while(*source != ' ' && *source != '\n' && *source !='\t' && *source !='\0')
          *sval++ = *source++;
        *sval = '\0';
        pnum++;
        break;
      case '\0':
        return pnum;
      default:
        if(*format != *source)
          return pnum;
        break;
    }
    format++;
  }  
  return pnum;
}

int armatoi(char str[], int *val) {
  
  int base, ival, digit, sign, i;
  
  i = 0;
  sign = (str[i] == '-') ? -1 : 1;
  if(str[i] == '+' || str[i] == '-') {
    str++;
  }
  while(str[i] == '0')
    i++;
  switch(str[i]) {
    case 'x':
    case 'X':
      base = 16;
      i++;
      break;
    case 'o':
      base = 8;
      i++;
      break;
    case 'b':
      base = 2;
      i++;
      break;
    default:
      base = 10;
      break;
  }
  ival = 0;
  while((digit = getvalue(str[i])) != -1) {
    ival = base * ival + digit;
    i++; 
  }
  *val = sign * ival;
  return sign == 1 ? i : i + 1;
}

__inline int getvalue(char digit) {

  if(digit >= '0' && digit <= '9')
    return digit - '0';
  if(digit >= 'A' && digit <= 'F')
    return digit - 'A' + 10;
  return -1;
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
			armputchar(*p);
			continue;
		}
		switch(*++p) {
			case 'u':
				uival = va_arg(ap, unsigned int);
				armitoa(uival, buf, 10, UNSIGNED);
				for(sval = buf; *sval; sval++)
					armputchar(*sval);
				break;
			case 'd':
				ival = va_arg(ap, int);
				armitoa(ival, buf, 10, SIGNED);
				for(sval = buf; *sval; sval++)
					armputchar(*sval);
				break;
			case 'p':
			case 'x':
			case 'X':
				uival = va_arg(ap, unsigned int);
				armitoa(uival, buf, 16, UNSIGNED);
				for(sval = buf; *sval; sval++)
					armputchar(*sval);
				break;
			case 'o':
				uival = va_arg(ap, unsigned int);
				armitoa(uival, buf, 8, UNSIGNED);
				for(sval = buf; *sval; sval++)
					armputchar(*sval);
				break;
			case 'b':
			case 'B':
				uival = va_arg(ap, unsigned int);
				armitoa(uival, buf, 2, UNSIGNED);
				for(sval = buf; *sval; sval++)
					armputchar(*sval);
				break;
			case 'c':
				*sval = (char)va_arg(ap, int);
				armputchar(*sval);
				break;
			case 's':
				for(sval = va_arg(ap, char *); *sval; sval++)
					armputchar(*sval);
				break;
      case '\0':
        return;
			default:
				armputchar(*p);
				break;
		}
	}
	
}

void armitoa(int val, char str[], int base, int valsigned) {
	
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

void armputchar(char val) {
  
  if(ser_tx_head == ser_tx_buf + SER_TX_BUF_SIZE) 
    return;
  *ser_tx_head++ = val;
  AT91C_BASE_US0->US_TCR++;

  
}

int armgetchar(void) {
  
  char gotchar;
  
  if((unsigned int)ser_rx_head == AT91C_BASE_US0->US_RPR)
    return EOF;
  if(read_loc == ser_rx_head)
    read_loc++;
  gotchar = *ser_rx_head++;
  return gotchar;
}

int armreadline(char *read_to, int max_size) {
  
  int size;
  char *buf_end, *cur_ptr;

  if(read_to == NULL || max_size == 0)
    return EOF;
  
  buf_end = ser_rx_buf + SER_RX_BUF_SIZE;
  cur_ptr = (char*)AT91C_BASE_US0->US_RPR;
  
  if(cur_ptr == ser_rx_head)
    return EOF;
  while(*read_loc != '\n') {
    if(read_loc == cur_ptr)
      return EOF;
    if(++read_loc == buf_end)
      read_loc = ser_rx_buf;
  }
  armputchar('^'); 
  for(size = 0; ser_rx_head != read_loc; size++) {
    if(size == max_size)
      return EOF;
    *read_to++ = *ser_rx_head++;
    if(ser_rx_head == buf_end)
      ser_rx_head = ser_rx_buf;
  }
  *read_to = '\0';
  if(++read_loc == buf_end)
    read_loc = ser_rx_buf;
  armputchar('*');
  return size;
}

RAMFUNC void ser_isr(void) {

  unsigned int status = AT91C_BASE_US0->US_CSR;

  if(status & AT91C_US_ENDRX) {
    AT91C_BASE_US0->US_RNCR = SER_RX_BUF_SIZE;
    AT91C_BASE_US0->US_RNPR = (unsigned int)ser_rx_buf;
  }
  if(status & AT91C_US_TXBUFE) 
    AT91C_BASE_US0->US_TPR = (unsigned int)(ser_tx_head = ser_tx_buf);
  return;
}

void init_serial_port_stdio(void) {
  
  AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, 1 << AT91C_ID_US0);
  AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, SERIAL_A_PINS, SERIAL_B_PINS);
  AT91C_BASE_US0->US_MR = (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE);
  AT91C_BASE_US0->US_BRGR = 13; //SER_BRGR;
  AT91C_BASE_US0->US_CR = (AT91C_US_RXEN | AT91C_US_TXEN | AT91C_US_RSTSTA );
  AT91F_AIC_ConfigureIt(AT91C_BASE_AIC,
                        AT91C_ID_US0,
                        AT91C_AIC_PRIOR_LOWEST,
                        AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE,
                        ser_isr );
  AT91C_BASE_US0->US_IER = AT91C_US_ENDRX | AT91C_US_TXBUFE;
  
  AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_US0);

  AT91C_BASE_US0->US_RPR = (unsigned int)ser_rx_buf;
  read_loc = ser_rx_head = ser_rx_buf;
  AT91C_BASE_US0->US_RCR = SER_RX_BUF_SIZE;
  AT91C_BASE_US0->US_RNPR = (unsigned int)ser_rx_buf;
  AT91C_BASE_US0->US_RNCR = SER_RX_BUF_SIZE;

  AT91C_BASE_US0->US_TPR = (unsigned int)(ser_tx_head = ser_tx_buf);
  AT91C_BASE_US0->US_TNPR = (unsigned int)ser_tx_buf;
  
  AT91C_BASE_US0->US_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;
  
  armprintf("rpr: %p\n", AT91C_BASE_US0->US_RPR);
  
  // Disable Amplifier
  AT91C_BASE_PIOA->PIO_PER = 1 << 8;
  AT91C_BASE_PIOA->PIO_OER = 1 << 8;
  AT91C_BASE_PIOA->PIO_SODR = 1 << 8;
}
