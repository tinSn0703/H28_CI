/*
 * H28_CI.cpp
 *
 * Created: 2016/06/14 16:06:04
 * Author : sn
 */ 

#include <avr/io.h>
#include <H28_AVR.h>
#include <FallMoon/akilcd_ci.h>

#define led_sw_0	EN_IO0
#define led_sw_1	EN_IO1
#define led_sw_2	EN_IO2
#define led_sw_3	EN_IO3
#define led_link_0	EN_IO4
#define led_out_0	EN_IO5
#define led_link_1	EN_IO6
#define led_out_1	EN_IO7

void F_In_BT_slave(C_UART_R _arg_uart_r,T_DATA _arg_in_data[])
{	
	T_NUM num_data = 0;
	
	_arg_uart_r.Check();
	
	if (_arg_uart_r.Ret_flag() == EF_ERROR)
	{
		_arg_in_data[0] = 0x3f;
		_arg_in_data[1] = 0x7f;
		_arg_in_data[2] = 0x8f;
		_arg_in_data[3] = 0xc0;
		
		return (void)0;
	}
	
	while (num_data != 0x0f)
	{
		T_DATA temp_data = _arg_uart_r.In();
		
		_arg_in_data[(temp_data >> 6) & 3] = temp_data;
		
		num_data |= (1 << ((temp_data >> 6) & 3));
	}
}

int main(void)
{
	C_UART_R uart_r_bt[2] = 
	{
		C_UART_R(EN_UART0),
		C_UART_R(EN_UART1),
	};
	
	C_UART_T uart_t_main(EN_UART0);
	
	C_IO_OUT io_o_led(EN_PORTA,0xff);
	
	C_IO_IN io_i_sw(EN_PORTB,0x1e);
	
	C_IO_IN_pin io_i_link[2] =
	{
		C_IO_IN_pin(EN_PORTD,EN_IO4),
		C_IO_IN_pin(EN_PORTD,EN_IO5)
	};
	
	INI_LCD();
	
	const T_DATA data_o_standard[4] = {0x3f,0x7f,0x8f,0xc0};
	
    while (1) 
    {
		T_PORT sw = ((io_i_sw.In() >> 1) & 0x0f);
		
		E_LOGIC link[2] = 
		{
			io_i_link[0].In(),
			io_i_link[1].In()
		};
    }
}

