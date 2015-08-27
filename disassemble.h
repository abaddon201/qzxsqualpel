#ifndef DISASSEMBLE_H
#define DISASSEMBLE_H

typedef unsigned char libspectrum_byte;
typedef unsigned short libspectrum_word;

void
debugger_disassemble( char* buffer, size_t buflen, size_t* length,
                      libspectrum_word address );

#endif
