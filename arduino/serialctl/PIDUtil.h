#include "eeprom_fs.h"
#include <EEPROM.h>
#define PID_OUTPUT_LIMIT 255
#define PID_SAMPLE_TIME 20
#define ENCODER_LEFT_ADDR 5
#define ENCODER_RIGHT_ADDR 6

void PIDWriteTunings();
void PIDLoadTunings();
void PIDTuner();
void PIDInit();
void PIDDrive();
void PIDRefreshTunings();
extern PID leftPID;
extern PID rightPID;
