
double pidLeftP=0, pidLeftI=0, pidLeftD=0, pidRightP=0, pidRightI=0, pidRightD=0;
double leftIn=0, leftOut=0, leftSet=0, rightIn=0, rightOut=0, rightSet=0;
PID leftPID(&leftIn, &leftOut, &leftSet, pidLeftP, pidLeftI, pidLeftD, DIRECT);
PID rightPID(&rightIn, &rightOut, &rightSet, pidRightP, pidRightI, pidRightD, DIRECT);

void PIDDrive(){
  if(leftPID.NeedsCompute()){
    if(iic_encoder_read(ENCODER_LEFT_ADDR,&leftIn)){
    //Successfully read the left encoder
      leftPID.Compute();
    } else {
      //Bus fault!
      leftOut=0;
    }
    SerCommDbg.print("PID(L): ");
    SerCommDbg.print(leftOut);
    drive_left(leftOut);
  }
  if(rightPID.NeedsCompute()){
    if(iic_encoder_read(ENCODER_RIGHT_ADDR,&rightIn)){
    //Successfully read the left encoder
      rightPID.Compute();
    } else {
      //Bus fault!
      rightOut=0;
    }
    SerCommDbg.print("PID(R): ");
    SerCommDbg.print(rightOut);
    drive_right(rightOut);
  }
}

void PIDInit(){
    PIDLoadTunings(); //Load the PID tunings from the EEPROM
    PIDRefreshTunings();
    leftPID.SetMode(MANUAL);
    rightPID.SetMode(MANUAL);
    leftPID.SetSampleTime(PID_SAMPLE_TIME);
    rightPID.SetSampleTime(PID_SAMPLE_TIME);
    leftPID.SetOutputLimits(-PID_OUTPUT_LIMIT,PID_OUTPUT_LIMIT);
    rightPID.SetOutputLimits(-PID_OUTPUT_LIMIT,PID_OUTPUT_LIMIT);
}

void PIDRefreshTunings(){
  leftPID.SetTunings(pidLeftP, pidLeftI, pidLeftD);
  rightPID.SetTunings(pidLeftP, pidLeftI, pidLeftD);  
}

void PIDTuner(){
  //Parse the data relating to the local PID tuner
  static char serialInputBuffer[PID_SERIAL_BUFFER_SIZE];
  static char serialInputBufferIndex = 0;
  static char currentPIDValueIsLeft = 1;
  static double *currentPIDValueToUpdate = &pidLeftP;
  //Serial Input for PID configuration
  if (SerCommDbg.available()) {
    int incomingByte = SerCommDbg.read();
    switch(incomingByte) {
      case 'L':
      case 'l':
        currentPIDValueIsLeft = 1;
        break;
      case 'R':
      case 'r':
        currentPIDValueIsLeft = 0;
        break;
      case 'P':
      case 'p':
        if (currentPIDValueIsLeft) {
          currentPIDValueToUpdate = &pidLeftP;
        }
        else {
          currentPIDValueToUpdate = &pidRightP;
        }
        break;
      case 'I':
      case 'i':
        if (currentPIDValueIsLeft) {
          currentPIDValueToUpdate = &pidLeftI;
        }
        else {
          currentPIDValueToUpdate = &pidRightI;
        }
        break;
      case 'D':
      case 'd':
        if (currentPIDValueIsLeft) {
          currentPIDValueToUpdate = &pidLeftD;
        }
        else {
          currentPIDValueToUpdate = &pidRightD;
        }
        break;
      case 'W':
      case 'w':
        SerCommDbg.println("Writing PID tunings to EEPROM");
        PIDWriteTunings();
        serialInputBufferIndex = 0;
        break;
      case 'M':
      case 'm':
        SerCommDbg.println("Reloading PID tunings from EEPROM");
        PIDLoadTunings();
        serialInputBufferIndex = 0;
        break;
      case '\n':
        serialInputBuffer[serialInputBufferIndex] = '\0';
        *currentPIDValueToUpdate = strtod(serialInputBuffer, NULL);
        SerCommDbg.print("Setting ");
        if (currentPIDValueToUpdate == &pidLeftP) {
          SerCommDbg.print("Left P ");
        }
        if (currentPIDValueToUpdate == &pidLeftI) {
          SerCommDbg.print("Left I ");
        }
        if (currentPIDValueToUpdate == &pidLeftD) {
          SerCommDbg.print("Left D ");
        }
        if (currentPIDValueToUpdate == &pidRightP) {
          SerCommDbg.print("Right P ");
        }
        if (currentPIDValueToUpdate == &pidRightI) {
          SerCommDbg.print("Right I ");
        }
        if (currentPIDValueToUpdate == &pidRightD) {
          SerCommDbg.print("Right D ");
        }
        SerCommDbg.print("to ");
        SerCommDbg.println(*currentPIDValueToUpdate);
        serialInputBufferIndex = 0;
        PIDRefreshTunings();
        break;
      case ' ':
        break;
      default:
        if (serialInputBufferIndex < PID_SERIAL_BUFFER_SIZE - 1) {
          serialInputBuffer[serialInputBufferIndex] = incomingByte;
          serialInputBufferIndex++;
        }
        break;
    }
  }
}

void PIDWriteTunings(){
  //Write the current value of the PID tunings to the EEPROM, which is nonvolatile
  EEPROM.put(FS_LEFT_P_4,pidLeftP);
  EEPROM.put(FS_LEFT_I_4,pidLeftI);
  EEPROM.put(FS_LEFT_D_4,pidLeftD);
  EEPROM.put(FS_RIGHT_P_4,pidRightP);
  EEPROM.put(FS_RIGHT_I_4,pidRightI);
  EEPROM.put(FS_RIGHT_D_4,pidRightD);
}

void PIDLoadTunings(){
  //Load the PID tunings from the EEPROM
  EEPROM.get(FS_LEFT_P_4,pidLeftP);
  EEPROM.get(FS_LEFT_I_4,pidLeftI);
  EEPROM.get(FS_LEFT_D_4,pidLeftD);
  EEPROM.get(FS_RIGHT_P_4,pidRightP);
  EEPROM.get(FS_RIGHT_I_4,pidRightI);
  EEPROM.get(FS_RIGHT_D_4,pidRightD);
  PIDRefreshTunings();

}

