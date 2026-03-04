#ifndef _IO_H_
#define _IO_H_

extern unsigned char insb(unsigned short port);
extern unsigned short insw(unsigned short port);

extern void outb(unsigned short port, unsigned char val);
extern void outw(unsigned short port, unsigned short val);

#endif