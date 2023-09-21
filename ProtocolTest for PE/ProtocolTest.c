/** 
 * @file ProtocolTest.c
 * @date 2023/7/17~2023/8/11
 * @author Minho Shin (smh9800@g.skku.edu)
 * @version 0.0.0.1
 * @brief Fastech 프로그램의 Protocol Test GUI 구현을 위한 프로그램
 * @details 
 * C언어와 GTK3(라즈비안(=데비안11) 호환을 위해서), GLADE(UI XML->.glade파일) 사용
 * Ethernet 부분(Ezi Servo Plus-E 모델용)의 일부 기능만 구현함->메뉴얼 참고
 * 
 * libary c/h파일에 유틸성 함수, 겹치는 헤더파일/자료형/매크로 등 선언
 * 
 * @warning 예외처리는 2초 내에 명령이 제대로 수행 안될 경우 소켓을 닫는 정도로만 구현해둠, 정해진 기능만 사용할 것
 */

#include "library.h"
#include "ProtocolTest.h"
#include "FASTECH.h"

 /** @brief Main 함수 GUI를 시작하는 역할 */
int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *status_monitor;
    GtkWidget *position_table;
    GObject *button;
    GtkComboBoxText *combo_text;
    GtkComboBox* combo_id;
    GObject *checkbox;
    GError *error = NULL;
    
    srand(time(NULL));
    signal(SIGALRM, handle_alarm);

    header = 0xAA;
    sync_no = (BYTE)(rand() % 256);
    
    /* GTK 초기화 */ 
    gtk_init(&argc, &argv);

    /* GtkBuilder 생성 */ 
    builder = gtk_builder_new();

    /* XML file에서 UI불러오기 */ 
    if (!gtk_builder_add_from_file(builder, "ProtocolTest.glade", &error)) {
        g_printerr("Error loading UI file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    /* Window 생성 */ 
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    GtkWidget *button_toggle = GTK_WIDGET(gtk_builder_get_object(builder, "button_toggle"));
    g_signal_connect(button_toggle, "clicked", G_CALLBACK(toggle_fullscreen), window);
    status_monitor = GTK_WIDGET(gtk_builder_get_object(builder, "status_monitor"));
    position_table = GTK_WIDGET(gtk_builder_get_object(builder, "position_table"));
    gtk_widget_show_all(window);
    
    /* 구성 요소들 생성 & 시그널 연결(-> Glade에서도 되어있긴한데 일단 해둠) */ 
    entry_interval = GTK_ENTRY(gtk_builder_get_object(builder, "entry_interval"));
    entry_pt = GTK_ENTRY(gtk_builder_get_object(builder, "entry_pt"));
    g_signal_connect(entry_pt, "key-release-event", G_CALLBACK(on_entry_pt_key_release_event), NULL);
    
    GtkStack *stk2 = GTK_STACK(gtk_builder_get_object(builder, "stk2"));
    
    text_monitor1 = GTK_WIDGET(gtk_builder_get_object(builder, "text_monitor1"));
    monitor1_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_monitor1));
    text_monitor2 = GTK_WIDGET(gtk_builder_get_object(builder, "text_monitor2"));
    monitor2_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_monitor2));
    text_sendbuffer = GTK_WIDGET(gtk_builder_get_object(builder, "text_sendbuffer"));
    sendbuffer_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_sendbuffer));
    text_autosync = GTK_WIDGET(gtk_builder_get_object(builder, "text_autosync"));
    autosync_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_autosync));
    text_pt_frame = GTK_WIDGET(gtk_builder_get_object(builder, "text_pt_frame"));
    pt_frame_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_pt_frame));
    
    record_command1 = GTK_WIDGET(gtk_builder_get_object(builder, "record_command1"));
    record1_buffer= gtk_text_view_get_buffer(GTK_TEXT_VIEW(record_command1));
    record_command2 = GTK_WIDGET(gtk_builder_get_object(builder, "record_command2"));
    record2_buffer= gtk_text_view_get_buffer(GTK_TEXT_VIEW(record_command2));
    record_command3 = GTK_WIDGET(gtk_builder_get_object(builder, "record_command3"));
    record3_buffer= gtk_text_view_get_buffer(GTK_TEXT_VIEW(record_command3));
    record_command4 = GTK_WIDGET(gtk_builder_get_object(builder, "record_command4"));
    record4_buffer= gtk_text_view_get_buffer(GTK_TEXT_VIEW(record_command4));
    
    text_axis_status = GTK_WIDGET(gtk_builder_get_object(builder, "text_axis_status"));
    axis_status_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_axis_status));
    text_input = GTK_WIDGET(gtk_builder_get_object(builder, "text_input"));
    input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_input));
    text_output = GTK_WIDGET(gtk_builder_get_object(builder, "text_output"));
    output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_output));
    text_command_pos = GTK_WIDGET(gtk_builder_get_object(builder, "text_command_pos"));
    command_pos_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_command_pos));
    text_actual_pos = GTK_WIDGET(gtk_builder_get_object(builder, "text_actual_pos"));
    actual_pos_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_actual_pos));
    text_position = GTK_WIDGET(gtk_builder_get_object(builder, "text_position"));
    position_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_position));
    text_velocity = GTK_WIDGET(gtk_builder_get_object(builder, "text_velocity"));
    velocity_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_velocity));
    text_table = GTK_WIDGET(gtk_builder_get_object(builder, "text_table"));
    table_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_table));
    text_status = GTK_WIDGET(gtk_builder_get_object(builder, "text_status"));
    status_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_status));

    text_lPosition = GTK_WIDGET(gtk_builder_get_object(builder, "text_lPosition"));
    lPosition_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_lPosition));
    text_wAccelRate = GTK_WIDGET(gtk_builder_get_object(builder, "text_wAccelRate"));
    wAccelRate_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wAccelRate));
    text_wWaitTime = GTK_WIDGET(gtk_builder_get_object(builder, "text_wWaitTime"));
    wWaitTime_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wWaitTime));
    text_wCond_branch0 = GTK_WIDGET(gtk_builder_get_object(builder, "text_wCond_branch0"));
    wCond_branch0_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wCond_branch0));
    text_wLoopCount = GTK_WIDGET(gtk_builder_get_object(builder, "text_wLoopCount"));
    wLoopCount_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wLoopCount));
    text_wLoopCountCLR = GTK_WIDGET(gtk_builder_get_object(builder, "text_wLoopCountCLR"));
    wLoopCountCLR_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wLoopCountCLR));
    text_wTriggerOnTime = GTK_WIDGET(gtk_builder_get_object(builder, "text_wTriggerOnTime"));
    wTriggerOnTime_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wTriggerOnTime));
    text_lPushPosition = GTK_WIDGET(gtk_builder_get_object(builder, "text_lPushPosition"));
    lPushPosition_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_lPushPosition));
    text_dwStartSpd = GTK_WIDGET(gtk_builder_get_object(builder, "text_dwStartSpd"));
    dwStartSpd_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_dwStartSpd));
    text_wDecelRate = GTK_WIDGET(gtk_builder_get_object(builder, "text_wDecelRate"));
    wDecelRate_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wDecelRate));
    text_wContinuous = GTK_WIDGET(gtk_builder_get_object(builder, "text_wContinuous"));
    wContinuous_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wContinuous));
    text_wCond_branch1 = GTK_WIDGET(gtk_builder_get_object(builder, "text_wCond_branch1"));
    wCond_branch1_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wCond_branch1));
    text_wBranchAfterLoop = GTK_WIDGET(gtk_builder_get_object(builder, "text_wBranchAfterLoop"));
    wBranchAfterLoop_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wBranchAfterLoop));
    text_bCheckInpos = GTK_WIDGET(gtk_builder_get_object(builder, "text_bCheckInpos"));
    bCheckInpos_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_bCheckInpos));
    text_wPushRatio = GTK_WIDGET(gtk_builder_get_object(builder, "text_wPushRatio"));
    wPushRatio_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wPushRatio));
    text_wPushMode = GTK_WIDGET(gtk_builder_get_object(builder, "text_wPushMode"));
    wPushMode_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wPushMode));
    text_dwMoveSpd = GTK_WIDGET(gtk_builder_get_object(builder, "text_dwMoveSpd"));
    dwMoveSpd_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_dwMoveSpd));
    text_wCommand = GTK_WIDGET(gtk_builder_get_object(builder, "text_wCommand"));
    wCommand_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wCommand));
    text_wBranch = GTK_WIDGET(gtk_builder_get_object(builder, "text_wBranch"));
    wBranch_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wBranch));
    text_wCond_branch2 = GTK_WIDGET(gtk_builder_get_object(builder, "text_wCond_branch2"));
    wCond_branch2_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wCond_branch2));

    text_wPTSet = GTK_WIDGET(gtk_builder_get_object(builder, "text_wPTSet"));
    wPTSet_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_wPTSet));
    text_lTriggerPos = GTK_WIDGET(gtk_builder_get_object(builder, "text_lTriggerPos"));
    lTriggerPos_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_lTriggerPos));
    text_dwPushSpeed = GTK_WIDGET(gtk_builder_get_object(builder, "text_dwPushSpeed"));
    dwPushSpeed_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_dwPushSpeed));
    
    text_frame = GTK_WIDGET(gtk_builder_get_object(builder, "text_frame"));
    frame_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_frame));
    g_signal_connect(text_frame, "key-release-event", G_CALLBACK(on_text_frame_key_release_event), NULL);
    
    label_status = GTK_LABEL(gtk_builder_get_object(builder, "label_status"));
    label_time = GTK_LABEL(gtk_builder_get_object(builder, "label_time"));
    
    button = gtk_builder_get_object(builder, "button_connect");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_connect_clicked), builder);
    button = gtk_builder_get_object(builder, "button_ok");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_ok_clicked), builder);
    button = gtk_builder_get_object(builder, "button_clear");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clear_clicked), builder);
    button = gtk_builder_get_object(builder, "button_send");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_send_clicked), builder);
    button = gtk_builder_get_object(builder, "button_statusmonitor");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_statusmonitor_clicked), status_monitor);
    g_signal_connect(status_monitor, "delete-event", G_CALLBACK(on_status_monitor_delete_event), NULL);
    button = gtk_builder_get_object(builder, "button_pt");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_pt_clicked), position_table);
    g_signal_connect(position_table, "delete-event", G_CALLBACK(on_position_table_delete_event), NULL);
    button = gtk_builder_get_object(builder, "button_pt_ok");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_pt_ok_clicked), builder);
    
    button = gtk_builder_get_object(builder, "button_record1");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_record1_clicked), builder);
    button = gtk_builder_get_object(builder, "button_record2");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_record2_clicked), builder);
    button = gtk_builder_get_object(builder, "button_record3");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_record3_clicked), builder);
    button = gtk_builder_get_object(builder, "button_record4");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_record4_clicked), builder);
    
    button = gtk_builder_get_object(builder, "button_transfer1");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_transfer1_clicked), NULL);
    button = gtk_builder_get_object(builder, "button_transfer2");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_transfer2_clicked), NULL);
    button = gtk_builder_get_object(builder, "button_transfer3");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_transfer3_clicked), NULL);
    button = gtk_builder_get_object(builder, "button_transfer4");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_transfer4_clicked), NULL);
    
    img_error_all = GTK_IMAGE(gtk_builder_get_object(builder, "img_error_all"));
    img_hw_plus = GTK_IMAGE(gtk_builder_get_object(builder, "img_hw_plus"));
    img_hw_minus = GTK_IMAGE(gtk_builder_get_object(builder, "img_hw_minus"));
    img_sw_plus = GTK_IMAGE(gtk_builder_get_object(builder, "img_sw_plus"));
    img_sw_minus = GTK_IMAGE(gtk_builder_get_object(builder, "img_sw_minus"));
    img_reserved = GTK_IMAGE(gtk_builder_get_object(builder, "img_reserved"));
    img_pos_count = GTK_IMAGE(gtk_builder_get_object(builder, "img_pos_count"));
    img_err_servo = GTK_IMAGE(gtk_builder_get_object(builder, "img_err_servo"));
    img_err_current = GTK_IMAGE(gtk_builder_get_object(builder, "img_err_current"));
    img_err_speed = GTK_IMAGE(gtk_builder_get_object(builder, "img_err_speed"));
    img_err_out = GTK_IMAGE(gtk_builder_get_object(builder, "img_err_out"));
    img_err_load = GTK_IMAGE(gtk_builder_get_object(builder, "img_err_load"));
    img_err_heat = GTK_IMAGE(gtk_builder_get_object(builder, "img_err_heat"));
    img_err_back_emf = GTK_IMAGE(gtk_builder_get_object(builder, "img_err_back_emf"));
    img_err_power = GTK_IMAGE(gtk_builder_get_object(builder, "img_err_power"));
    img_err_inpositin = GTK_IMAGE(gtk_builder_get_object(builder, "img_err_inpositin"));
    img_emg_stop = GTK_IMAGE(gtk_builder_get_object(builder, "img_emg_stop"));
    img_slow_stop = GTK_IMAGE(gtk_builder_get_object(builder, "img_slow_stop"));
    img_org_return = GTK_IMAGE(gtk_builder_get_object(builder, "img_org_return"));
    img_inposition = GTK_IMAGE(gtk_builder_get_object(builder, "img_inposition"));
    img_servo_on = GTK_IMAGE(gtk_builder_get_object(builder, "img_servo_on"));
    img_alarm_reset = GTK_IMAGE(gtk_builder_get_object(builder, "img_alarm_reset"));
    img_pt_stopped = GTK_IMAGE(gtk_builder_get_object(builder, "img_pt_stopped"));
    img_origin_sensor = GTK_IMAGE(gtk_builder_get_object(builder, "img_origin_sensor"));
    img_z_pulse = GTK_IMAGE(gtk_builder_get_object(builder, "img_z_pulse"));
    img_org_ret = GTK_IMAGE(gtk_builder_get_object(builder, "img_org_ret"));
    img_motion_dir = GTK_IMAGE(gtk_builder_get_object(builder, "img_motion_dir"));
    img_motioning = GTK_IMAGE(gtk_builder_get_object(builder, "img_motioning"));
    img_motion_pause = GTK_IMAGE(gtk_builder_get_object(builder, "img_motion_pause"));
    img_motion_accel = GTK_IMAGE(gtk_builder_get_object(builder, "img_motion_accel"));
    img_motion_decel = GTK_IMAGE(gtk_builder_get_object(builder, "img_motion_decel"));
    img_motion_constant = GTK_IMAGE(gtk_builder_get_object(builder, "img_motion_constant"));
    
    combo_text = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "combo_protocol"));
    g_signal_connect(combo_text, "changed", G_CALLBACK(on_combo_protocol_changed), NULL);
    
    combo_id = GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_command"));
    g_signal_connect(combo_id, "changed", G_CALLBACK(on_combo_command_changed), builder);
    combo_id = GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_data1"));
    g_signal_connect(combo_id, "changed", G_CALLBACK(on_combo_data1_changed), NULL);
    combo_id = GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_direction"));
    g_signal_connect(combo_id, "changed", G_CALLBACK(on_combo_direction_changed), builder);


    checkbox = gtk_builder_get_object(builder, "check_autosync");
    g_signal_connect(checkbox, "toggled", G_CALLBACK(on_check_autosync_toggled), NULL);
    checkbox = gtk_builder_get_object(builder, "check_fastech");
    g_signal_connect(checkbox, "toggled", G_CALLBACK(on_check_fastech_toggled), NULL);
    checkbox = gtk_builder_get_object(builder, "check_showsend");
    g_signal_connect(checkbox, "toggled", G_CALLBACK(on_check_showsend_toggled), NULL);
    
    /* Sync */ 
    sync_no_update();

    /* GTK 메인 루프 실행 */ 
    gtk_main();

    return 0;
}

/****************************************************************************************************************************************
 ********************************GUI 프로그램의 버튼 등 구성요소들에서 사용하는 callback함수*************************************************
 ****************************************************************************************************************************************/

/** @brief 전체화면<->창모드 버튼의 callback*/
void toggle_fullscreen(GtkWidget *widget, gpointer data) {
    GtkWindow *window = GTK_WINDOW(data);
    GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
    GtkButton *button = GTK_BUTTON(widget);
    gboolean is_fullscreen = (gdk_window_get_state(gdk_window) & GDK_WINDOW_STATE_FULLSCREEN);
    
    if (is_fullscreen) {
        gtk_window_unfullscreen(window);
        gtk_button_set_label(button, "전체화면");
    } else {
        gtk_window_fullscreen(GTK_WINDOW(window));
        gtk_button_set_label(button, "창모드");
    }
    g_print("FullScreen\n");
}
 
/** @brief Connect버튼의 callback*/
static void on_button_connect_clicked(GtkButton *button, gpointer user_data) {
    BYTE sb1, sb2, sb3, sb4;
    
    /* Get the GtkBuilder object passed as user data */ 
    GtkBuilder *builder = GTK_BUILDER(user_data);

    /* Get the label of the button */ 
    const char *label_text = gtk_button_get_label(button);
    GObject *button_send = gtk_builder_get_object(builder, "button_send");
    GObject *button_statusmonitor = gtk_builder_get_object(builder, "button_statusmonitor");

    /* Get the entry widget by its ID */
    GtkEntry *entry_ip = GTK_ENTRY(gtk_builder_get_object(builder, "entry_ip"));

    /* Get the entered text from the entry */ 
    const char *ip_text = gtk_entry_get_text(entry_ip);

    /* IP 검증 */ 
    if (g_strcmp0(ip_text, "") != 0) {
        g_print("IP: %s\n", ip_text);

        /* IP는 문자열로 받아서 분리하고 숫자 파라미터로 바꿔서 보내는데 비효율적이긴 한듯 */ 
        char *ip_copy = g_strdup(ip_text);
        char *token, *saveptr;

        for (int i = 0; i < 4; i++) {
            token = strtok_r(i == 0 ? ip_copy : NULL, ".", &saveptr);
            if (token == NULL) {
                g_print("Invalid IP format: %s\n", ip_text);
                g_free(ip_copy);
                return;
            }
            BYTE byte_val = atoi(token);
            switch (i) {
                case 0:
                    sb1 = byte_val;
                    break;
                case 1:
                    sb2 = byte_val;
                    break;
                case 2:
                    sb3 = byte_val;
                    break;
                case 3:
                    sb4 = byte_val;
                    break;
            }
        }
        g_print("Parsed IP: %d.%d.%d.%d\n", sb1, sb2, sb3, sb4);
        g_free(ip_copy);
    }
    else {
            g_print("Please enter a valid IP.\n");
    }
        
    /* 연결 상태면->해제, 해제상태면-> 연결 Send,Monitor버튼도 필요에따라 활성화 */
    if (strcmp(label_text, "Connect") == 0)
    {
        g_print("Selected Protocol: %s\n", protocol);
        
        if(strcmp(protocol, "TCP") == 0){
            if(FAS_ConnectTCP(sb1, sb2, sb3, sb4, 0)){
                gtk_button_set_label(button, "Disconn");
                gtk_widget_set_sensitive(GTK_WIDGET(button_send), TRUE);
                gtk_widget_set_sensitive(GTK_WIDGET(button_statusmonitor), TRUE);
            }
        }
        else if(strcmp(protocol, "UDP") == 0){
            if(FAS_Connect(sb1, sb2, sb3, sb4, 0)){
                gtk_button_set_label(button, "Disconn");
                gtk_widget_set_sensitive(GTK_WIDGET(button_send), TRUE);
                gtk_widget_set_sensitive(GTK_WIDGET(button_statusmonitor), TRUE);
            }
        }
        else if(protocol != NULL){
            g_print("Select Protocol\n");
        }
    }
    else if (strcmp(label_text, "Disconn") == 0)
    {
        FAS_Close(0);
        gtk_button_set_label(button, "Connect");
        gtk_widget_set_sensitive(GTK_WIDGET(button_send), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(button_statusmonitor), FALSE);
    }
}

 /** @brief Send버튼의 callback */
static void on_button_send_clicked(GtkButton *button, gpointer user_data){
    gtk_label_set_text(label_status, "Sending");
    
    /* SendBuffer에서 Frame 가져옴 */
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(sendbuffer_buffer, &start);
    gtk_text_buffer_get_end_iter(sendbuffer_buffer, &end);
    gchar *text = gtk_text_buffer_get_text(sendbuffer_buffer, &start, &end, FALSE);

    /* 텍스트를 띄어쓰기를 기준으로 16진수로 변환하여 uint8_t 배열에 저장 */
    BYTE byte_array[258];
    int byte_count = 0;
    gchar **tokens = g_strsplit(text, " ", -1);
    for (int i = 0; tokens[i] != NULL; i++) {
        byte_array[byte_count] = (BYTE)g_ascii_strtoull(tokens[i], NULL, 16);
        byte_count++;
    }
    g_strfreev(tokens);
    byte_array[2] = sync_no;

    /* 체크용 출력문, 이건 나중에 지워도 이상없음*/ 
    for (int i = 0; i < byte_count; i++) {
        printf("%02X ", byte_array[i]);
    }
    printf("\n");

    /* 프로토콜에 따라 전달함수 호출 */ 
    if(strcmp(protocol, "TCP") == 0){
        send_packetTCP(byte_array, byte_count);
    }
    else if(strcmp(protocol, "UDP") == 0){
        send_packet(byte_array, byte_count);
    }
    else if(protocol != NULL){
        g_print("Select Protocol\n");
    }
    sync_no_update();
}

 /** @brief TCP/UDP 프로토콜 선택 콤보박스의 callback*/
static void on_combo_protocol_changed(GtkComboBoxText *combo_text, gpointer user_data) {
    protocol = gtk_combo_box_text_get_active_text(combo_text);
    if (protocol != NULL) {
        g_print("Selected Protocol: %s\n", protocol);
    }
}

 /** @brief 명령어 콤보박스 combo_command의 callback*/
static void on_combo_command_changed(GtkComboBox *combo_id, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    const gchar *selected_id = gtk_combo_box_get_active_id(combo_id);
    GtkStack *stk2 = GTK_STACK(gtk_builder_get_object(builder, "stk2"));
    GObject *button_pt = gtk_builder_get_object(builder, "button_pt");
    gtk_label_set_text(label_status, "Ready");
    if (selected_id != NULL) {

        /* 선택한 옵션에 따라서 GTKStack에서 보여지는 페이지를 변경->Glade에서 보면서 만드는게 좋음 */ 
        if (g_strcmp0(selected_id, "0x2A") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page1");
        } 
        else if (g_strcmp0(selected_id, "0x37") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page2");
        }
        else if (g_strcmp0(selected_id, "0x60") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page3");
            gtk_widget_set_sensitive(GTK_WIDGET(button_pt), FALSE);
        }
        else if (g_strcmp0(selected_id, "0x61") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page3");
            gtk_widget_set_sensitive(GTK_WIDGET(button_pt), TRUE);
        }
        else if (g_strcmp0(selected_id, "0x62") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page3");
            gtk_widget_set_sensitive(GTK_WIDGET(button_pt), FALSE);
        }
        else if (g_strcmp0(selected_id, "0x63") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page3");
            gtk_widget_set_sensitive(GTK_WIDGET(button_pt), FALSE);
        }
        else if (g_strcmp0(selected_id, "0x64") == 0) {
            gtk_stack_set_visible_child_name(stk2, "page3");
            gtk_widget_set_sensitive(GTK_WIDGET(button_pt), FALSE);
        }
        else{
            gtk_stack_set_visible_child_name(stk2, "page0");
        }
    }
    
    if (selected_id  != NULL) {
        g_print("Selected Command: %s\n", selected_id);
        char* endptr;
        unsigned long int value = strtoul(selected_id, &endptr, 16);
        if (*endptr == '\0' && value <= UINT8_MAX) {
            frame_type = (uint8_t)value;
        } else {
            g_print("Invalid input: %s\n", selected_id);
        }
    } else {
        g_print("No item selected.\n");
    }
    /* 출력문은 나중에 다 지워도 무방, frame_type->library interface-> sendbuffer 출력 */ 
    g_print("Converted Frame: %X \n", frame_type);
    char *sendbuffer = library_interface();
    gtk_text_buffer_set_text(sendbuffer_buffer, sendbuffer, -1);
}

 /** @brief 명령어 콤보박스 combo_data1의 callback*/
static void on_combo_data1_changed(GtkComboBox *combo_id, gpointer user_data) {
    const gchar *selected_id = gtk_combo_box_get_active_id(combo_id);
    memset(&data, 0, sizeof(data));
    if (selected_id != NULL) {
        g_print("Selected Data: %s\n", selected_id);
        char* endptr;
        unsigned long int value = strtoul(selected_id, &endptr, 16);
        if (*endptr == '\0' && value <= UINT8_MAX) {
            data[0] = (uint8_t)value;
        } else {
            g_print("Invalid input: %s\n", selected_id);
        }
    } else {
        g_print("No item selected.\n");
    }
    /* 출력문은 나중에 다 지워도 무방, frame_type->library interface-> sendbuffer 출력 */ 
    g_print("Converted Data: %X \n", data[0]);
    char *sendbuffer = library_interface();
    gtk_text_buffer_set_text(sendbuffer_buffer, sendbuffer, -1);
}

 /** @brief AutoSync 체크박스의 callback 체크가 안되면 00으로 고정*/
static void on_check_autosync_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    gboolean is_checked = gtk_toggle_button_get_active(togglebutton);
    if (is_checked) {
        sync_no = (BYTE)(rand() % 256);
        g_print("Auto Sync Enabled Sync No: %X \n",sync_no);
    } else {
        sync_no = 0x00;
        g_print("Auto Sync Disabled Sync No: %X \n",sync_no);
    }
}

 /** @brief 보낸 패킷 표시 체크박스의 callback*/
static void on_check_showsend_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    gboolean is_checked = gtk_toggle_button_get_active(togglebutton);
    if (is_checked) {
        show_send = TRUE;
    } else {
        show_send = FALSE;
    }
}

 /** @brief FASTECH 프로토콜 체크박스의 callback*/
static void on_check_fastech_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    gboolean is_checked = gtk_toggle_button_get_active(togglebutton);
    if (is_checked) {
        header = 0xAA;
        g_print("FASTECH Protocol header: %X \n",header);
    } else {
        header = 0x00;
        g_print("USER Protocol header: %X \n",header);
    }
}

 /** @brief MoveVelocity에서 방향 선택 콤보박스의 callback*/
static void on_combo_direction_changed(GtkComboBox *combo_id, gpointer user_data) {
    memset(&data, 0, sizeof(data));
    /* Get the GtkBuilder object passed as user data */ 
    GtkBuilder *builder = GTK_BUILDER(user_data);

    GtkEntry *entry_speed = GTK_ENTRY(gtk_builder_get_object(builder, "entry_speed"));
    const gchar *selected_id = gtk_combo_box_get_active_id(combo_id);
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry_speed));
    
    /* 속도를 little endian 변환함 */
    int value = atoi(text);
    
    for (int i = 0; i < 4; i++) {
        data[i] = (value >> (8 * i)) & 0xFF;
    }

    if (selected_id != NULL) {
        g_print("Selected Data: %s\n", selected_id);
        char* endptr;
        unsigned long int value = strtoul(selected_id, &endptr, 16);
        if (*endptr == '\0' && value <= UINT8_MAX) {
            data[4] = (uint8_t)value;
        } else {
            g_print("Invalid input: %s\n", selected_id);
        }
    } else {
        g_print("No item selected.\n");
    }
    /* 출력문은 나중에 다 지워도 무방, frame_type->library interface-> sendbuffer 출력 */ 
    print_buffer(data, 5);
    char *sendbuffer = library_interface();
    gtk_text_buffer_set_text(sendbuffer_buffer, sendbuffer, -1);
}

 /** @brief Frame칸에 명령어를 입력할 때 생기는 callback*/
gboolean on_text_frame_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GtkTextBuffer *frame_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(frame_buffer, &start, &end);
    char *input = gtk_text_buffer_get_text(frame_buffer, &start, &end, FALSE);

    g_print("Current Text: %s\n", input);

    int inputLength = strlen(input);
    int outputSize = 0;
    uint8_t *output = NULL;

    /* "[2A 01 12]"처럼 HEX 형식인지 확인 */ 
    if (inputLength >= 3 && input[0] == '[' && input[inputLength - 1] == ']') {
        /*  "2A 01 12" 부분처럼 HEX 명령어 추출 */
        char hexPart[100];
        strncpy(hexPart, input + 1, inputLength - 2);
        hexPart[inputLength - 2] = '\0';

        /* 16진수 문자열을 숫자로 변환하여 동적 메모리 할당 후 저장 */ 
        char *token;
        int index = 0;
        token = strtok(hexPart, " ");
        while (token != NULL) {
            output = (uint8_t *)realloc(output, (index + 1) * sizeof(uint8_t));
            output[index++] = (uint8_t)strtol(token, NULL, 16);
            token = strtok(NULL, " ");
        }

        outputSize = index;
    }
    /* "112 123 123" 처럼 10진수 형식인 경우 */ 
    else if (input[0] != '[') {
        /* 10진수 문자열을 숫자로 변환하여 동적 메모리 할당 후 저장 */
        char *token;
        int index = 0;
        token = strtok(input, " ");
        while (token != NULL) {
            output = (uint8_t *)realloc(output, (index + 1) * sizeof(uint8_t));
            output[index++] = (uint8_t)atoi(token);
            token = strtok(NULL, " ");
        }
        outputSize = index;
    }
    else{
        g_free(output);
    }
    /* 출력문은 나중에 다 지워도 무방 */
    printf("Parsed Hex: ");
    for (int i = 0; i < outputSize; i++) {
        printf("%02X ", output[i]);
    }
    printf("\n");
    
    g_free(input);

    /* header를 기준으로 프로토콜 구분 */
    if (output != NULL && header == 0xAA) {
        buffer[0] = header;
        buffer[1] = outputSize + 2;
        buffer[2] = sync_no;
        memcpy(&buffer[4], output, outputSize);
        free(output);

        char *text = array_to_string(buffer, buffer[1] + 2);
        gtk_text_buffer_set_text(sendbuffer_buffer, text, -1);
        g_free(text);
    } 
    else if((output != NULL && header == 0x00)){
        memcpy(&buffer[0], output, outputSize); 
        free(output);
        
        char *text = array_to_string(buffer, outputSize);
        gtk_text_buffer_set_text(sendbuffer_buffer, text, -1);
        g_free(text);
    }
    else {
        gtk_text_buffer_set_text(sendbuffer_buffer, "", -1);
        memset(buffer, 0, sizeof(buffer));
    }

    return FALSE;
}

 /** @brief Postion table 실행 버튼의 callback*/
static void on_button_pt_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *position_table = GTK_WINDOW(user_data);
    gtk_widget_set_visible(GTK_WIDGET(position_table), TRUE);
}

 /** @brief Postion table창에서 OK 버튼의 callback*/
static void on_button_pt_ok_clicked(GtkButton *button, gpointer user_data) {
    ITEM_NODE pos_table;

    GtkTextIter start, end;
    char *input;
    gtk_text_buffer_get_bounds(lPosition_buffer, &start, &end);
    input = gtk_text_buffer_get_text(lPosition_buffer, &start, &end, FALSE);
    pos_table.lPosition = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(dwStartSpd_buffer, &start, &end);
    input = gtk_text_buffer_get_text(dwStartSpd_buffer, &start, &end, FALSE);
    pos_table.dwStartSpd = strtoul(input, NULL, 10);

    gtk_text_buffer_get_bounds(dwMoveSpd_buffer, &start, &end);
    input = gtk_text_buffer_get_text(dwMoveSpd_buffer, &start, &end, FALSE);
    pos_table.dwMoveSpd = strtoul(input, NULL, 10);

    gtk_text_buffer_get_bounds(wAccelRate_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wAccelRate_buffer, &start, &end, FALSE);
    pos_table.wAccelRate = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wDecelRate_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wDecelRate_buffer, &start, &end, FALSE);
    pos_table.wDecelRate = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wCommand_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wCommand_buffer, &start, &end, FALSE);
    pos_table.wCommand = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wWaitTime_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wWaitTime_buffer, &start, &end, FALSE);
    pos_table.wWaitTime = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wContinuous_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wContinuous_buffer, &start, &end, FALSE);
    pos_table.wContinuous = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wBranch_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wBranch_buffer, &start, &end, FALSE);
    pos_table.wBranch = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wCond_branch0_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wCond_branch0_buffer, &start, &end, FALSE);
    pos_table.wCond_branch0 = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wCond_branch1_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wCond_branch1_buffer, &start, &end, FALSE);
    pos_table.wCond_branch1 = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wCond_branch2_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wCond_branch2_buffer, &start, &end, FALSE);
    pos_table.wCond_branch2 = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wLoopCount_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wLoopCount_buffer, &start, &end, FALSE);
    pos_table.wLoopCount = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wBranchAfterLoop_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wBranchAfterLoop_buffer, &start, &end, FALSE);
    pos_table.wBranchAfterLoop = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wPTSet_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wPTSet_buffer, &start, &end, FALSE);
    pos_table.wPTSet = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wLoopCountCLR_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wLoopCountCLR_buffer, &start, &end, FALSE);
    pos_table.wLoopCountCLR = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(bCheckInpos_buffer, &start, &end);
    input = gtk_text_buffer_get_text(bCheckInpos_buffer, &start, &end, FALSE);
    pos_table.bCheckInpos = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(lTriggerPos_buffer, &start, &end);
    input = gtk_text_buffer_get_text(lTriggerPos_buffer, &start, &end, FALSE);
    pos_table.lTriggerPos = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wTriggerOnTime_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wTriggerOnTime_buffer, &start, &end, FALSE);
    pos_table.wTriggerOnTime = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wPushRatio_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wPushRatio_buffer, &start, &end, FALSE);
    pos_table.wPushRatio = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(dwPushSpeed_buffer, &start, &end);
    input = gtk_text_buffer_get_text(dwPushSpeed_buffer, &start, &end, FALSE);
    pos_table.dwPushSpeed = strtoul(input, NULL, 10);

    gtk_text_buffer_get_bounds(lPushPosition_buffer, &start, &end);
    input = gtk_text_buffer_get_text(lPushPosition_buffer, &start, &end, FALSE);
    pos_table.lPushPosition = strtol(input, NULL, 10);

    gtk_text_buffer_get_bounds(wPushMode_buffer, &start, &end);
    input = gtk_text_buffer_get_text(wPushMode_buffer, &start, &end, FALSE);
    pos_table.wPushMode = strtol(input, NULL, 10);

    memset(&data, 0, sizeof(data));

    WORD pt_no = atoi(gtk_entry_get_text(entry_pt));
    *((WORD*)&data[0]) = pt_no;

    *((LONG*)&data[OFFSET_POSITION + 2]) = pos_table.lPosition;
    *((LONG*)&data[OFFSET_TRIGGERPOSITION + 2]) = pos_table.lTriggerPos;
    *((LONG*)&data[OFFSET_PUSHPOSITION + 2]) = pos_table.lPushPosition;

    *((DWORD*)&data[OFFSET_LOWSPEED + 2]) = pos_table.dwStartSpd;
    *((DWORD*)&data[OFFSET_HIGHSPEED + 2]) = pos_table.dwMoveSpd;
    *((DWORD*)&data[OFFSET_PUSHSPEED + 2]) = pos_table.dwPushSpeed;

    *((WORD*)&data[OFFSET_ACCELTIME + 2]) = pos_table.wAccelRate;
    *((WORD*)&data[OFFSET_DECELTIME + 2]) = pos_table.wDecelRate;
    *((WORD*)&data[OFFSET_COMMAND + 2]) = pos_table.wCommand;
    *((WORD*)&data[OFFSET_WAITTIME + 2]) = pos_table.wWaitTime;
    *((WORD*)&data[OFFSET_CONTINUOUS + 2]) = pos_table.wContinuous;
    *((WORD*)&data[OFFSET_JUMPTABLENO + 2]) = pos_table.wBranch;
    *((WORD*)&data[OFFSET_JUMPPT0 + 2]) = pos_table.wCond_branch0;
    *((WORD*)&data[OFFSET_JUMPPT1 + 2]) = pos_table.wCond_branch1;
    *((WORD*)&data[OFFSET_JUMPPT2 + 2]) = pos_table.wCond_branch2;
    *((WORD*)&data[OFFSET_LOOPCOUNT + 2]) = pos_table.wLoopCount;
    *((WORD*)&data[OFFSET_LOOPJUMPTABLENO + 2]) = pos_table.wBranchAfterLoop;
    *((WORD*)&data[OFFSET_PTSET + 2]) = pos_table.wPTSet;
    *((WORD*)&data[OFFSET_LOOPCOUNTCLEAR + 2]) = pos_table.wLoopCountCLR;
    *((WORD*)&data[OFFSET_CHECKINPOSITION + 2]) = pos_table.bCheckInpos;
    *((WORD*)&data[OFFSET_TRIGGERONTIME + 2]) = pos_table.wTriggerOnTime;
    *((WORD*)&data[OFFSET_PUSHRATIO + 2]) = pos_table.wPushRatio;
    *((WORD*)&data[OFFSET_PUSHMODE + 2]) = pos_table.wPushMode;

    char *text = array_to_string(data, 66);
    gtk_text_buffer_set_text(pt_frame_buffer, text, -1);

    char *sendbuffer = library_interface();
    gtk_text_buffer_set_text(sendbuffer_buffer, sendbuffer, -1);

}

gboolean on_entry_pt_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    WORD pt_no = atoi(gtk_entry_get_text(entry_pt));
    *((WORD*)&data[0]) = pt_no;

    char *sendbuffer = library_interface();
    gtk_text_buffer_set_text(sendbuffer_buffer, sendbuffer, -1);
}

 /** @brief  Postion table  닫힐 때 숨기기 */
gboolean on_position_table_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
     /*  윈도우를 숨기기만 하고 파괴하지 않음 */
    gtk_widget_hide(widget);
     /* TRUE를 반환하여 기본 동작인 윈도우 파괴를 방지 */
    return TRUE;
}

 /** @brief  상태 모니터 윈도우가 닫힐 때 숨기기 */
gboolean on_status_monitor_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
     /*  윈도우를 숨기기만 하고 파괴하지 않음 */
    gtk_widget_hide(widget);
    g_source_remove(repeat_timer);

    /* 메인윈도우에 모니터 부분은 다시 보이게 */
    show_send = TRUE;
    show_recv = TRUE;
     /* TRUE를 반환하여 기본 동작인 윈도우 파괴를 방지 */
    return TRUE;
}

 /** @brief statusmonitor실행 버튼의 callback*/
static void on_button_statusmonitor_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *status_monitor = GTK_WINDOW(user_data);
    gtk_widget_set_visible(GTK_WIDGET(status_monitor), TRUE);
    /* entry_interval에 숫자 기준으로 반복적으로 status_monitor_update */
    gint interval = atoi(gtk_entry_get_text(entry_interval));
    repeat_timer = g_timeout_add(interval, status_monitor_update, NULL);

    /* 메인윈도우에 모니터 부분은 다 안보여지게 */
    show_send = FALSE;
    show_recv = FALSE;
}

/** @brief  상태 모니터 업데이트-> 무지성 GetAllStatus 반복, 정보처리 후 표시 */
gboolean status_monitor_update(gpointer user_data) {

    BYTE byte_array[5];
    BYTE* recv_frame;
    byte_array[0] = 0xAA; byte_array[1] = 0x03; byte_array[2] = sync_no; byte_array[3] = 0x00; byte_array[4] = 0x43;
    if(strcmp(protocol, "TCP") == 0){
        recv_frame = send_packetTCP(byte_array, 5);
    }
    else if(strcmp(protocol, "UDP") == 0){
        recv_frame = send_packet(byte_array, 5);
    }
    else if(protocol != NULL){
        g_print("Select Protocol\n");
    }
    /* 
    Little Endian변환->i 기준으로 value[i]에 넣음
    입력상태[0] 6-9, 출력상태[1] 10-13, 
    상태 Flag[2] 14-17, 
    command pos[3] 18-21, actual pos[4] 22-25, pos차이[5] 26-29, 
    운전속도[6] 30-33, 현재 pt번호[7] 34-37 
    */
    uint32_t value[8];
    for(int i = 6; i < 35; i += 4){
        value[(i - 6) / 4] = (recv_frame[i] | (recv_frame[i + 1] << 8) | (recv_frame[i + 2] << 16) | (recv_frame[i + 3] << 24));
    }
    
    /* 무지성 정보 업데이트 */
    char value_buffer[20];
    snprintf(value_buffer, sizeof(value_buffer), "0x%X", value[0]);
    gtk_text_buffer_set_text(input_buffer, value_buffer, -1);
    
    memset(&value_buffer, 0, sizeof(value_buffer));
    snprintf(value_buffer, sizeof(value_buffer), "0x%X", value[1]);
    gtk_text_buffer_set_text(output_buffer, value_buffer, -1);
    
    memset(&value_buffer, 0, sizeof(value_buffer));
    snprintf(value_buffer, sizeof(value_buffer), "0x%08X", value[2]);
    gtk_text_buffer_set_text(axis_status_buffer, value_buffer, -1);
    
    memset(&value_buffer, 0, sizeof(value_buffer));
    snprintf(value_buffer, sizeof(value_buffer), "%d", value[3]);
    gtk_text_buffer_set_text(command_pos_buffer, value_buffer, -1);
    
    memset(&value_buffer, 0, sizeof(value_buffer));
    snprintf(value_buffer, sizeof(value_buffer), "%d", value[4]);
    gtk_text_buffer_set_text(actual_pos_buffer, value_buffer, -1);
    
    memset(&value_buffer, 0, sizeof(value_buffer));
    snprintf(value_buffer, sizeof(value_buffer), "%d", value[5]);
    gtk_text_buffer_set_text(position_buffer, value_buffer, -1);
    
    memset(&value_buffer, 0, sizeof(value_buffer));
    snprintf(value_buffer, sizeof(value_buffer), "%d", value[6]);
    gtk_text_buffer_set_text(velocity_buffer, value_buffer, -1);
    
    memset(&value_buffer, 0, sizeof(value_buffer));
    snprintf(value_buffer, sizeof(value_buffer), "%d", value[7]);
    gtk_text_buffer_set_text(table_buffer, value_buffer, -1);
    
    memset(&value_buffer, 0, sizeof(value_buffer));
    snprintf(value_buffer, sizeof(value_buffer), "0x%X", buffer[5]);
    gtk_text_buffer_set_text(status_buffer, value_buffer, -1);
    
    /* MOTION_EziSERVO2_DEFINE에서 Flag부분을 가져옴 */
     EZISERVO2_AXISSTATUS eziservo2_axisstatus;
    eziservo2_axisstatus.dwValue = value[2];

    /* 무지성 Flag확인 yes->초록색 no->빨간색 기본 표시 설정 반복 -> 정확한 값이 아닌 부분이 있을 수 있음 */
    if (eziservo2_axisstatus.FFLAG_ERRORALL) {
        printf("FFLAG_ERRORALL 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_error_all), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_error_all), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_HWPOSILMT) {
        printf("FFLAG_HWPOSILMT 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_hw_plus), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_hw_plus), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_HWNEGALMT) {
        printf("FFLAG_HWNEGALMT 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_hw_minus), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_hw_minus), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_SWPOGILMT) {
        printf("FFLAG_SWPOGILMT 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_sw_plus), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_sw_plus), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_SWNEGALMT) {
        printf("FFLAG_SWNEGALMT 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_sw_minus), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_sw_minus), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_RESERVED0) {
        printf("FFLAG_RESERVED0 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_reserved), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_reserved), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_RESERVED1) {
        printf("FFLAG_RESERVED1 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_reserved), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_reserved), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ERRPOSOVERFLOW) {
        printf("FFLAG_ERRPOSOVERFLOW 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_pos_count), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_pos_count), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ERROVERCURRENT) {
        printf("FFLAG_ERROVERCURRENT 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_err_current), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_err_current), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ERROVERSPEED) {
        printf("FFLAG_ERROVERSPEED 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_err_speed), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_err_speed), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ERRPOSTRACKING) {
        printf("FFLAG_ERRPOSTRACKING 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_err_out), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_err_out), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ERROVERLOAD) {
        printf("FFLAG_ERROVERLOAD 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_err_load), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_err_load), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ERROVERHEAT) {
        printf("FFLAG_ERROVERHEAT 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_err_heat), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_err_heat), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ERRBACKEMF) {
        printf("FFLAG_ERRBACKEMF 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_err_back_emf), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_err_back_emf), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ERRMOTORPOWER) {
        printf("FFLAG_ERRMOTORPOWER 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_err_power), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_err_power), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ERRINPOSITION) {
        printf("FFLAG_ERRINPOSITION 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_err_inpositin), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_err_inpositin), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_EMGSTOP) {
        printf("FFLAG_EMGSTOP 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_emg_stop), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_emg_stop), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_SLOWSTOP) {
        printf("FFLAG_SLOWSTOP 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_slow_stop), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_slow_stop), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ORIGINRETURNING) {
        printf("FFLAG_ORIGINRETURNING 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_org_return), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_org_return), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_INPOSITION) {
        printf("FFLAG_INPOSITION 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_inposition), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_inposition), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_SERVOON) {
        printf("FFLAG_SERVOON 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_servo_on), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_servo_on), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ALARMRESET) {
        printf("FFLAG_ALARMRESET 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_alarm_reset), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_alarm_reset), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_PTSTOPPED) {
        printf("FFLAG_PTSTOPPED 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_pt_stopped), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_pt_stopped), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ORIGINSENSOR) {
        printf("FFLAG_ORIGINSENSOR 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_origin_sensor), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_origin_sensor), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ZPULSE) {
        printf("FFLAG_ZPULSE 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_z_pulse), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_z_pulse), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_ORIGINRETOK) {
        printf("FFLAG_ORIGINRETOK 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_org_ret), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_org_ret), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_MOTIONDIR) {
        printf("FFLAG_MOTIONDIR 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_motion_dir), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_motion_dir), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_MOTIONING) {
        printf("FFLAG_MOTIONING 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_motioning), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_motioning), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_MOTIONPAUSE) {
        printf("FFLAG_MOTIONPAUSE 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_motion_pause), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_motion_pause), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_MOTIONACCEL) {
        printf("FFLAG_MOTIONACCEL 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_motion_accel), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_motion_accel), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_MOTIONDECEL) {
        printf("FFLAG_MOTIONDECEL 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_motion_decel), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_motion_decel), "stock", "gtk-no", NULL);
    }
    if (eziservo2_axisstatus.FFLAG_MOTIONCONST) {
        printf("FFLAG_MOTIONCONST 플래그가 설정되어 있음\n");
        g_object_set(G_OBJECT(img_motion_constant), "stock", "gtk-yes", NULL);
    }
    else{
        g_object_set(G_OBJECT(img_motion_constant), "stock", "gtk-no", NULL);
    }

    free(recv_frame);
    return TRUE;
}

 /** @brief status monitor에서 interval설정하는 버튼의 callback*/
static void on_button_ok_clicked(GtkButton *button, gpointer user_data){
    g_source_remove(repeat_timer);
    
    gint interval = atoi(gtk_entry_get_text(entry_interval));
    repeat_timer = g_timeout_add(interval, status_monitor_update, NULL);
}

 /** @brief 메인 윈도우 monitor에서 Clear 버튼의 callback*/
static void on_button_clear_clicked(GtkButton *button, gpointer user_data){
    gtk_text_buffer_set_text(monitor1_buffer, "", -1);
    gtk_text_buffer_set_text(monitor2_buffer, "", -1);
}

 /** @brief Record기능의 기록 버튼 callback sendbuffer 내용 복사->자기 버퍼에 붙여넣기 */
static void on_button_record1_clicked(GtkButton *button, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(sendbuffer_buffer, &start);
    gtk_text_buffer_get_end_iter(sendbuffer_buffer, &end);
    gchar *text = gtk_text_buffer_get_text(sendbuffer_buffer, &start, &end, FALSE);
    
    gtk_text_buffer_set_text(record1_buffer, text, -1);
}
 /** @brief Record기능의 기록 버튼 callback sendbuffer 내용 복사->자기 버퍼에 붙여넣기 */
static void on_button_record2_clicked(GtkButton *button, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(sendbuffer_buffer, &start);
    gtk_text_buffer_get_end_iter(sendbuffer_buffer, &end);
    gchar *text = gtk_text_buffer_get_text(sendbuffer_buffer, &start, &end, FALSE);
    
    gtk_text_buffer_set_text(record2_buffer, text, -1);
}
 /** @brief Record기능의 기록 버튼 callback sendbuffer 내용 복사->자기 버퍼에 붙여넣기 */
static void on_button_record3_clicked(GtkButton *button, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(sendbuffer_buffer, &start);
    gtk_text_buffer_get_end_iter(sendbuffer_buffer, &end);
    gchar *text = gtk_text_buffer_get_text(sendbuffer_buffer, &start, &end, FALSE);
    
    gtk_text_buffer_set_text(record3_buffer, text, -1);
}
 /** @brief Record기능의 기록 버튼 callback sendbuffer 내용 복사->자기 버퍼에 붙여넣기 */
static void on_button_record4_clicked(GtkButton *button, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(sendbuffer_buffer, &start);
    gtk_text_buffer_get_end_iter(sendbuffer_buffer, &end);
    gchar *text = gtk_text_buffer_get_text(sendbuffer_buffer, &start, &end, FALSE);
    
    gtk_text_buffer_set_text(record4_buffer, text, -1);
}


 /** @brief Record기능의 전송 버튼 자기 버퍼 내용 복사->Sendbuffer에 붙여넣기, 전송 */
static void on_button_transfer1_clicked(GtkButton *button, gpointer user_data) {
    gtk_label_set_text(label_status, "Sending");
    
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(record1_buffer, &start);
    gtk_text_buffer_get_end_iter(record1_buffer, &end);
    gchar *text = gtk_text_buffer_get_text(record1_buffer, &start, &end, FALSE);
    
    gtk_text_buffer_set_text(sendbuffer_buffer, text, -1);
    /* 텍스트를 띄어쓰기를 기준으로 16진수로 변환하여 BYTE 배열에 저장합니다. */ 
    BYTE byte_array[258];
    int byte_count = 0;
    gchar **tokens = g_strsplit(text, " ", -1);
    for (int i = 0; tokens[i] != NULL; i++) {
        byte_array[byte_count] = (BYTE)g_ascii_strtoull(tokens[i], NULL, 16);
        byte_count++;
    }
    g_strfreev(tokens);

    byte_array[2] = sync_no;
    for (int i = 0; i < byte_count; i++) {
        printf("%02X ", byte_array[i]);
    }
    printf("\n");
    
    /* 전송 */ 
    if(strcmp(protocol, "TCP") == 0){
        send_packetTCP(byte_array, byte_count);
    }
    else if(strcmp(protocol, "UDP") == 0){
        send_packet(byte_array, byte_count);
    }
    else if(protocol != NULL){
        g_print("Select Protocol\n");
    }
    sync_no_update();
}

 /** @brief Record기능의 전송 버튼 자기 버퍼 내용 복사->Sendbuffer에 붙여넣기, 전송 */
static void on_button_transfer2_clicked(GtkButton *button, gpointer user_data) {
    gtk_label_set_text(label_status, "Sending");
    
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(record2_buffer, &start);
    gtk_text_buffer_get_end_iter(record2_buffer, &end);
    gchar *text = gtk_text_buffer_get_text(record2_buffer, &start, &end, FALSE);
    
    gtk_text_buffer_set_text(sendbuffer_buffer, text, -1);
    /* 텍스트를 띄어쓰기를 기준으로 16진수로 변환하여 BYTE 배열에 저장합니다. */ 
    BYTE byte_array[258];
    int byte_count = 0;
    gchar **tokens = g_strsplit(text, " ", -1);
    for (int i = 0; tokens[i] != NULL; i++) {
        byte_array[byte_count] = (BYTE)g_ascii_strtoull(tokens[i], NULL, 16);
        byte_count++;
    }
    g_strfreev(tokens);

    byte_array[2] = sync_no;

    for (int i = 0; i < byte_count; i++) {
        printf("%02X ", byte_array[i]);
    }
    printf("\n");
    
    /* 전송 */ 
    if(strcmp(protocol, "TCP") == 0){
        send_packetTCP(byte_array, byte_count);
    }
    else if(strcmp(protocol, "UDP") == 0){
        send_packet(byte_array, byte_count);
    }
    else if(protocol != NULL){
        g_print("Select Protocol\n");
    }
    sync_no_update();
}

 /** @brief Record기능의 전송 버튼 자기 버퍼 내용 복사->Sendbuffer에 붙여넣기, 전송 */
static void on_button_transfer3_clicked(GtkButton *button, gpointer user_data) {
    gtk_label_set_text(label_status, "Sending");
    
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(record3_buffer, &start);
    gtk_text_buffer_get_end_iter(record3_buffer, &end);
    gchar *text = gtk_text_buffer_get_text(record3_buffer, &start, &end, FALSE);
    
    gtk_text_buffer_set_text(sendbuffer_buffer, text, -1);
    /* 텍스트를 띄어쓰기를 기준으로 16진수로 변환하여 BYTE 배열에 저장합니다. */ 
    BYTE byte_array[258];
    int byte_count = 0;
    gchar **tokens = g_strsplit(text, " ", -1);
    for (int i = 0; tokens[i] != NULL; i++) {
        byte_array[byte_count] = (BYTE)g_ascii_strtoull(tokens[i], NULL, 16);
        byte_count++;
    }
    g_strfreev(tokens);

    byte_array[2] = sync_no;

    for (int i = 0; i < byte_count; i++) {
        printf("%02X ", byte_array[i]);
    }
    printf("\n");
    
    /* 전송 */ 
    if(strcmp(protocol, "TCP") == 0){
        send_packetTCP(byte_array, byte_count);
    }
    else if(strcmp(protocol, "UDP") == 0){
        send_packet(byte_array, byte_count);
    }
    else if(protocol != NULL){
        g_print("Select Protocol\n");
    }
    sync_no_update();
}

 /** @brief Record기능의 전송 버튼 자기 버퍼 내용 복사->Sendbuffer에 붙여넣기, 전송 */
static void on_button_transfer4_clicked(GtkButton *button, gpointer user_data) {
    gtk_label_set_text(label_status, "Sending");
    
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(record4_buffer, &start);
    gtk_text_buffer_get_end_iter(record4_buffer, &end);
    gchar *text = gtk_text_buffer_get_text(record4_buffer, &start, &end, FALSE);
    
    gtk_text_buffer_set_text(sendbuffer_buffer, text, -1);
    /* 텍스트를 띄어쓰기를 기준으로 16진수로 변환하여 BYTE 배열에 저장합니다. */ 
    BYTE byte_array[258];
    int byte_count = 0;
    gchar **tokens = g_strsplit(text, " ", -1); 
    for (int i = 0; tokens[i] != NULL; i++) {
        byte_array[byte_count] = (BYTE)g_ascii_strtoull(tokens[i], NULL, 16);
        byte_count++;
    }
    g_strfreev(tokens);

    byte_array[2] = sync_no;

    for (int i = 0; i < byte_count; i++) {
        printf("%02X ", byte_array[i]);
    }
    printf("\n");
    
    /* 전송 */ 
    if(strcmp(protocol, "TCP") == 0){
        send_packetTCP(byte_array, byte_count);
    }
    else if(strcmp(protocol, "UDP") == 0){
        send_packet(byte_array, byte_count);
    }
    else if(protocol != NULL){
        g_print("Select Protocol\n");
    }
    sync_no_update();
}

/**************************************************************************************************************************************
 ******************************************************* 편의상 만든 함수 **************************************************************
 *************************************************************************************************************************************/

 /** @brief sync_no 1씩 올리고 표시 */
void sync_no_update(){
    sync_no++;
    char sync_str[4];
    sprintf(sync_str, "%u", sync_no);
    gtk_text_buffer_set_text(autosync_buffer, sync_str, -1);
}

 /** @brief 타이머 시간이 초과되면 SIGALRM 신호가 발생, 이때 소켓을 닫고 에러 송출 */
void handle_alarm(int signum) {
    perror("timeout handler");
    close(client_socket);
    
    gtk_label_set_text(label_status, "NG");
    
    GtkTextIter iter;
    gtk_text_buffer_set_text(monitor2_buffer, "ERROR : Connection timed out", -1);
    gtk_text_buffer_get_end_iter(monitor2_buffer, &iter);
    gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
    gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
    gtk_text_buffer_insert(monitor2_buffer, &iter,"Please verify the connection or protocol", -1);
    gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
    gtk_text_buffer_insert(monitor2_buffer, &iter,"And then reconnect after disconnecting.", -1);
    gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
    gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
    gtk_text_buffer_insert(monitor2_buffer, &iter,"연결상태 또는 패킷의 프로토콜을 확인하시고", -1);
    gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
    gtk_text_buffer_insert(monitor2_buffer, &iter,"연결해제 후 다시 연결해주세요.", -1);
}

 /** 
  * @brief UDP통신에서 패킷전송 (sendto/recvfrom 함수 사용)
  * @param send_frame 보낼 명령어 @param byte_count 바이트 길이
  */
BYTE* send_packet(BYTE *send_frame, int byte_count){
    
    char* currentTimeString = get_time();
    if (currentTimeString != NULL) {
        gtk_label_set_text(label_time, currentTimeString);
        free(currentTimeString); // 메모리 해제
    }
    /* 보낸패킷 표시 체크된 상태-> monitor1에는 frame, monitor2에는 정보 표시 */
    if(show_send){
        char *text = array_to_string(send_frame, byte_count);
        gtk_text_buffer_set_text(monitor1_buffer, text, -1);
        
        frame_type = send_frame[4];
        char *command = command_interface();
        gtk_text_buffer_set_text(monitor2_buffer, "[SEND]", -1);
        
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(monitor2_buffer, &iter);
        gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
        gtk_text_buffer_insert(monitor2_buffer, &iter, command, -1);
        gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
        gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
    }
    int send_result = sendto(client_socket, send_frame, byte_count, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if (send_result < 0) {
        perror("sendto failed");
    }
    alarm(TIMEOUT_SECONDS);
    BYTE* recv_frame = (BYTE*)malloc(258 * sizeof(BYTE));
    
    while(1){
        ssize_t received_bytes = recvfrom(client_socket, recv_frame, 257, 0, NULL, NULL);
        
        if (received_bytes < 0) {
            perror("recvfrom failed");
            break;
        }
        /* 출력문은 나중에 다 지워도 무방 */
        printf("Server: ");
        for (ssize_t i = 0; i < received_bytes; i++) {
            printf("%02x ", (BYTE)recv_frame[i]);
        }
        printf("\n");
        
        gtk_label_set_text(label_status, "OK");
        packet_update(recv_frame,received_bytes);
        alarm(0);
        break;
    }
    return recv_frame;
}

 /** 
  * @brief TCP/IP통신에서 패킷전송 (send/recv 함수 사용)
  * @param send_frame 보낼 명령어 @param byte_count 바이트 길이
  */
BYTE* send_packetTCP(BYTE *send_frame, int byte_count){
    
    char* currentTimeString = get_time();
    if (currentTimeString != NULL) {
        gtk_label_set_text(label_time, currentTimeString);
        free(currentTimeString); // 메모리 해제
    }
    
    /* 보낸패킷 표시 체크된 상태-> monitor1에는 frame, monitor2에는 정보 표시 */
    if(show_send){
        char *text = array_to_string(send_frame, byte_count);
        gtk_text_buffer_set_text(monitor1_buffer, text, -1);
        frame_type = send_frame[4];
        char *command = command_interface();
        gtk_text_buffer_set_text(monitor2_buffer, "[SEND]", -1);
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(monitor2_buffer, &iter);
        gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
        gtk_text_buffer_insert(monitor2_buffer, &iter, command, -1);
        gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
        gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
    }
    alarm(TIMEOUT_SECONDS);
    ssize_t sent_bytes = send(client_socket, send_frame, byte_count, 0);
    if (sent_bytes < 0) {
        perror("sendto failed");
    }
    BYTE* recv_frame = (BYTE*)malloc(258 * sizeof(BYTE));
    
    while (1) {
        ssize_t received_bytes = recv(client_socket, recv_frame, 257, 0);
        recv_frame[received_bytes] = '\0';
         if (received_bytes < 0) {
            perror("recvfrom failed");
            break;
        }
        gtk_label_set_text(label_status, "OK");
        /* 출력문은 나중에 다 지워도 무방 */
        printf("Server: ");
        for (ssize_t i = 0; i < received_bytes; i++) {
            printf("%02x ", (BYTE)recv_frame[i]);
        }
        printf("\n");
        packet_update(recv_frame,received_bytes);
        alarm(0);
        break;
    }
    return recv_frame;
}

 /** 
  * @brief 수신한 프레임을 모니터에 표시하는 기능(쓰는 곳이 여러곳이라 분리한 함수)
  * @param send_frame 보낼 명령어 @param byte_count 바이트 길이
  */
void packet_update(BYTE *byte_array, ssize_t byte_count){
    FMM_ERROR errorCode = byte_array[5];
    char *errorMsg = FMM_interface(errorCode);
    GtkTextIter iter;
    char *response_text = array_to_string(byte_array, byte_count);
    /* 보낸패킷 표시 체크된 상태 & 상태모니터 켜고 끔에 따라 표시 정보처리 */
    if(show_send){
        gtk_text_buffer_get_end_iter(monitor1_buffer, &iter);
        gtk_text_buffer_insert(monitor1_buffer, &iter, "\n", -1);
        gtk_text_buffer_insert(monitor1_buffer, &iter, "\n", -1);
        gtk_text_buffer_insert(monitor1_buffer, &iter, response_text, -1);
    }
    else if(show_recv){
        gtk_text_buffer_set_text(monitor1_buffer, response_text, -1);
    }
    frame_type = byte_array[4];
    char *command = command_interface();
    
    if(show_send && show_recv){
        gtk_text_buffer_get_end_iter(monitor2_buffer, &iter);
        gtk_text_buffer_insert(monitor2_buffer, &iter, "[RECEIVE]", -1);
    }
    else if(!show_send && show_recv){
        gtk_text_buffer_set_text(monitor2_buffer, "[RECEIVE]", -1);
        gtk_text_buffer_get_end_iter(monitor2_buffer, &iter);
    }
    
    if(show_recv){
        gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
        gtk_text_buffer_insert(monitor2_buffer, &iter, command, -1);
        gtk_text_buffer_insert(monitor2_buffer, &iter, "\n", -1);
        gtk_text_buffer_insert(monitor2_buffer, &iter, "RESPONSE : ", -1);
        gtk_text_buffer_insert(monitor2_buffer, &iter, errorMsg, -1);
    }
}
