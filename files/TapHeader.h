//
// Created by aglebov on 11/20/2019.
//

#ifndef QZXSQUALPEL_TAPHEADER_H
#define QZXSQUALPEL_TAPHEADER_H

struct TapHeader {
  uint8_t flag;
  uint8_t type;
  char name[10];
  uint16_t length;
  uint16_t param1;
  uint16_t param2;
  uint8_t crc;
};

#endif //QZXSQUALPEL_TAPHEADER_H
