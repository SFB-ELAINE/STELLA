#ifndef COMPARATOR_H_
#define COMPARATOR_H_

void EnableComparator(void);
void DisableComparator(void);
Impedance_Status ImpedanceCheck(int *VBoostArray, Stimulation_Channel channel, Stimulation_Type type);
int _compare(Stimulation_Channel channel, int *VBoostArray);

#endif
