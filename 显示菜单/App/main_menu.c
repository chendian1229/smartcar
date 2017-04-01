  //可供参考的菜单函数模版

#include "main_menu.h"

void main_menu_task(void)
{
    result_e result;
    KEY_MSG_t keymsg;
MAIN_INIT:
    while(get_key_msg(&keymsg) == 1);      //清空按键消息

    LCD_clear(BCOLOUR);                     //清屏

    main_menu_init();                       //菜单初始化
    while(1)
    {
        result = main_menu_deal();
        if(result == RESULT_INIT )
        {
            //LCD_init();
            return ;      //直接退出后，会重新进来
        }
        else if(result == RESULT_TO_MENU )
        {
            goto MAIN_INIT;
        }
    }
}

/***************************************************************************************************/

result_e main_func(void)
{
   Site_t site     = {0, 0};                           //显示图像左上角位置


   KEY_MSG_t keymsg;
   while(get_key_msg(&keymsg) == 1);                   //清空按键消息

   LCD_clear(BCOLOUR);                                 //清屏
   LCD_FSTR_CH(site,main_func_name,FCOLOUR,BCOLOUR);     //显示菜单文字

   //主函数初始化
   gpio_init (PTA17,GPO,0);
   
   while(1)
    {
      
      //任务循环执行内容
      gpio_turn (PTA17);
      DELAY_MS(500);
    
      //任务按键处理
      while(get_key_msg(&keymsg) == 1)               // 按键处理
      {
          if((keymsg.status == KEY_HOLD) && (keymsg.key == KEY_B ))       //B 键 长按时退出
          {
              //任务退出处理【用户自行补充】

              return  RESULT_TO_MENU;
          }
          //其他的按键处理【用户自行补充】
          
          
      }
   }
}

/***************************************************************************************************/

result_e led_func(void)
{
   Site_t site     = {0, 0};                           //显示图像左上角位置


   KEY_MSG_t keymsg;
   while(get_key_msg(&keymsg) == 1);                   //清空按键消息

   LCD_clear(BCOLOUR);                                 //清屏
   LCD_FSTR_CH(site,led_func_name,FCOLOUR,BCOLOUR);     //显示菜单文字

   //任务初始化【用户自行补充】
   gpio_init (PTE26,GPO,1);
   while(1)
    {
      //任务循环执行内容【用户自行补充】
      gpio_turn (PTE26);
      DELAY_MS(500);

      //任务按键处理
      while(get_key_msg(&keymsg) == 1)               // 按键处理
      {
          if((keymsg.status == KEY_HOLD) && (keymsg.key == KEY_B ))       //A 键 按下时退出
          {
              //任务退出处理【用户自行补充】

              return  RESULT_TO_MENU;
          }
          //其他的按键处理【用户自行补充】
          
          
      }
   }
}