/*
 * cc2500.h
 *
 *  Created on: 18.02.2020
 *      Author: ma439
 */

#ifndef CC2500_H_
#define CC2500_H_

extern uint8_t packet[11];
void spi_init(void);
void GDO0Setup();
//void cc2500_write_reg(uint8_t address,uint8_t data);
int cc2500_read_reg(uint8_t address);
int cc2500_SPIReadStatus(uint8_t addr);
void cc2500_reset(void);
void cc2500_strobe(uint8_t strobe);
void cc2500_init(void);
void cc2500_receive2();
void led_init_match(void);
void led_init_nmatch(void);
//uint8_t *cc2500_receive();
//uint8_t *cc2500_receive_chk();
uint8_t *cc2500_receive_wor();
void cc2500_transmit();
void cc2500_transmit_arr(uint8_t l, volatile uint8_t d[]);
void cc2500_transmit_arr_new(uint8_t l, volatile uint8_t d[]); //used for transferring data after receiving packet
void cc2500_wor_init();
void cc2500_nwor_init();
uint8_t cc2500_strobe1(uint8_t strobe);
void pwd_mode_cc2500(void);
void awake_cc2500(void);
void Rx_on_cc2500(void);
void Rx_idle_cc2500(void);




#endif /* CC2500_H_ */
