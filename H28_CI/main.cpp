/*
 * H28_CI.cpp
 *
 * Created: 2016/06/14 16:06:04
 * Author : sn
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <H28_AVR/H28_AVR.h>
#include <FallMoon/akilcd_ci.h>

#define DATA_NUM_MAIN 4

#define LED_SW_0	EI_IO0
#define LED_SW_1	EI_IO1
#define LED_SW_2	EI_IO2
#define LED_SW_3	EI_IO3
#define LED_LINK_0	EI_IO4
#define LED_OUT_0	EI_IO5
#define LED_LINK_1	EI_IO6
#define LED_OUT_1	EI_IO7

#define	PRIORITY_O 1
#define PRIORITY_1 2

#define	SIGN_LINK	 0
#define	SIGN_STATE_0 1
#define	SIGN_STATE_1 2
#define	SIGN_DATA	 3

E_LOGIC F_In_BT_slave(C_UART_R &_arg_uart_r, T_DATA _arg_re_in_data[DATA_NUM_MAIN])
{	
	T_NUM num_data = 0;
	
	_arg_uart_r.Check();
	
//	wdt_reset();
	
	if (_arg_uart_r.Ret_flag() == EU_ERROR)
	{
		_arg_re_in_data[0] = 0x3f;
		_arg_re_in_data[1] = 0x7f;
		_arg_re_in_data[2] = 0x8f;
		_arg_re_in_data[3] = 0xc0;
		
		return FALES;
	}
	
	while (num_data != 0x0f)
	{
		T_DATA temp_data = _arg_uart_r.In();
		
		if (temp_data != IN_ERROR)
		{
			_arg_re_in_data[(temp_data >> 6) & 3] = temp_data;
			
			num_data |= (1 << ((temp_data >> 6) & 3));
		}
		else
		{
			break;
		}
	}
	
//	wdt_reset();
	
	return TRUE;
}

inline void F_Out_main(C_UART_T &_arg_uart_t, const T_DATA _arg_out_data[DATA_NUM_MAIN])
{
	for (usint i = 0; i < DATA_NUM_MAIN; i++)
	{
		_arg_uart_t.Out(_arg_out_data[i]);
		
		_delay_us(20);
	}
	
//	wdt_reset();
}

int main(void)
{
	cli();
	
	C_UART_R _uart_r_bt_0(EU_UART0);
	C_UART_R _uart_r_bt_1(EU_UART1);
	
	C_UART_T _uart_t_main(EU_UART0);
	
	C_IO_OUT _io_o_led(EI_PORTA,0xff);
	
	C_IO_IN _io_i_sw(EI_PORTB,0x1e);
	
	C_IO_IN_pin _io_i_link_0(EI_PORTD,EI_IO4);
	C_IO_IN_pin _io_i_link_1(EI_PORTD,EI_IO5);;
	
//	wdt_reset();
//	WDTCSR |= ((1<<WDCE)|(1<<WDE));
//	WDTCSR = ((1<<WDE)|(1<<WDP2)|(1<<WDP0));	//wdt0.5[s]
	
//	sei();
	
	INI_LCD();
	
	const T_DATA _arr_temp_o_data[DATA_NUM_MAIN] = {0x3f,0x7f,0x8f,0xc0};
	
    while (1) 
    {
		T_PORT _data_led = ((_io_i_sw.In() >> 1) & 0x0f);
		
		const T_NUM _priority_num = _data_led & 0x03;
		
		const T_NUM _display_num  = (_data_led >> 2) & 0x03;
		
		const E_LOGIC _flag_link_0 = _io_i_link_0.In();
		const E_LOGIC _flag_link_1 = _io_i_link_1.In();
		
		_data_led |= ((_flag_link_0 << LED_LINK_0) | (_flag_link_1 << LED_LINK_1));
		
		T_DATA _arr_i_data_bt_0[DATA_NUM_MAIN] = {0x3f,0x7f,0x8f,0xc0};
		E_LOGIC _flag_succe_0 = FALES;
			
		T_DATA _arr_i_data_bt_1[DATA_NUM_MAIN] = {0x3f,0x7f,0x8f,0xc0};
		E_LOGIC _flag_succe_1 = FALES;

		if (_flag_link_0 == TRUE)
		{
			_flag_succe_0 = F_In_BT_slave(_uart_r_bt_0,_arr_i_data_bt_0);
		}
		
		if (_flag_link_1 == TRUE)
		{
			_flag_succe_1 = F_In_BT_slave(_uart_r_bt_1,_arr_i_data_bt_1);
		}
		
//		wdt_reset();
		
		switch (_priority_num)
		{
			case PRIORITY_O:
			{
				if (_flag_succe_0 == TRUE)
				{
					F_Out_main(_uart_t_main,_arr_i_data_bt_0);
					_data_led |= (1 << LED_OUT_0);
				}
				else if (_flag_succe_1 == TRUE)
				{
					F_Out_main(_uart_t_main,_arr_i_data_bt_1);
					_data_led |= (1 << LED_OUT_1);
				}
				else
				{
					F_Out_main(_uart_t_main,_arr_temp_o_data);
				}
				
				break;
			}
			case PRIORITY_1:
			{
				if (_flag_succe_1 == TRUE)
				{
					F_Out_main(_uart_t_main,_arr_i_data_bt_1);
					_data_led |= (1 << LED_OUT_1);
				}
				else if (_flag_succe_0 == TRUE)
				{
					F_Out_main(_uart_t_main,_arr_i_data_bt_0);
					_data_led |= (1 << LED_OUT_0);
				}
				else
				{
					F_Out_main(_uart_t_main,_arr_temp_o_data);
				}
				
				break;
			}
			default:
			{
				if (_flag_succe_0 == TRUE)
				{
					F_Out_main(_uart_t_main,_arr_i_data_bt_0);
					_data_led |= (1 << LED_OUT_0);
				}
				else if (_flag_succe_1 == TRUE)
				{
					F_Out_main(_uart_t_main,_arr_i_data_bt_1);
					_data_led |= (1 << LED_OUT_1);
				}
				else
				{
					F_Out_main(_uart_t_main,_arr_temp_o_data);
				}
				
				break;
			}
		}
		
//		wdt_reset();
		
		Lcd_clr_dis();
		
		switch (_display_num)
		{
			case SIGN_DATA:
			{
				Lcd_put_str(0x00 ,"RX_0");
				Lcd_put_str(0x40 ,"RX_1");
				
				for (usint i = 0; i < DATA_NUM_MAIN; i++)
				{
					Lcd_put_num(0x05+(i*3), _arr_i_data_bt_0[i], 2, ED_16);
					Lcd_put_num(0x45+(i*3), _arr_i_data_bt_1[i], 2, ED_16);
				}
				
				break;
			}
			case SIGN_LINK:
			{
				if (_flag_link_0 == TRUE)
				{
					Lcd_put_str(0x00, "RX_0 connect");
				}
				else
				{
					Lcd_put_str(0x00, "RX_0 disconnect");
				}
				
				if (_flag_link_1 == TRUE)
				{
					Lcd_put_str(0x40, "RX_1 connect");
				}
				else
				{
					Lcd_put_str(0x40, "RX_1 disconnect");
				}
				
				break;
			}			
			case SIGN_STATE_0:
			{
				if (_flag_link_0 == TRUE)
				{
					Lcd_put_str(0x00, "RX_0 connect");
				}
				else
				{
					Lcd_put_str(0x00, "RX_0 disconnect");
				}
				
				for (usint i = 0; i < DATA_NUM_MAIN; i++)
				{
					Lcd_put_num(0x40+(i*3), _arr_i_data_bt_0[i], 2, ED_16);
				}
				
				break;
			}
			case SIGN_STATE_1:
			{
				if (_flag_link_1 == TRUE)
				{
					Lcd_put_str(0x00, "RX_1 connect");
				}
				else
				{
					Lcd_put_str(0x00, "RX_1 disconnect");
				}
				
				for (usint i = 0; i < DATA_NUM_MAIN; i++)
				{
					Lcd_put_num(0x40+(i*3), _arr_i_data_bt_1[i], 2, ED_16);
				}
				
				break;
			}
		}
		
//		wdt_reset();
		
		if ((_flag_link_0 & _flag_link_1) == FALES)	_delay_ms(100);
		
		_io_o_led.Out(_data_led);
    }
	
	return 0;
}

