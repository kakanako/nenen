/**
 ******************************************************************************
 ** �t�@�C���� : app.cpp
 **
 ** �T�v : �J���[�Z���T�̒l��bluetooth�ŏE���Ă�����̍�肽��
 **2017/5/17�΂�
 ** ���L : 
 ******************************************************************************
 **/

#include "ev3api.h"
#include "app.h"
#include "balancer.h"
#include "TouchSensor.h"
#include "SonarSensor.h"
#include "ColorSensor.h"
#include "GyroSensor.h"
#include "Motor.h"
#include "Clock.h"

using namespace ev3api;

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

/* Bluetooth */
//static int32_t   bt_cmd = 0;      /* Bluetooth�R�}���h 1:�����[�g�X�^�[�g */
static FILE     *bt = NULL;      /* Bluetooth�t�@�C���n���h�� */

#define DEVICE_NAME     "RisoRanger2016-mk2"  /* Bluetooth�� hrp2/target/ev3.h BLUETOOTH_LOCAL_NAME�Őݒ� */
#define PASS_KEY        "1111" /* �p�X�L�[    hrp2/target/ev3.h BLUETOOTH_PIN_CODE�Őݒ� */
#define CMD_START         '1'    /* �����[�g�X�^�[�g�R�}���h */

/* LCD�t�H���g�T�C�Y */
#define CALIB_FONT (EV3_FONT_SMALL)
#define CALIB_FONT_WIDTH (6/*TODO: magic number*/)
#define CALIB_FONT_HEIGHT (8/*TODO: magic number*/)
#define MESSAGE_LEN 8

/* �֐��v���g�^�C�v�錾 */
//���b�Z�[�W�������֐�
static void Message(const char* str);
//��Ԃ�\������֐�
void display();
//�e�Z���T�̏�����������֐�
static void Init();


/* �I�u�W�F�N�g�ւ̃|�C���^��` */
TouchSensor*    touchSensor;
SonarSensor*    sonarSensor;
ColorSensor*    colorSensor;
GyroSensor*     gyroSensor;
Motor*          leftMotor;
Motor*          rightMotor;
Motor*          tailMotor;
Clock*          clock;

/*�\�����邽�߂̃O���[�o���ϐ�*/
int count;
static char message[MESSAGE_LEN + 1] = {0};

int colorSensor_data = 0;

/* ���C���^�X�N */
void main_task(intptr_t unused)
{
	/*�O���[�o���ϐ��̏�����*/
	count = 1;
	
	/*�e�Z���T�̃|�[�g�ݒ�*/
	Init();
	
    /* Open Bluetooth file */
	/*�ڑ���Ԃ��m�F*/
	while(!ev3_bluetooth_is_connected()){
		tslp_tsk(100);
	}
	
	/*�V���A���|�[�g���J��*/
	bt = ev3_serial_open_file(EV3_SERIAL_BT);
	assert(bt != NULL);
	Message("bluetooth serial port open");
	display();
    
    /* Bluetooth�ʐM�^�X�N�̋N�� */
	act_tsk(BT_TASK);
	Message("Bluetooth task Start");
	
    ev3_led_set_color(LED_ORANGE); /* �����������ʒm */
	Message("Init finished.");
	
    /**
    * ���C�����[�v
    */
    while(1)
    {
    	if (ev3_button_is_pressed(BACK_BUTTON)){
    		//backbutton���������ƏI��
    		Message("finished...");
    		break;
    	}
    	
    	/*�J���[�Z���T����l���Ƃ��Ă���*/
    	colorSensor_data = colorSensor->getBrightness();
    	
        clock->sleep(10); /* 10msec�����N�� */
    }
	

	/*�I������*/
    ter_tsk(BT_TASK);
    fclose(bt);

    ext_tsk();
}

//*****************************************************************************
// �֐��� : bt_task
// ���� : unused
// �Ԃ�l : �Ȃ�
// �T�v : 
//*****************************************************************************
void bt_task(intptr_t unused)
{
	//colorSensor_data�𑗐M������
	/*�ʐM����*/
	while(1){
		
		char str[8];
		sprintf(str, "%d", colorSensor_data);
		
		fwrite(str,1,8,bt);
		
		display();
	}
	
}

//*******************************************************************
// �֐��� : display
// ���� : �Ȃ�
// �Ԃ�l : �Ȃ�
// �T�v : ��Ԃ�\������
//*******************************************************************
void display()
{
  ev3_lcd_set_font(EV3_FONT_SMALL);
  ev3_lcd_draw_string("Program is running", 10, 30);
  ev3_lcd_set_font(EV3_FONT_MEDIUM);
  ev3_lcd_draw_string(message, 10, 40);
}

//*******************************************************************
// �֐��� : Message
// ���� : str(�\��������������)
// �Ԃ�l : �Ȃ�
// �T�v : 
//*******************************************************************
void Message(const char* str){
	ev3_lcd_draw_string(str, 0, CALIB_FONT_HEIGHT*count);
	count++;
}


//*******************************************************************
// �֐��� : Init
// ���� : �Ȃ�
// �Ԃ�l : �Ȃ�
// �T�v : 
//*******************************************************************
void Init(){

	/* �e�I�u�W�F�N�g�𐶐��E���������� */
    touchSensor = new TouchSensor(PORT_1);
    colorSensor = new ColorSensor(PORT_2);
    sonarSensor = new SonarSensor(PORT_3);
    gyroSensor  = new GyroSensor(PORT_4);
    leftMotor   = new Motor(PORT_C);
    rightMotor  = new Motor(PORT_B);
    tailMotor   = new Motor(PORT_A);
    clock       = new Clock();
}


//*******************************************************************
// �֐��� : 
// ���� : �Ȃ�
// �Ԃ�l : �Ȃ�
// �T�v : 
//*******************************************************************
