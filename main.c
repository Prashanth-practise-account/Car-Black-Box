/*
 Name: Prashanth B H
 Date: 08-11-2024
 Description : Car black Box  Project


*/
// Library Files
#include <xc.h>
#include "main.h"
#include "matrix_keypad.h"
#include "clcd.h"
#include"adc.h"
#include "external_eeprom.h"   
#include "ds1307.h"
#include "i2c.h"
#include "uart.h" 

void init_ds1307();

// Declared the variable

static char *message[] = {"ON","GN","GR","G1","G2","G3","G4","G5","C "};
short i=0,j=0,k=0,star=0,end,hour,min,sec,time_flag,field;
short speed=0,add=0,save_flag=0,view_flag=0,download_flag=0,clear_flag=0,settime_flag=0,menu_flag=0,flag=0,flag_count=0;
static char * menu[] = {"View Log","Set Time","Download Log","Clear Log"};
static char  detail[10][18];
unsigned char clock_reg[3];
unsigned char time[9];
unsigned int key;



void writing_eeprom()
{
    //  write a byte of data to an extranal eeprom  at a specified memory address 
    
    write_external_eeprom(add++,time[0]);   
    write_external_eeprom(add++,time[1]);
    write_external_eeprom(add++,time[3]);
    write_external_eeprom(add++,time[4]);
    write_external_eeprom(add++,time[6]);
    write_external_eeprom(add++,time[7]);
    write_external_eeprom(add++,message[i][0]);
    write_external_eeprom(add++,message[i][1]);
    write_external_eeprom(add++,(speed / 10) + 48);
    write_external_eeprom(add++,(speed % 10) + 48);             
}


// it will read the data stored in an extranal eeprom
void reading_eeprom()
{
    short index=0 , add2=0;
    if(flag_count==1)
    {
        add2 = add;
        end = 10;
    }
    else
    {
        add2 = 0;
        end = add/10;
    }
    for(short main_index=0;main_index<end;main_index++)
    {
        detail[main_index][index++] = main_index+48;
        detail[main_index][index++] = ' ';
        detail[main_index][index++] = read_external_eeprom(add2++);
        detail[main_index][index++] = read_external_eeprom(add2++);
        detail[main_index][index++] = ':';
        detail[main_index][index++] = read_external_eeprom(add2++);
        detail[main_index][index++] = read_external_eeprom(add2++);
        detail[main_index][index++] = ':';
        detail[main_index][index++] = read_external_eeprom(add2++);
        detail[main_index][index++] = read_external_eeprom(add2++);
        detail[main_index][index++] = ' ';
        detail[main_index][index++] = read_external_eeprom(add2++);
        detail[main_index][index++] = read_external_eeprom(add2++);
        detail[main_index][index++] = ' ';
        detail[main_index][index++] = read_external_eeprom(add2++);
        detail[main_index][index++] = read_external_eeprom(add2++);
        detail[main_index][index++] = '\0';
        index=0;
        if(add2==100)
            add2=0;
    }
}


 // It will display the time

static void get_time(void)
{
	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);

	time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
    time[1] = '0' + (clock_reg[0] & 0x0F);
	time[2] = ':';
	time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
	time[4] = '0' + (clock_reg[1] & 0x0F);
	time[5] = ':';
	time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
	time[7] = '0' + (clock_reg[2] & 0x0F);
	time[8] = '\0';
}

void calculate_speed(unsigned short adc_reg_val)
{
    
    speed = (adc_reg_val/10);       
    if(speed>99)            // if speed reaches 99 it show only the speed is 99
        speed=99;
}

void check_matrix_keypad(void)
{   
	unsigned char key;
	key = read_switches(STATE_CHANGE);
    
    if(key==1 && settime_flag==1)       // press key 1  and also set the time flag it print only the time ,min,sec
    {
        
        // to print the time to edit and also increment the hour min and sec
        if(field==0)
        {
            hour++;         // increment the hour
            if(hour==24)
                hour=0;
        }
        else if(field==1)
        {
            min++;
            if(min==60)      // if min reaches 60 it will be reset to 0
                min=0;
        }
        else if(field==2)
        {
            sec++;
            if(sec==60)         // if sec reaches 60 it will be reset to 0
                sec=0;
        }            
        return;
    }
    else if(key==2 && settime_flag==1)      //press key 2
    {
        field++;
        if(field==3)
            field=0;
        return;
    } 
    else if(key==11 && settime_flag==1)             // when key press 11 one time and also set the time flag it store the time log
    {
        
                // to write  the time min and sec and displayed in the clcd
        
        write_ds1307(HOUR_ADDR,((hour/10)<<4 ) | (hour%10));         // set the time   
        write_ds1307(MIN_ADDR, ((min/10)<<4 ) | (min%10));          // set the min
        write_ds1307(SEC_ADDR, ((sec/10)<<4 ) | (sec%10));          // set the sec
        menu_flag=0;                // if key press 12 it come  back to the dash board
        settime_flag=0;             // reset the time flag
        j=0;
        star=0;
        k=0;
        flag=0;             // reset the flag
        time_flag=0;   
        field=0;            //reset the field value
        return;
    }
    else if(key==12 && settime_flag==1)        
    {
        menu_flag=1;                 // if key press 12 it come back to the menu log

        // reset 
        settime_flag=0;             
        j=0;
        star=0;
        k=0;
        flag=0;
        time_flag=0;
        field=0;
        return;
    }
    
/*
        if menu_flag=1 and key press 12 means it come back to the menu log
        if menu_flag=0 and key press 12 means it come back to the dashboard
*/

    if(key==12 && menu_flag!=0)            
    {
        CLEAR_DISP_SCREEN;
        menu_flag--;
        view_flag=0;
        settime_flag=0;
        download_flag=0;
        settime_flag=0;
        j=0;
        star=0;
        k=0;
        flag=0;
        return;
    }   
    
    if(view_flag==1 && key==2 && j<9)
    {
        if(flag_count==1)
            j++;
        else if(flag_count==0 && j<(add/10)-1)
            j++;        
        return;
    }
    else if(view_flag==1 && key==1 && j>0)
    {
        j--;
        return;
    }
        
    if(key==1 && i<=8 && menu_flag==0)
    {
        if(add==100)
        {
            flag_count=1;
            add=0;
        } 
        save_flag=0;
        if(i==7)
            return;
        else if(i==8)
            i=1;
        else
            i++;       
        writing_eeprom();           // when the event is updated it will also stored and it will not store the on condition
    }       
    else if(key==2 && i>1 && menu_flag==0)
    {
        if(add==100)
        {
            flag_count=1;
            add=0;
        } 
        save_flag=0;
        if(i==8)                // i value exactly similar to the 8 it will set to 1  else decrement 
            i=1;
        else
            i--;
        writing_eeprom();           // when the event is updated it will also stored and it will not store the on condition
    }
    else if(key==3 && menu_flag==0)
    {
        if(add==100)
        {
            flag_count=1;
            add=0;
        } 
        i=8;
        
        if(save_flag==0)
            writing_eeprom();           // when the event is updated it will also stored
        save_flag++;
    }
          
    if(key==11 && menu_flag<2)
    {
        menu_flag++;                // increment the menu flag
    }
    
    if(key==2 && k<3 && menu_flag==1)
    {
        if(add==100)
        {
            flag_count=1;
            add=0;
        }
        if(star==0)
        {
            star=1;
            flag++;
            return;
        }
        if(flag!=0)
            k++;
        flag++;
        if(k==3)
        {
            k=2;
            flag=3;
        }
    }
    else if(key==1 && k<3 && menu_flag==1)
    {
        if(add==100)
        {
            flag_count=1;
            add=0;
        }
        if(star==1)
        {
            star=0;
            flag--;
            return;
        }
        if(flag!=3)
            k--;
        flag--;
        if(k==-1)
        {
            k=0;
            flag=0;
        }           
    }
    
    // If the menu flag press twise it enter to the menu log which you want  to see
    
    if(menu_flag==2)
    {
        if(star==0 && k==0)
            view_flag=1;
        else if(star==1 && k==0)
            settime_flag=1;
        else if(star==0 && k==1)
            settime_flag=1;
        else if(star==1 && k==1)
            download_flag=1;
        else if(star==0 && k==2)
            download_flag=1;
        else if(star==1 && k==2)
            clear_flag=1;
                       
    }   
}

static void init_config(void)
{
    init_clcd();
    init_matrix_keypad();
    init_adc(); 
    init_i2c();
	init_ds1307();
    init_uart();
}

void main(void)
{
    unsigned short adc_reg_val;
    init_config();

    while (1)
    {
        check_matrix_keypad();
        adc_reg_val = read_adc(CHANNEL4); 
        calculate_speed(adc_reg_val);               // to calculate the speed
        get_time();                     // to get the time
        
        // It display the Dashboard
        if(menu_flag==0)
        {
            CLEAR_DISP_SCREEN;
            clcd_print("  TIME    EV  SP", LINE1(0)); 
            clcd_print(time, LINE2(0));                 // it will print the time on the clcd
            clcd_print(message[i], LINE2(10));                 // it will print the event when key1 and key2 pressed means it also update
            clcd_putch((speed % 10) + 48, LINE2(15));  // it will print the speed
            clcd_putch((speed / 10) + 48, LINE2(14));
        }
        
        //  It will display the menu log
            
        else if(menu_flag==1)               
        {
            
            // to display the star on the menu log 
            // if the star is set  1 or 0
            
            clcd_print("                  ", LINE1(0));
            clcd_print("                  ", LINE2(0));
            if(star==0)
                clcd_putch('*', LINE1(0));
            else
                clcd_putch('*', LINE2(0));
            
            clcd_print(menu[k], LINE1(2));
            clcd_print(menu[k+1], LINE2(2));
        }
        
        // it display when it was in the default value
        else if(view_flag==1)
        {      
            if(add==0 && flag_count==0)
            {
                unsigned long long int delay=0;                         // if the event is in the on state it will not saved 
                clcd_print("NO LOGS   ", LINE1(0));
                clcd_print("  TO DISPLAY :( ", LINE2(0));
                while(delay++<300000);
                delay=0;
                menu_flag=1;
                k=0;
                star=0;
                view_flag=0;
            }
            else                    // it will store after update the event
            {
                reading_eeprom();                   // it will store the all event
                clcd_print("                  ", LINE1(0));
                clcd_print("                  ", LINE2(0));
                clcd_print("# VIEW LOG : ", LINE1(0));             
                clcd_print(detail[j], LINE2(0));     
            }
        }
        
            // Clear log
        
        else if(clear_flag==1)
        {
            
            
            add=0;
            flag_count=0;
            unsigned long long int delay=0;
            CLEAR_DISP_SCREEN;
            clcd_print("Clearing logs...", LINE1(0));
            clcd_print("Just a minute", LINE2(0));
            while(delay++<300000);                              // It will take some delay than it go automatically to the menu log
            delay=0;
            menu_flag=1;                // make menu flag as 1
            k=0;
            star=0;     // reset
            clear_flag=0;            //reset the clear flag
        }
        
        
        // Downlod Log
        
        else if(download_flag==1)
        {
            unsigned long long int delay=0;
            CLEAR_DISP_SCREEN;           
            if(add==0 && flag_count==0)
            {
                clcd_print("NO LOGS   ", LINE1(0));
                clcd_print("  TO DOWNLOAD :( ", LINE2(0));
                while(delay++<300000);                           // It will take some delay than it go automatically to the menu log
                delay=0;
                menu_flag=1;
                k=0;
                star=0;
                download_flag=0;
            }
            else
            {
                reading_eeprom();                               // to read the data and displayed in the terminal by using through uart
                clcd_print("Downloading..", LINE1(0));
                clcd_print("Through UART..", LINE2(0));               
                for(int it=0;it<end;it++)
                {
                    puts(detail[it]);
                    puts("\n\r");
                }
                while(delay++<300000);                               // It will take some delay than it go automatically to the menu log
                delay=0;
                menu_flag=1;
                k=0;
                star=0;
                download_flag=0;                    // it will reset the downlod flag
            }
        }
        else if(settime_flag==1)
        {
            	
            if(time_flag==0)                // to edit the time like hour,minit,second
            {
                hour = (time[0]-48)*10;
                hour += (time[1]-48);
                min = (time[3]-48)*10;
                min += (time[4]-48);
                sec = (time[6]-48)*10;
                sec += (time[7]-48);
                time_flag=1;
            }
            unsigned long long int delay=0;
            
            //  to display the hour
            
            clcd_print("HH:MM:SS            ", LINE1(0));
            clcd_putch((hour/10) + '0', LINE2(0));
            clcd_putch((hour%10) + '0',LINE2(1));
            if(field==0)
            {
                while(delay++ < 50000);                 // to blink the Hour
                delay=0;
            }
            
            // to display the minit
            clcd_putch(':', LINE2(2));
            clcd_putch((min/10) + '0', LINE2(3));
            clcd_putch((min%10) + '0', LINE2(4));
            if(field==1)
            {
                while(delay++ < 50000);                   // to blink the minut  
                delay=0;
            }
            clcd_putch(':', LINE2(5));
            clcd_putch((sec/10) + '0', LINE2(6));
            clcd_putch((sec%10) + '0', LINE2(7));
            if(field==2)
            {   
                while(delay++ < 50000);                 // to blink the second 
                delay=0;
            }
            clcd_print("      ", LINE2(8)); 
            if(field==0)
            { 
                clcd_print("  ", LINE2(0));
                while(delay++ <50000 );
                delay=0;
            }
            else if(field==1)
            {
                clcd_print("  ", LINE2(3));
                while(delay++ < 50000);
                delay=0;
            }
            else if(field==2)
            {
                clcd_print("  ", LINE2(6));
                while(delay++ < 50000);
                delay=0;
            }   
          
           
        }
      
    }
}