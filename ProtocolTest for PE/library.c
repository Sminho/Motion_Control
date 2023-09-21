#include "library.h"

/** 
 * @file library.c
 * @date 2023/7/17~2023/8/11
 * @author Minho Shin (smh9800@g.skku.edu)
 * @version 0.0.0.1
 * @brief 간단한 유틸성 함수 모음
 */

 /** 
  * @brief 프로그램 상에서 시간이 필요할 때
  * @return 00:00:00 양식의 시간 문자열
  */
char* get_time() {
    time_t currentTime;
    struct tm* timeInfo;
    /* "hh:mm:ss" + null 문자 */ 
    char* timeString = (char*)malloc(9); 

    if (timeString == NULL) {
        printf("메모리 할당 오류\n");
        return NULL;
    }
    /* "시스템의 현재 시간 가져오기 */ 
    currentTime = time(NULL);
    /* 현재 시간을 localtime 함수를 이용하여 시간 구조체에 저장 */
    timeInfo = localtime(&currentTime);

    /* 시간:분:초 형식으로 문자열로 변환 */ 
    strftime(timeString, 9, "%H:%M:%S", timeInfo);
    return timeString;
}

/** @brief 명령전달에 쓰는 버퍼 내용을 터미널에 일단 보여주는 함수-> 아마 프로그램이 완성되면 필요없을듯*/
 void print_buffer(uint8_t *array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", array[i]);
    }
    printf("\n");
}

/** @brief 배열을 문자열로 변환하는 함수 */
char* array_to_string(const uint8_t *array, int size) {
    /* 첫 번째 요소는 따로 처리 */ 
    char *str = g_strdup_printf("%02X", array[0]); 

    char *temp;
    /* 나머지 처리 */
    for (int i = 1; i < size; i++) { 
        temp = g_strdup_printf("%s %02X", str, array[i]);
        g_free(str);
        str = temp;
    }

    return str;
}
