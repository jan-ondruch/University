// Project: FITkit svetelne noviny
// Author: Jan Ondruch, xondru14
// Class: IMP
// Date: 4.12.2017

// Used files and changes
// program: Demo - Klávesnice a LCD
//			used files - main.c (90% changes, implementation of the keyboard logic)
//					   - project.xml (30% changes, content of attributes)
//					   - top_level.vhd (0% changes, usage of the whole file)


#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>

#define INPUT 1		// user input
#define OUTPUT 2	// FITkit output

#define INPUT_REPETITIONS 512		// loop to enable keyboard interrupt
#define ANIMATION_REPETITIONS 64	// loop to enable animation interrupt
#define TEXT_LEN 256				// length of input text
#define DISPLAY_SIZE 32				// FITkit display size

// info & warning messages
const char msg_max_len_exceeded[] = "Maximalni delka textu presazena!";
const char msg_welcome[] = "Vitejte v appce svetelne noviny!";
const char msg_animation[] = "Ukazka animace!";

const int ap = 15;	// animation pointer

int mode;		// mode of the newspaper
char last_ch;	//naposledy precteny znak

char text[TEXT_LEN];	// input text
char tp = 0;			// identify position in text


/**
 * Prints help.
 * Mandatory function.
 */
void print_user_help(void) { }


/**
 * User command decoding.
 * Mandatory function.
 */
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return (CMD_UNKNOWN);
}


/**
 * Prints typed in char on the display.
 * If the screen is full, shifts it.
 */
void print_char(char ch)
{
	if (tp <= DISPLAY_SIZE) LCD_append_char(ch);
	else
	{
		LCD_clear();
		int i;
		for (i = 1; i < DISPLAY_SIZE; ++i)
		{
			LCD_append_char(text[tp - DISPLAY_SIZE + i]);
		}
	}
}


/**
 * Decodes user input like a classic mobile phone keypad.
 * Return corresponding char based on the given key and count
 * of times it was pressed.
 */
unsigned char decode_user_input(char key, unsigned int cnt)
{
	switch(key)
	{
		case '1':
			cnt = cnt % 4;
			if (cnt == 0) return '.';
			if (cnt == 1) return '?';
			if (cnt == 2) return '!';
			else return '1';
		case '2':
			cnt = cnt % 4;
			if (cnt == 0) return 'a';
			else if (cnt == 1) return 'b';
			else if (cnt == 2) return 'c';
			else return '2';
		case '3':
			cnt = cnt % 4;
			if (cnt == 0) return 'd';
			else if (cnt == 1) return 'e';
			else if (cnt == 2) return 'f';
			else return '3';
		case '4':
			cnt = cnt % 4;
			if (cnt == 0) return 'g';
			else if (cnt == 1) return 'h';
			else if (cnt == 2) return 'i';
			else return '4';
		case '5':
			cnt = cnt % 4;
			if (cnt == 0) return 'j';
			else if (cnt == 1) return 'k';
			else if (cnt == 2) return 'l';
			else return '5';
		case '6':
			cnt = cnt % 4;
			if (cnt == 0) return 'm';
			else if (cnt == 1) return 'n';
			else if (cnt == 2) return 'o';
			else return '6';
		case '7':
			cnt = cnt % 5;
			if (cnt == 0) return 'p';
			else if (cnt == 1) return 'q';
			else if (cnt == 2) return 'r';
			else if (cnt == 3) return 's';
			else return '7';
		case '8':
			cnt = cnt % 4;
			if (cnt == 0) return 't';
			else if (cnt == 1) return 'u';
			else if (cnt == 2) return 'v';
			else return '8';
		case '9':
			cnt = cnt % 5;
			if (cnt == 0) return 'w';
			else if (cnt == 1) return 'x';
			else if (cnt == 2) return 'y';
			else if (cnt == 3) return 'z';
			else return '9';
		case '0':
			cnt = cnt % 2;
			if (cnt == 0) return ' ';
			else return '0';
	}

	return ' '; // invalid state
}


/**
 * FPGA initialization.
 * Mandatory function.
 */
void fpga_initialized() { }


/**
 * Animation interrupt.
 * Check if other keys are pressed during the animation.
 */
unsigned char animation_interrupt()
{
	char ch;

	int lc = 0;	// loop counter
	while (lc < INPUT_REPETITIONS)
	{
		ch = key_decode(read_word_keyboard_4x4());
		
		if (ch != 0)
		{
			// mode has been changed
			if (ch == '*')
			{
				mode = INPUT;
				return ch;
			}
			// animation will be switched to another animation
			else if (ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D') return ch;
		}

		delay_ms(2);
		lc++;
	}

	return '0';
}


/**
 * Animation A - blinking text.
 * Until the mode is set to a OUTPUT mode,
 * animate something.
 */
char animation_A()
{
	char ch;
	unsigned int dc = 0;	// display counter

	while (mode == OUTPUT)
	{
		LCD_clear();

		if ((ch = animation_interrupt()) != '0') return ch;
		
		// nothing to display, let's show the message
		if (tp == 0) LCD_write_string(msg_animation);
		else
		{
			int i;
			for (i = dc; i <= tp-1; i++)
			{
				LCD_append_char(text[i]);
			}
			dc++;
			if (tp-dc < DISPLAY_SIZE) dc = 0;
			}

		if ((ch = animation_interrupt()) != '0') return ch;
	}

	return '*';
}


/**
 * Animation B - animation right.
 * Until the mode is set to a OUTPUT mode,
 * animate something.
 */
char animation_B()
{
	char ch;

	while (mode == OUTPUT)
	{
		LCD_clear();

		if ((ch = animation_interrupt()) != '0') return ch;
		
		int i, j;

		// nothing to display, let's show the message
		if (tp == 0)
		{
			for (i = 0; i <= ap-1; i++)
			{
				for (j = ap-1-i; j < ap; j++)
				{
					LCD_append_char(msg_animation[j]);
				}
				if ((ch = animation_interrupt()) != '0') return ch;
				LCD_clear();
			}
		}
		else
		{
			for (i = 0; i <= tp-1; i++)	// number of displayed chars on the display
			{
				for (j = tp-1-i; j < tp; j++)	// display chars in the reversed order
				{
					LCD_append_char(text[j]);
				}
				if ((ch = animation_interrupt()) != '0') return ch;
				LCD_clear();
			}
		}
	}

	return '*';
}


/**
 * Animation C - animation left.
 * Until the mode is set to a OUTPUT mode,
 * animate something.
 */
char animation_C()
{
	char ch;

	while (mode == OUTPUT)
	{
		LCD_clear();

		if ((ch = animation_interrupt()) != '0') return ch;
		
		// nothing to display, let's show the message
		if (tp == 0)
		{
			int i, j, k;
			for (i = 0; i <= ap; i++)
			{
				// fill in spaces if the text is shorter than display size
				if (i <= DISPLAY_SIZE-1)
				{
					// spaces
					for (k = 0; k < DISPLAY_SIZE-i; k++)
					{
						LCD_append_char(' ');
					}

					// text
					for (j = 0; j <= i; j++)
					{
						LCD_append_char(msg_animation[j]);
					}
				}
				else
				{
					for (j = 0; j <= DISPLAY_SIZE; j++)
					{
						LCD_append_char(msg_animation[i-DISPLAY_SIZE+j]);
					}
				}
				
				if ((ch = animation_interrupt()) != '0') return ch;
				LCD_clear();
			}
		}
		else
		{
			int i, j, k;
			for (i = 0; i <= tp; i++)
			{
				// fill in spaces if the text is shorter than display size
				if (i <= DISPLAY_SIZE-1)
				{
					// spaces
					for (k = 0; k < DISPLAY_SIZE-i; k++)
					{
						LCD_append_char(' ');
					}

					// text
					for (j = 0; j <= i; j++)
					{
						LCD_append_char(text[j]);
					}
				}
				else
				{
					for (j = 0; j <= DISPLAY_SIZE; j++)
					{
						LCD_append_char(text[i-DISPLAY_SIZE+j]);
					}
				}
				
				if ((ch = animation_interrupt()) != '0') return ch;
				LCD_clear();
			}
		}
	}

	return '*';
}


/**
 * Animation D - animation right + blinking.
 * Until the mode is set to a OUTPUT mode,
 * animate something.
 */
char animation_D()
{
	char ch;

	while (mode == OUTPUT)
	{
		LCD_clear();

		if ((ch = animation_interrupt()) != '0') return ch;
		
		int i, j;

		// nothing to display, let's show the message
		if (tp == 0)
		{
			for (i = 0; i <= ap-1; i++)
			{
				for (j = ap-1-i; j < ap; j++)
				{
					LCD_append_char(msg_animation[j]);
				}
				if ((ch = animation_interrupt()) != '0') return ch;
				LCD_clear();

				delay_ms(200);	// wait and then draw again = blink

				for (j = ap-1-i; j < ap; j++)
				{
					LCD_append_char(msg_animation[j]);
				}
				if ((ch = animation_interrupt()) != '0') return ch;
				LCD_clear();
			}
		}
		else
		{
			for (i = 0; i <= tp-1; i++)
			{
				for (j = tp-1-i; j < tp; j++)
				{
					LCD_append_char(text[j]);
				}
				if ((ch = animation_interrupt()) != '0') return ch;
				LCD_clear();
				
				delay_ms(200);	// wait and then draw again = blink

				for (j = tp-1-i; j < tp; j++)
				{
					LCD_append_char(text[j]);
				}
				if ((ch = animation_interrupt()) != '0') return ch;
				LCD_clear();
			}
		}
	}

	return '*';
}


/**
 * Repeat animation if the mode is set to OUTPUT
 * and the character is valid.
 */
int repeat_animation(char r)
{
	if (r != 0) return 1; 
	else return 0;
}


/**
 * Select animation.
 * If currently running animatinon was interrupted,
 * check for the pressed char and in case it's another
 * animation -> animate it.
 */
void select_animation(char ch)
{
	int repeat;	// repetition check
	char result;	// animation result

	switch(ch)
	{
		case 'A':
			result = animation_A();
			repeat = repeat_animation(result);
			
			if (repeat == 1) select_animation(result);

			break;
		case 'B':
			result = animation_B();
			repeat = repeat_animation(result);
			
			if (repeat == 1) select_animation(result);
			
			break;
		case 'C':
			result = animation_C();
			repeat = repeat_animation(result);
			
			if (repeat == 1) select_animation(result);
			
			break;
		case 'D':
			result = animation_D();
			repeat = repeat_animation(result);
			
			if (repeat == 1) select_animation(result);
			
			break;
		case '*':
			break;
		default:
			break;
	}
}


/**
 * Text array is full.
 * Clear it all and print out a warining message.
 */
void clear_text()
{
	LCD_clear();
	
	// output warning message
	LCD_write_string(msg_max_len_exceeded);
	delay_ms(3000);
	LCD_clear();

	tp = 0;
}


/**
 * Read keyboard state from SPI.
 * Keyboard manipulation is represented by a finite state automaton.
 */
int keyboard_idle()
{
	char ch;
	ch = key_decode(read_word_keyboard_4x4());	// read key
  
	if (ch != 0) // if a key was pressed
	{
		// user input text
		if (ch == '*')
		{
			mode = INPUT;
			return 0;
		}

		// save text - animations are possible now
		else if (ch == '#')
		{
			mode = OUTPUT;
			return 0;
		}

		// output of a text and we can animate it
		else if (ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D')
		{
			if (mode == OUTPUT)
			{
				select_animation(ch);

				// display back the text
				if (tp < DISPLAY_SIZE-1)
				{
					LCD_clear();
					LCD_write_string(text);
				}
				else
				{
					LCD_clear();

					int i;
					for (i = 1; i < DISPLAY_SIZE; ++i)
					{
						LCD_append_char(text[tp - DISPLAY_SIZE + i]);
					}
				}

				return 0;
			}

			if (mode == INPUT && ch == 'D')
			{
				LCD_clear();
				
				int i;
				for (i = 0; i < TEXT_LEN; i++)
				{
					text[i] = 0;
				}
				
				tp = 0;

				return 0;
			}

			return 0;
		}

		// animation has been selected
		// pressing irrelevant keys has no effect
		else if (mode == OUTPUT)
		{
			return 0;
		}

		// some text is being typed
		else
		{
			// simulate mobile phone way of typing
			char key_first, key_prev;
			unsigned int key_press_cnt = 0;	// how many times was the same key pressed
			key_first = ch;	// the initial key

			int lc = 0;	// loop counter
			while (mode == INPUT && lc < INPUT_REPETITIONS)
			{
				key_prev = ch;
				ch = key_decode(read_word_keyboard_4x4());
				
				if (ch != 0 && ch != 'A' && ch != 'B' && ch != 'C' && ch != 'D')
				{
					if (ch == '#')
					{
						mode = OUTPUT;
						return 0;
					}
					else if (ch == key_first && ch != key_prev)
					{
						lc = 0;
						key_press_cnt++;
					}
					else if (ch != key_first)
					{
						key_first = decode_user_input(key_first, key_press_cnt);

						text[tp++] = key_first;
						print_char(key_first);

						key_first = ch;
						key_prev = ch;
						key_press_cnt = 0;
						
						lc = 0;

						// text is full, clear it all
						if (tp >= TEXT_LEN-1)
						{
							clear_text();
							return 0;
						}
					}
				}

				delay_ms(2);
				lc++;
			}
			
			// out of the waiting loop
			ch = decode_user_input(key_first, key_press_cnt);
			text[tp++] = ch;

			// text is full, clear it all
			if (tp >= TEXT_LEN-1)
			{
				clear_text();
			}

			// append the decoded key
			print_char(ch);
		}
	}
  
	return 0;
}


int main()
{
	WDG_stop();  // stop watchdog

	// init the app
	initialize_hardware();
	keyboard_init();

	// init LCD and clear it
	LCD_init();
	LCD_clear();
	
	CCR0 = 0x4000;	// interrupt every 1/2 second (every 16384 ticks) [32768HZ]
	mode = INPUT;  // start by writing in stuff

	// output welcome message
	LCD_write_string(msg_welcome);
	delay_ms(3000);
	LCD_clear();

	while (1)
	{
		delay_ms(2);

		keyboard_idle();
		terminal_idle(); // terminal interaction
	}
}
