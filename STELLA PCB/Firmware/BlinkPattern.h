#ifndef BLINKPATTERN_H_
#define BLINKPATTERN_H_


void ImpedanceBlinkPattern(Impedance_Status impedanceStatus, Stimulation_Channel channel);
void LowBatteryPattern(void);
void LowImpedancePattern(void);
void HighImpedancePattern(void);
void EverythingIsFinePattern(void);
void StimulationOffPattern(void);
void ErrorDetectionPattern(void);
void ControlLight(void);


#endif
