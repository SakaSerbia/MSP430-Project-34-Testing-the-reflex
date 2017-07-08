/**
 * @file lcd16x2.c
 * @author Stefan Tešanović 675/2016
 * @date 01/05/2017
 * @version 1.0
 * @brief Library functions for LCD
 *
 */

#ifndef LCD16X2_H_
#define LCD16X2_H_

void send_command(char kom);
void send_data(char pod);
void send_position(char y, char x);
void send_string(char *string);
void send_number(unsigned long br);

#endif /* LCD16X2_H_ */
