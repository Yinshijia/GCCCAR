#include "include.h"

void 
main()
{
    /* 设置中断向量表与中断服务函数的映射 */ 
    setvector();
    /* 初始化正交解码模块 */
    ftm_quad_init(FTM2);
    /* 电机、舵机初始化 */  
    motorinit();
    /* 摄像头初始化 */
    camera_init(imgbuff);
    /* 用来观察程序中的中间输出数据 */
    char Pointstr[10];
    char Dutystr1[10];
    char Dutystr2[10];

    while(1)
    {
      /*
      Pulses_Count = ftm_quad_get(FTM2);
      ftm_quad_clean(FTM2);
      if(Pulses_Count >= 0)
      {
        sprintf(Pointstr,"%d  \n", Pulses_Count);
        uart_putstr(UART5,Pointstr);
      }
      else
      {
        sprintf(Pointstr,"%d  \n", -Pulses_Count);
        uart_putstr(UART5,Pointstr);
      }
      */
    
      /* 获取图像 */
      camera_get_img();
      /* 解压图像 */
      img_extract(img, imgbuff, CAMERA_SIZE);
      /* 图像分析，得到中点数量 */
      Mid_Count = Image_GetLine(img);
      /* 使用加权平均获取目标点横坐标 */	
      Point = Point_Weight();
      /*
      sprintf(Pointstr,"%d  \n", Point);
      uart_putstr(UART5,Pointstr);
      */
      /* 电机控制 */
      //motor_control();
      /* 使用位置式PID解算,获取所需舵机占空比*/
      S_D5_Duty = PlacePID_Control(&S_D5_PID, 40, Point);
      /*
      sprintf(Dutystr1,"%d  ",S_D5_Duty);
      uart_putstr(UART5,Dutystr1);
      */
      /* 限幅保护，防止舵机转角过大被打坏，坏板子(8350~8750)，好板子(8270~8570) */
      S_D5_Duty = range_protect(S_D5_Duty, 8270, 8570);
      /*
      sprintf(Dutystr2,"%d  \n",S_D5_Duty);
      uart_putstr(UART5,Dutystr2);
      */
      /* 修改舵机占空比 */
      if(Starting_Line_Flag == 1)
      {
        ftm_pwm_init(MOTOR_FTM, MOTOR1_PWM, MOTOR_HZ, 0);
        ftm_pwm_init(MOTOR_FTM, MOTOR4_PWM, MOTOR_HZ, 0);
        ftm_pwm_init(MOTOR_FTM, MOTOR2_PWM, MOTOR_HZ, 7);
        ftm_pwm_init(MOTOR_FTM, MOTOR3_PWM, MOTOR_HZ, 7);
        S_D5_Duty = 8420;
      }
      ftm_pwm_duty(S_D5_FTM, S_D5_CH, S_D5_Duty);
      //vcan_sendimg(imgbuff,CAMERA_SIZE);
    }
}

