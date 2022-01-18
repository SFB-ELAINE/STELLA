#include <stdint.h> //included for uint16_t
#ifndef COMPARATOR_H_
#define COMPARATOR_H_

void EnableComparator(void);
void DisableComparator(void);
Impedance_Status ImpedanceCheck(int *VBoostArray, Stimulation_Channel channel, Stimulation_Type type);
int _compare(Stimulation_Channel channel, int *VBoostArray, uint16_t Freq_compare, uint8_t pack_rec);

//timer and DBS freq functions
void setupTimer_freq(void);
void stopTimer_freq(void);
unsigned int dbs_freq(Stimulation_Channel channel, uint16_t Freq_compare);
unsigned int dbs_pw(Stimulation_Channel channel, uint16_t Freq_compare);
void _compare_helper(Stimulation_Channel channel, uint16_t Freq_compare);
void sample_dbs(Stimulation_Channel channel, unsigned int Wiper_new, uint16_t Freq_compare);
//volatile int VRef_test = 0x00;

#endif
