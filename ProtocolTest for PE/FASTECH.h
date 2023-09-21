/** 
 * @file FASTECH.h
 * @date 2023/7/17~2023/8/11
 * @author Minho Shin (smh9800@g.skku.edu)
 * @version 0.0.0.1
 * @brief Fastech 모터드라이브 동작관련 함수
 * @details  
 * 리눅스 상의 모터드라이브 동작 관련 동적/정적 라이브러리(SO)를 생성하기 전에 프로그램을 작성하여 함수 원형만 존재함
 * 현재는 전역변수인 buffer의 값들만 수정하는 정도
 * 프로토콜 수준으로 명령어를 생생하여 전송하는 방식으로 프로그램을 완성하거나,
 * 추후에 모션라이브러리를 완성 후 GUI코드와 연동을 하는 식으로 개발하여야 할 것으로 보임
 */

#ifndef FASTECH_H  // 중복 포함 방지를 위한 전처리기 지시문
#define FASTECH_H

#include "library.h"

extern int client_socket;
extern struct sockaddr_in server_addr;

/** 
* @param header, sync_no, frame_type 파스텍 프로토콜에 header, sync_no, frame_type 부분
* @param data[DATA_SIZE] 파스텍 프로토콜에 data부분 
* @return  buffer[BUFFER_SIZE] 임시로 FRAME구조를 생성해서 함수들에서 돌려쓰는 변수
*/
extern BYTE header, sync_no, frame_type;
extern BYTE data[DATA_SIZE];
extern BYTE buffer[BUFFER_SIZE];

/************************************************************************************************************************************
 ****************************************************** Interface 함수 ***************************************************************
 ************************************************************************************************************************************/
char *library_interface();
char* command_interface();
char* FMM_interface(FMM_ERROR error);

/************************************************************************************************************************************
 ******************************************************* FASTECH 관련 함수 ************************************************************
 ************************************************************************************************************************************/

bool FAS_Connect(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID);
bool FAS_ConnectTCP(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID);

void FAS_Close(int iBdID);

int FAS_ServoEnable(int iBdID, bool bOnOff);
int FAS_MoveOriginSingleAxis(int iBdID);
int FAS_MoveStop(int iBdID);
int FAS_MoveVelocity(int iBdID, DWORD IVelocity, int iVelDir);
int FAS_GetboardInfo(int iBdID, BYTE pType, LPSTR LpBuff, int nBuffSize);
int FAS_GetMotorInfo(int iBdID, BYTE pType, LPSTR LpBuff, int nBuffSize);
int FAS_GetEncoder(int iBdID, BYTE pType, LPSTR LpBuff, int nBuffSize);
int FAS_GetFirmwareInfo(int iBdID, BYTE pType, LPSTR LpBuff, int nBuffSize);
int FAS_GetSlaveInfoEx(int iBdID, BYTE pType, LPSTR LpBuff, int nBuffSize);
int FAS_SaveAllParameters(int iBdID);
int FAS_ServoAlarmReset(int iBdID);
int FAS_EmergencyStop(int iBdID);
int FAS_GetAlarmType(int iBdID);
int FAS_GetAllStatus(int iBdID, DWORD* dwInStatus,DWORD* dwOutStatus,DWORD* dwAxisStatus,long* ICmdPos,long* IActPos,long* IPosErr,long* IActVel,WORD* wPosItemNo);

/*마지막날 추가한 포지션 테이블 테스트용 임시함수*/
void FAS_PosTableWriteitem();
void FAS_PosTableReaditem();
void FAS_PosTableReadROM();
void FAS_PosTableWriteROM();
void FAS_PosTableRunItem();
#endif
