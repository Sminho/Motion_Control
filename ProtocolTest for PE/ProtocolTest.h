/** 
 * @file ProtocolTest.h
 * @date 2023/7/17~2023/8/11
 * @author Minho Shin (smh9800@g.skku.edu)
 * @version 0.0.0.1
 * @brief Fastech 프로그램의 Protocol Test 구현을 위한 프로그램 헤더
 * @details library.h에 FASTECH c/h파일과 공유하는 요소들 선언해둠
 */

#ifndef PROTOCOLTEST_H  // 중복 포함 방지를 위한 전처리기 지시문
#define PROTOCOLTEST_H

#include "library.h"

int client_socket;
struct sockaddr_in server_addr;

BYTE header, sync_no, frame_type;
BYTE data[DATA_SIZE];
BYTE buffer[BUFFER_SIZE];

char *protocol;
bool show_send = TRUE;
bool show_recv = TRUE;


/************************************************************************************************************************************
 ********************************GUI 프로그램의 버튼 등 구성요소들에서 사용하는 callback함수*************************************************
 ************************************************************************************************************************************/

void toggle_fullscreen(GtkWidget *widget, gpointer data);

static void on_button_connect_clicked(GtkButton *button, gpointer user_data);
static void on_button_ok_clicked(GtkButton *button, gpointer user_data);
static void on_button_clear_clicked(GtkButton *button, gpointer user_data);
static void on_button_send_clicked(GtkButton *button, gpointer user_data);
static void on_button_statusmonitor_clicked(GtkButton *button, gpointer user_data);

static void on_button_record1_clicked(GtkButton *button, gpointer user_data);
static void on_button_record2_clicked(GtkButton *button, gpointer user_data);
static void on_button_record3_clicked(GtkButton *button, gpointer user_data);
static void on_button_record4_clicked(GtkButton *button, gpointer user_data);

static void on_button_transfer1_clicked(GtkButton *button, gpointer user_data);
static void on_button_transfer2_clicked(GtkButton *button, gpointer user_data);
static void on_button_transfer3_clicked(GtkButton *button, gpointer user_data);
static void on_button_transfer4_clicked(GtkButton *button, gpointer user_data);

static void on_combo_protocol_changed(GtkComboBoxText *combo_text, gpointer user_data);
static void on_combo_command_changed(GtkComboBox *combo_id, gpointer user_data);
static void on_combo_data1_changed(GtkComboBox *combo_id, gpointer user_data);
static void on_combo_direction_changed(GtkComboBox *combo_id, gpointer user_data);

static void on_check_autosync_toggled(GtkToggleButton *togglebutton, gpointer user_data);
static void on_check_fastech_toggled(GtkToggleButton *togglebutton, gpointer user_data);
static void on_check_showsend_toggled(GtkToggleButton *togglebutton, gpointer user_data);

gboolean on_text_frame_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_status_monitor_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
gboolean status_monitor_update(gpointer user_data);

static void on_button_pt_clicked(GtkButton *button, gpointer user_data);
static void on_button_pt_ok_clicked(GtkButton *button, gpointer user_data);
gboolean on_position_table_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
gboolean on_entry_pt_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

/************************************************************************************************************************************
 ******************************************************* 편의상 만든 함수 **************************************************************
 ************************************************************************************************************************************/
 
void sync_no_update();
void packet_update();

void handle_alarm(int signum);
BYTE* send_packet(BYTE *send_frame, int byte_count);
BYTE* send_packetTCP(BYTE *send_frame, int byte_count);

/************************************************************************************************************************************
 ***************************************************** 전역변수인 GUI요소 **************************************************************
 ************************************************************************************************************************************/

GtkWidget *text_sendbuffer;
GtkWidget *text_monitor1;
GtkWidget *text_monitor2;
GtkWidget *text_autosync;
GtkWidget *text_frame;
GtkWidget *record_command1;
GtkWidget *record_command2;
GtkWidget *record_command3;
GtkWidget *record_command4;

GtkWidget *text_axis_status;

GtkWidget *text_input;
GtkWidget *text_output;
GtkWidget *text_command_pos;
GtkWidget *text_actual_pos;
GtkWidget *text_position;
GtkWidget *text_velocity;
GtkWidget *text_table;
GtkWidget *text_status;

GtkWidget *text_lPosition;
GtkWidget *text_wAccelRate;
GtkWidget *text_wWaitTime;
GtkWidget *text_wCond_branch0;
GtkWidget *text_wLoopCount;
GtkWidget *text_wLoopCountCLR;
GtkWidget *text_wTriggerOnTime;
GtkWidget *text_lPushPosition;
GtkWidget *text_dwStartSpd;
GtkWidget *text_wDecelRate;
GtkWidget *text_wContinuous;
GtkWidget *text_wCond_branch1;
GtkWidget *text_wBranchAfterLoop;
GtkWidget *text_bCheckInpos;
GtkWidget *text_wPushRatio;
GtkWidget *text_wPushMode;
GtkWidget *text_dwMoveSpd;
GtkWidget *text_wCommand;
GtkWidget *text_wBranch;
GtkWidget *text_wCond_branch2;
GtkWidget *text_wPTSet;
GtkWidget *text_lTriggerPos;
GtkWidget *text_dwPushSpeed;
GtkWidget *text_pt_frame;

GtkTextBuffer *sendbuffer_buffer;
GtkTextBuffer *monitor1_buffer;
GtkTextBuffer *monitor2_buffer;
GtkTextBuffer *autosync_buffer;
GtkTextBuffer *frame_buffer;
GtkTextBuffer *record1_buffer;
GtkTextBuffer *record2_buffer;
GtkTextBuffer *record3_buffer;
GtkTextBuffer *record4_buffer;

GtkTextBuffer *axis_status_buffer;

GtkTextBuffer *input_buffer;
GtkTextBuffer *output_buffer;
GtkTextBuffer *command_pos_buffer;
GtkTextBuffer *actual_pos_buffer;
GtkTextBuffer *position_buffer;
GtkTextBuffer *velocity_buffer;
GtkTextBuffer *table_buffer;
GtkTextBuffer *status_buffer;

GtkTextBuffer *lPosition_buffer;
GtkTextBuffer *wAccelRate_buffer;
GtkTextBuffer *wWaitTime_buffer;
GtkTextBuffer *wCond_branch0_buffer;
GtkTextBuffer *wLoopCount_buffer;
GtkTextBuffer *wLoopCountCLR_buffer;
GtkTextBuffer *wTriggerOnTime_buffer;
GtkTextBuffer *lPushPosition_buffer;
GtkTextBuffer *dwStartSpd_buffer;
GtkTextBuffer *wDecelRate_buffer;
GtkTextBuffer *wContinuous_buffer;
GtkTextBuffer *wCond_branch1_buffer;
GtkTextBuffer *wBranchAfterLoop_buffer;
GtkTextBuffer *bCheckInpos_buffer;
GtkTextBuffer *wPushRatio_buffer;
GtkTextBuffer *wPushMode_buffer;
GtkTextBuffer *dwMoveSpd_buffer;
GtkTextBuffer *wCommand_buffer;
GtkTextBuffer *wBranch_buffer;
GtkTextBuffer *wCond_branch2_buffer;
GtkTextBuffer *wPTSet_buffer;
GtkTextBuffer *lTriggerPos_buffer;
GtkTextBuffer *dwPushSpeed_buffer;
GtkTextBuffer *pt_frame_buffer;


GtkImage *img_error_all;
GtkImage *img_hw_plus;
GtkImage *img_hw_minus;
GtkImage *img_sw_plus;
GtkImage *img_sw_minus;
GtkImage *img_reserved;
GtkImage *img_pos_count;
GtkImage *img_err_servo;
GtkImage *img_err_current;
GtkImage *img_err_speed;
GtkImage *img_err_out;
GtkImage *img_err_load;
GtkImage *img_err_heat;
GtkImage *img_err_back_emf;
GtkImage *img_err_power;
GtkImage *img_err_inpositin;
GtkImage *img_emg_stop;
GtkImage *img_slow_stop;
GtkImage *img_org_return;
GtkImage *img_inposition;
GtkImage *img_servo_on;
GtkImage *img_alarm_reset;
GtkImage *img_pt_stopped;
GtkImage *img_origin_sensor;
GtkImage *img_z_pulse;
GtkImage *img_org_ret;
GtkImage *img_motion_dir;
GtkImage *img_motioning;
GtkImage *img_motion_pause;
GtkImage *img_motion_accel;
GtkImage *img_motion_decel;
GtkImage *img_motion_constant;

GtkLabel *label_status;
GtkLabel *label_time; 

GtkEntry *entry_interval;
GtkEntry *entry_pt;

guint repeat_timer = 0; 

#endif
