#ifndef UART_UTILS_H
# define UART_UTILS_H

void    uart_init(void);
void    uart_tx(char c);
void    uart_printstr(const char *str);

#endif
