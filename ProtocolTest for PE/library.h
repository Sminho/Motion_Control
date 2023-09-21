#ifndef LIBRARY_H  // 중복 포함 방지를 위한 전처리기 지시문
#define LIBRARY_H

/** 
 * @file library.h
 * @date 2023/7/17~2023/8/11
 * @author Minho Shin (smh9800@g.skku.edu)
 * @version 0.0.0.1
 * @brief 간단한 유틸성 함수 모음
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "ReturnCodes_Define.h"
#include "MOTION_EziSERVO2_DEFINE.h"
#include "MOTION_DEFINE.h"

/* BYTE 자료형이 C++에 존재하거나 파스텍 기존 라이브러리에 존재할 것으로 생각하는데, 일단 uint8_t로 해둠 */
typedef uint8_t BYTE;
typedef long LONG;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef char* LPSTR;

#define BUFFER_SIZE 258
#define DATA_SIZE 253
#define PORT_UDP 3001 //UDP GUI
#define PORT_TCP 2001 //TCP GUI
#define TIMEOUT_SECONDS 2

void print_buffer(uint8_t *array, size_t size);
char* array_to_string(const unsigned char *array, int size);
char* get_time();

#endif
