/** 
 * @file FASTECH.c
 * @date 2023/7/17~2023/8/11
 * @author Minho Shin (smh9800@g.skku.edu)
 * @version 0.0.0.1
 * @brief Fastech 모터드라이브 동작관련 함수
 * @details  
 * 리눅스 상의 모터드라이브 동작 관련 동적/정적 라이브러리(SO)를 생성하기 전에 프로그램을 작성하여 함수 원형만 존재함
 * 현재는 전역변수인 buffer의 값들만 수정하는 정도
 * 프로토콜 수준으로 명령어를 생생하여 전송하는 방식으로 프로그램을 완성하거나,
 * 추후에 모션라이브러리를 완성 후 GUI코드와 연동을 하는 식으로 개발하여야 할 것으로 보임
 * 각 모션라이브러리를 호출하려고 만든 인터페이스 함수들은 Switch문 말고 알고리즘을 고민해야할듯
 */

#include "FASTECH.h"

/** 
* @param header, sync_no, frame_type 파스텍 프로토콜에 header, sync_no, frame_type 부분
* @param data[DATA_SIZE] 파스텍 프로토콜에 data부분 
* @return  buffer[BUFFER_SIZE] 임시로 FRAME구조를 생성해서 함수들에서 돌려쓰는 변수
*/
extern BYTE header, sync_no, frame_type;
extern BYTE data[DATA_SIZE];
extern BYTE buffer[BUFFER_SIZE];

/*************************************************************************************************************************************
 ******************************************************* FASTECH 관련 함수 ************************************************************
 *************************************************************************************************************************************/

 /** 
  * @brief UDP 연결 시 사용
  * @param BYTE sb1,sb2,sb3,sb4 IPv4주소 입력 시 각 자리
  * @param int iBdID 드라이브 ID
  * @return boolean 성공시 TRUE 실패시 FALSE
  */
bool FAS_Connect(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID){
    /* 최대 길이 가정 "xxx.xxx.xxx.xxx\0" */
    char SERVER_IP[16]; 
    /* 문자열로 바로 IP를 넣어도 되는데, 기존 파스텍 함수의 파라미터 대로 함수를 만들어서 추가한 부분 */
    snprintf(SERVER_IP, sizeof(SERVER_IP), "%u.%u.%u.%u", sb1, sb2, sb3, sb4);
    
    /* Create socket */ 
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed\n");
        close(client_socket);
        return false;
    }
    else{
        g_print("Socket created\n");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    
    /* Configure server address */ 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_UDP);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported\n");
        close(client_socket);
        return false;
    }
    else{
        g_print("Valid address\n");
    }
    return true;
}

 /** 
  * @brief TCP 연결 시 사용
  * @param BYTE sb1,sb2,sb3,sb4 IPv4주소 입력 시 각 자리
  * @param int iBdID 드라이브 ID
  * @return boolean 성공시 TRUE 실패시 FALSE 
  */
bool FAS_ConnectTCP(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4, int iBdID){
    /* 최대 길이 가정 "xxx.xxx.xxx.xxx\0" */
     char SERVER_IP[16];
      /* 문자열로 바로 IP를 넣어도 되는데, 기존 파스텍 함수의 파라미터 대로 함수를 만들어서 추가한 부분 */
    snprintf(SERVER_IP, sizeof(SERVER_IP), "%u.%u.%u.%u", sb1, sb2, sb3, sb4);

    /* Create socket */ 
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        close(client_socket);
        return false;
    } else {
        g_print("Socket created\n");
    }

    /* Prepare the sockaddr_in structure */ 
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT_TCP);

    /* 타임아웃을 2초로 잡아둠 연결을 2초 안에 실패하면 핸들러 호출 */ 
    alarm(TIMEOUT_SECONDS);

    /* Connect to the server (blocking call, but limited by the timer) */ 
    int result = connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    /* 여기까지 왔으면 성공했으니 알람삭제 */ 
    alarm(0);

    if (result == -1) {
        perror("Connection failed");
        close(client_socket);
        return false;
    } else {
        g_print("Connection Success\n");
    }

    return true;
}

 /**
  * @brief 연결 해제 시 사용
  * @param int iBdID 드라이브 ID 
  */
void FAS_Close(int iBdID){
    close(client_socket);
}

 /**
  * @brief 해당보드의 정보
  * @param int iBdID 드라이브 ID
  * @param BYTE pType 모터의 Type
  * @param LPSTR LpBuff Motor정보를 받을 문자열
  * @param int nBuffSize 버퍼의 사이즈 
  */
int FAS_GetboardInfo(int iBdID, BYTE pType, LPSTR LpBuff, int nBuffSize){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}

 /**
  * @brief 해당모터의 정보
  * @param int iBdID 드라이브 ID
  * @param BYTE pType 모터의 Type
  * @param LPSTR LpBuff Motor정보를 받을 문자열
  * @param int nBuffSize 버퍼의 사이즈
  */
int FAS_GetMotorInfo(int iBdID, BYTE pType, LPSTR LpBuff, int nBuffSize){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}

 /**
  * @brief 해당엔코더의 정보
  * @param int iBdID 드라이브 ID
  * @param BYTE pType 모터의 Type
  * @param LPSTR LpBuff Motor정보를 받을 문자열
  * @param int nBuffSize 버퍼의 사이즈 
  */
int FAS_GetEncoder(int iBdID, BYTE pType, LPSTR LpBuff, int nBuffSize){
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}

 /**
  * @brief 펌웨어의 정보
  * @param int iBdID 드라이브 ID
  * @param BYTE pType 모터의 Type
  * @param LPSTR LpBuff Motor정보를 받을 문자열
  * @param int nBuffSize 버퍼의 사이즈 
  */
int FAS_GetFirmwareInfo(int iBdID, BYTE pType, LPSTR LpBuff, int nBuffSize){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}

 /**
  * @brief 해당보드의 정보
  * @param int iBdID 드라이브 ID
  * @param BYTE pType 모터의 Type
  * @param LPSTR LpBuff Motor정보를 받을 문자열
  * @param int nBuffSize 버퍼의 사이즈 
  */
int FAS_GetSlaveInfoEx(int iBdID, BYTE pType, LPSTR LpBuff, int nBuffSize){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}

 /**
  * @brief 현재까지 수정된 파라미터 값고 입출력 신호를 ROM영역에 저장
  * @param int iBdID 드라이브 ID
  */
int FAS_SaveAllParameters(int iBdID){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}

 /**
  * @brief 비상정지
  * @param int iBdID 드라이브 ID
  * @return 명령이 수행된 정보
  */
int FAS_EmergencyStop(int iBdID){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}

 /**
  * @brief Servo의 상태를 ON/OFF
  * @param int iBdID 드라이브 ID 
  * @param bool bOnOff Enable/Disable
  * @return 명령이 수행된 정보
  */
int FAS_ServoEnable(int iBdID, bool bOnOff){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x04; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type; buffer[5] = data[0];
    return 0;
}

 /**
  * @brief Alarm Reset명령 보냄
  * @param int iBdID 드라이브 ID
  */
int FAS_ServoAlarmReset(int iBdID){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}

 /**
  * @brief Alarm 정보 요청
  * @param int iBdID 드라이브 ID
  */
int FAS_GetAlarmType(int iBdID){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}
 /**
  * @brief Servo를 천천히 멈추는 기능
  * @param int iBdID 드라이브 ID
  * @return 명령이 수행된 정보
  */
int FAS_MoveStop(int iBdID){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}

 /**
  * @brief 시스템의 원점을 찾는 기능?
  * @param int iBdID 드라이브 ID
  * @return 명령이 수행된 정보
  */
int FAS_MoveOriginSingleAxis(int iBdID){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}

 /**
  * @brief Jog 운전 시작을 요청
  * @param int iBdID 드라이브 ID
  * @param DWORD lVelocity 이동 시 속도 값 (pps)
  * @param int iVelDir 이동할 방향 (0:-Jog, 1:+Jog)
  * @return 명령이 수행된 정보
  */
int FAS_MoveVelocity(int iBdID, DWORD lVelocity, int iVelDir) {
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x08; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    memcpy(&buffer[5], data, sizeof(data));
    memset(&data, 0, sizeof(data));
    return 0;
}

 /**
  * @brief 포지션 테이블에서 사용하는 함수들, 작성일자가 마지막날이라 완벽하게 구현은 못함
  */
void FAS_PosTableWriteitem() {
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x45; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    memcpy(&buffer[5], data, sizeof(data));
}

void FAS_PosTableReaditem() {
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x05; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    memcpy(&buffer[5], data, sizeof(data));
}

void FAS_PosTableReadROM(){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
}

void FAS_PosTableWriteROM(){
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
}
void FAS_PosTableRunItem() {
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x05; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    memcpy(&buffer[5], data, sizeof(data));
}


 /**
  * @brief 모든 정보 출력
  * @return 명령이 수행된 정보
  */
int FAS_GetAllStatus(int iBdID, DWORD* dwInStatus,DWORD* dwOutStatus,DWORD* dwAxisStatus,long* ICmdPos,long* IActPos,long* IPosErr,long* IActVel,WORD* wPosItemNo) {
    memset(&buffer, 0, sizeof(buffer));
    buffer[0] = header; buffer[1] = 0x03; buffer[2] = sync_no; buffer[3] = 0x00; buffer[4] = frame_type;
    return 0;
}
/************************************************************************************************************************************
 ****************************************************** Interface 함수 ***************************************************************
 ************************************************************************************************************************************/

 /** @brief 함수들을 찾아가게하는 인터페이스 용도 함수 */
char *library_interface(){
    switch(frame_type)
    {
        case 0x01:
            FAS_GetboardInfo(0, 0, NULL, 0);
            break;
        case 0x05:
            FAS_GetMotorInfo(0, 0, NULL, 0);
            break;
        case 0x06:
            FAS_GetEncoder(0, 0, NULL, 0);
            break;
        case 0x07:
            FAS_GetFirmwareInfo(0, 0, NULL, 0);
            break;
        case 0x09:
            FAS_ServoEnable(0, 0);
            break;
        case 0x10:
            FAS_SaveAllParameters(0);
            break;
        case 0x2A:
            FAS_ServoEnable(0, 0);
            break;
        case 0x2B:
            FAS_ServoAlarmReset(0);
            break;
        case 0x2E:
            FAS_GetAlarmType(0);
            break;
        case 0x31:
            FAS_MoveStop(0);
            break;
        case 0x32:
            FAS_EmergencyStop(0);
            break;
        case 0x33:
            FAS_MoveOriginSingleAxis(0);
            break;
        case 0x37:
            FAS_MoveVelocity(0, 0, 0);
            break;
        case 0x60:
            FAS_PosTableReaditem();
            break;
        case 0x61:
            FAS_PosTableWriteitem();
            break;
        case 0x62:
            FAS_PosTableReadROM();
            break;
        case 0x63:
            FAS_PosTableWriteROM();
            break;
        case 0x64:
            FAS_PosTableRunItem();
            break;
    }
    size_t data_size = buffer[1] + 2;
    print_buffer(buffer, data_size);
    
    char *text = array_to_string(buffer, buffer[1] + 2);
    return text;
}

 /** @brief 각 명령어의 함수 이름을 찾아가는 인터페이스 용도 함수 */
char *command_interface(){
    switch(frame_type)
    {
        case 0x01:
            return"FAS_GetboardInfo";
        case 0x05:
            return "FAS_GetMotorInfo";
        case 0x06:
            return "FAS_GetEncoder";
        case 0x07:
            return "FAS_GetFirmwareInfo";
        case 0x09:
            return "FAS_ServoEnable";
        case 0x10:
            return "FAS_SaveAllParameters";
        case 0x2A:
            return "FAS_ServoEnable";
        case 0x2B:
            return "FAS_ServoAlarmReset";
        case 0x2E:
            return "FAS_GetAlarmType";
        case 0x31:
            return "FAS_MoveStop";
        case 0x32:
            return "FAS_EmergencyStop";
        case 0x33:
            return "FAS_MoveOriginSingleAxis";
        case 0x37:
            return "FAS_MoveVelocity";
        case 0x60:
            return "FAS_PosTableReaditem";
        case 0x61:
            return "FAS_PosTableWriteitem";
        case 0x62:
            return "FAS_PosTableReadROM";
        case 0x63:
            return "FAS_PosTableWriteROM";
        case 0x64:
            return "FAS_PosTableRunItem";
        default:
            return "Undefined Command";
    }
}

 /**
  * @brief 통신상태에서 출력할 내용을 찾아가는 인터페이스 용도 함수
  * @param error FMM_ERROR이 자료형은 Return_code_define.h에 있음 
  */
char* FMM_interface(FMM_ERROR error) {
    switch (error) {
        case FMM_OK:
            return "FMM_OK";
        case FMM_NOT_OPEN:
            return "FMM_NOT_OPEN";
        case FMM_INVALID_PORT_NUM:
            return "FMM_INVALID_PORT_NUM";
        case FMM_INVALID_SLAVE_NUM:
            return "FMM_INVALID_SLAVE_NUM";
        case FMC_DISCONNECTED:
            return "FMC_DISCONNECTED";
        case FMC_TIMEOUT_ERROR:
            return "FMC_TIMEOUT_ERROR";
        case FMC_CRCFAILED_ERROR:
            return "FMC_CRCFAILED_ERROR";
        case FMC_RECVPACKET_ERROR:
            return "FMC_RECVPACKET_ERROR";
        case FMM_POSTABLE_ERROR:
            return "FMM_POSTABLE_ERROR";
        case FMP_FRAMETYPEERROR:
            return "FMP_FRAMETYPEERROR";
        case FMP_DATAERROR:
            return "FMP_DATAERROR";
        case FMP_PACKETERROR:
            return "FMP_PACKETERROR";
        case FMP_RUNFAIL:
            return "FMP_RUNFAIL";
        case FMP_RESETFAIL:
            return "FMP_RESETFAIL";
        case FMP_SERVOONFAIL1:
            return "FMP_SERVOONFAIL1";
        case FMP_SERVOONFAIL2:
            return "FMP_SERVOONFAIL2";
        case FMP_SERVOONFAIL3:
            return "FMP_SERVOONFAIL3";
        case FMP_SERVOOFF_FAIL:
            return "FMP_SERVOOFF_FAIL";
        case FMP_ROMACCESS:
            return "FMP_ROMACCESS";
        case FMP_PACKETCRCERROR:
            return "FMP_PACKETCRCERROR";
        case FMM_UNKNOWN_ERROR:
            return "FMM_UNKNOWN_ERROR";
        default:
            return "Unknown error";
    }
}
