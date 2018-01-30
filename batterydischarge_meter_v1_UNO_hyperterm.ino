/*--------------------------------------------------------------
  Program:      volt_measure

  Description:  Reads value on analog input A2 and calculates
                the voltage assuming that a voltage divider
                network on the pin divides by 11.
  
  Hardware:     Arduino Uno with voltage divider on A2.
                
  Software:     Developed using Arduino 1.0.5 software
                Should be compatible with Arduino 1.0 +

  Date:         22 May 2013
 
  Author:       W.A. Smith, http://startingelectronics.org
--------------------------------------------------------------*/
//FINAL BETA!!! Awesome enabled by B Money 1/2016

//
//
// number of analog samples to take per reading
#define NUM_SAMPLES 10

float Aref = 5.11; //DEFAULT CAN BE CHANGED WHEN RUNNING
/////////////////Variables for Time estimate////////////////////
float floatV = 0;
float V1 = 0;
float V2 = 0;
float Mdisch = 0;
float Mcut = 0;
float Vd = 0;
float Vcut = 44.5;
float Tr = 0;
////////////////////////////////////////////////////////////////
////////////////Variables for Coup De Fouet/////////////////////
float New_trough = 0;
float troughCount = 0;
float trough = 100;
float New_plateau = 0;
float plateauCount = 0;
float plateau = 0;
int V1readStarttime = 0;
////////////////////////////////////////////////////////////////
int tic = 10;
float devider = 11;//10.07325929;
int counter = 0;
int sum = 0;                    // sum of samples taken
unsigned char sample_count = 0; // current sample number
float voltage = 0.0;            // calculated voltage
int scrollLine = 80;
int graphV = 0;
int graphM = 0;
void setup()
{
    //analogReference(INTERNAL);
    Serial.begin(9600);
    delay(2000);
    Serial.println ("Battery discharge routine. For Flooded Batteries Only!");
    delay(500);
    Serial.println ("V= Hyperterm");
    printFloatVoltageMessage();
    floatV = waitForFloat();
    floatV = floatV/11;
    //Serial.print ("floatV / 11 = ");
    //Serial.println (floatV);
    floatV= floatV*1024;
    //Serial.print ("floatV * 1024 = ");
    //Serial.println (floatV);
    while (sample_count < NUM_SAMPLES) {
        sum += analogRead(A2);
        sample_count++;
        delay(20);
    }
    voltage = ((float)sum / (float)NUM_SAMPLES);
    //Serial.print ("Voltage = ");
    //Serial.println (voltage);
    Aref = floatV/voltage;
    //Serial.print ("Aref = ");
    //Serial.println (Aref);
    Serial.println (" ");
    int h;
    for (h=0; h<9; h++){
      Serial.print ("$");
      delay(500);
    }
    Serial.println();
    Serial.println ("Claibration complete!"); 
    delay(500); 
    Serial.println ("You will be asked to type OK....");
    delay(500);
    Serial.println ("Once this happens you will enter into a 10 second countdown");
    delay(500);
    Serial.println ("After that the readings will be measured and posted here.");
    delay(2000); 
    // take a number of analog samples and add them up
    while (sample_count < NUM_SAMPLES) {
        sum += analogRead(A2);
        sample_count++;
        delay(20);
    }
    // calculate the voltage
    // use 5.0 for a 5.0V ADC reference voltage
    // 5.05V is the calibrated reference voltage
    voltage = (((float)sum / (float)NUM_SAMPLES) * Aref) / 1024.0;
    // send voltage for display on Serial Monitor
    // voltage multiplied by 11 when using voltage divider that
    // divides by 11. 11.132 is the calibrated voltage divide
    // value  47.5905 / 4.7169 = 10.0892
    floatV= (voltage * devider);
    Serial.print ("Float Voltage = ");
    Serial.print(floatV);
    //result = (voltage * 10.0892);
    Serial.println (" V");
    Serial.println ("This is the Measured Float Voltage, if you are statisfied...");
    sample_count = 0;
    sum = 0;
    waitForOk();
    Serial.println ("TURN OFF RECTIFIERS!");
    delay(1000);
    while (!(tic == 0)){ 
    Serial.print (tic); 
    Serial.print (" ");
    tic = tic-1;
    delay (1000);
    }
    Serial.println ();
    tic = 10;
}

void loop()
{
   counter++;
    // take a number of analog samples and add them up
    while (sample_count < NUM_SAMPLES) {
        sum += analogRead(A2);
        sample_count++;
        delay(20);
    }
    // calculate the voltage
    // use 5.0 for a 5.0V ADC reference voltage
    // 5.05V is the calibrated reference voltage
    voltage = ((float)sum / (float)NUM_SAMPLES * Aref) / 1024.0;
    // send voltage for display on Serial Monitor
    // voltage multiplied by 11 when using voltage divider that
    // divides by 11. 11.132 is the calibrated voltage divide
    // value  47.5905 / 4.7169 = 10.0892
    //Serial.print(voltage * 10.0892);
    //result = (voltage * 10.0892);
    //Serial.println (" V");
    sample_count = 0;
    sum = 0;
    CDFtrough();
    CDFplateau();
    V1readTimer();
    if (counter ==  1) {
      //Serial.println ("************************");
      line();
      center();
      Serial.println ("START READINGS");
      //Serial.println ("************************");
      line();
    }
    if (counter == 3){
      Serial.print (" Float V =");
      space();
      Serial.print (floatV);
      Serial.print (" V");
      graphV = map ((floatV), 45,54,0,9);
      //Serial.print ("graphV (should be 1-10)= "); Serial.println (graphV);
      graphM = map (graphV,0,9,45,54);
      //Serial.print ("bring graph back from 1-10 to voltage 45-52.  = "); Serial.println (graphM);
      graphM = ((floatV)-graphM)*10;
      //Serial.print (" this should strip away voltage leaving a decimal value * 10...= "); Serial.println (graphM);
      graphM = map (graphM,1,9,1,9);
      //Serial.print ("graphM = "); Serial.println (graphM);
      int i;
      for(i=0; i<(graphV *  6 ); i++) {
        //for(i=0; i<(graphV *  10 ); i++) {
        Serial.print (":");
      }
      int j;
      for (j=0; j<(graphM);j++) {
        Serial.print (":");
      }
        Serial.println();
    }
    if (counter == 30) {
      Serial.print ("30 second's =");
      space3();
      Print();
    }
    if (counter == 60) {
      Serial.print ("1 Minute =");
      space();
      Print();
    }
    if (counter == 90) {
      Serial.print ("1 1/2 Minute =");
      space4();
      Print();
    }
    if (counter == 120) {
      Serial.print ("2 Minute =");
      space();
      Print();
    }
    if (counter == 135) {
      Serial.print ("2 1/4 Minute =");
      space4();
      Print();
    }
    if (counter == 150) {
      Serial.print ("2 1/2 Minute =");
      space4();
      Print();
    }
    if (counter == 165) {
      Serial.print ("2 3/4 Minute =");
      space4();
      Print();
    }
    if (counter == 180) {
      Serial.print ("3 Minute =");
      space();
      Print();
    }
    if (counter == 210) {
      Serial.print ("3 1/2 Minute =");
      space4();
      Print();
    }
    if (counter == 240) {
      Serial.print ("4 Minute =");
      space();
      Print();
    }
    if (counter == 270) {
      Serial.print ("4 1/2 Minute =");
      space4();
      Print();
    }
    if (counter == 300) {
      Serial.print ("5 Minute =");
      space();
      Print();
    }
    if (counter == 600) {
      //Serial.println ("************************");
      line();
      Serial.print ("10 Minute =");
      space2();
      Print();
    }
    if (counter == 900) {
      Serial.print ("15 Minute =");
      space2();
      Print();
    }
    if (counter == 1200) {
      Serial.print ("20 Minute =");
      space2();
      Print();
    }
    if (counter == 1500) {
      Serial.print ("25 Minute =");
      space2();
      Print();
    }
    if (counter == 1800) {
      Serial.print ("30 Minute =");
      space2();
      Print();
    }
    if (counter == 2100) {
      Serial.print ("35 Minute =");
      space2();
      Print();
    }
    if (counter == 2400) {
      Serial.print ("40 Minute =");
      space2();
      Print();
    }
    if (counter == 2700) {
      Serial.print ("45 Minute =");
      space2();
      Print();
    }
    /////////////////////////////////////////////////////////////////////
   /* if (counter == 2760) {//46 minute
      V1 = (voltage * devider);
      //Serial.println ("************************");
      line();
      Serial.print ("V1 battery read =      ");
      Serial.print (V1);
      Serial.println (" V");  
      //Serial.println ("************************");
      line();
    }*/
    if (counter == 3000) {
      Serial.print ("50 Minute =");
      space2();
      Print();
    }
    if (counter == 3300) {
      Serial.print ("55 Minute =");
      space2();
      Print();
    }
   /* if (counter == 3360) {//56 minute
      V2 = (voltage * devider);
      //Serial.println ("************************");
      line();
      Serial.print ("V2 battery read =      ");
      Serial.print (V2);
      Serial.println (" V"); 
      //Serial.println ("************************");
      line();
    }*/
    ////////////////////////////////////////////////////////////////////
    if (counter == 3600) {
      Serial.print ("60 Minute =");
      space2();
      Print();
    }
    if (counter == 3601) {
      //Serial.println ("Completed 60 minute discharge");
      Serial.println ("enter OK turn on rectifiers you will have ten seconds.");
      waitForOk();
      while (!(tic == 0)){ 
    Serial.print (tic); 
    Serial.print (" ");
    tic = tic-1;
    delay (1000);
    }
    Serial.println ();
    }
    if (counter == 3615) {
      //Serial.println ("************************");
      line();
      Serial.print ("15 Second's =");
      space3();
      Print();
    }
    if (counter == 3630) {
      Serial.print ("30 Second's =");
      space3();
      Print();
    }
    if (counter == 3660) {
      Serial.print ("1 Minute =");
      space();
      Print();
    }
    if (counter == 3900) {
      Serial.print ("5 Minute =");
      space();
      Print();
    }
    if (counter == 3901) {
      //Serial.println ("************************");
      line();
      Serial.println ("To calculate Battery Run Time...");
      waitForOk();
      if (V1 == 0){
        V1 = plateau;
      }
      if (V2 == 0){
        V2 = V1-.01;
      Serial.println ("NO PLATEAU DROP OCCURED IN 60 MIN DISCHARGE TEST... USING BARE MIN OF LOSS FOR CALCULATION");
      }
      Mdisch = (V1-V2)/10;
     // Mdisch = Mdisch/10;
      Mcut = 2*Mdisch;
      Vd = V2 - Vcut;
      Tr = Vd/Mcut;
      Serial.print ("Battery run time capacity is:");
      Serial.print (Tr);
      Serial.println (" Minutes");
      Serial.print ("Or ");
      Serial.print (Tr/60);
      Serial.println ("Hours");
      Serial.println ("*Coup de Fouet*");
      Serial.print ("Trough = ");
      Serial.print (trough);
      Serial.print (" V @ ");
      Serial.print (troughCount);
      Serial.print (" Seconds/ ");
      Serial.print (troughCount/60);
      Serial.println (" Minutes");
      Serial.print ("Plateau = ");
      Serial.print (plateau);
      Serial.print (" V @ ");
      Serial.print (plateauCount/60);
      Serial.println ("Minutes");   
}
delay(800);
}
void waitForOk() {
  Serial.println(F("Type OK to continue.\n"));
  while (!(Serial.findUntil("OK", "\n")));
  
}
void Print()  {
      Serial.print (voltage * devider);
      Serial.print (" V");
      if (44.99<(voltage * devider) && (voltage * devider) < 54){
      graphV = map ((voltage * devider), 45,54,0,9);
      //Serial.print ("graphV (should be 1-10)= "); Serial.println (graphV);
      graphM = map (graphV,0,9,45,54);
      //Serial.print ("bring graph back from 1-10 to voltage 45-52.  = "); Serial.println (graphM);
      graphM = ((voltage * devider)-graphM)*10;
      //Serial.print (" this should strip away voltage leaving a decimal value * 10...= "); Serial.println (graphM);
      graphM = map (graphM,1,9,1,9);
      //Serial.print ("graphM = "); Serial.println (graphM);
      int i;
      for(i=0; i<(graphV * 6); i++) {
        //for(i=0; i<(graphV *  10 ); i++) {
        Serial.print (":");
      }
      int j;
      for (j=0; j<(graphM); j++) {
        Serial.print (":");
      }
        Serial.println();
      }
else {
  Serial.println ("Voltage not within range!");
}
}
void space(){
  int i;
  for(i=0; i<5; i++){//13
    Serial.print (" ");      
  }
}
void space2(){
  int i;
  for(i=0; i<4; i++){//12
    Serial.print (" ");      
  }
}
void space3(){
  int i;
  for(i=0; i<2; i++){//10
    Serial.print (" ");      
  }
}
void space4(){
  int i;
  for(i=0; i<1; i++){//9
    Serial.print (" ");      
  }
}
void CDFtrough()  {
  if (counter < 150){
    New_trough = (voltage * devider);
    if (New_trough < trough)
    {
      trough = New_trough;
      troughCount = counter;
    }
    }
  }
  void CDFplateau(){
    if(counter > 150) {
    New_plateau = (voltage * devider);
    if (New_plateau > plateau)
    {
      if (New_plateau - plateau > .09){
      plateau = New_plateau;
      plateauCount = counter;
      }
    }
    else if (New_plateau < plateau && V1 == 0){
      if (plateau - New_plateau > .09){
        V1 = (voltage * devider);
      //Serial.println ("************************");
      line();
      Serial.print ("V1 battery read =      ");
      Serial.print (V1);
      Serial.println (" V");  
      V1readStarttime = counter;
      //Serial.println ("************************");
      line();
    }
    }
    }
  }
  void V1readTimer(){
    if (!V1readStarttime == 0){
    if (counter == V1readStarttime + 600)
    V2 = (voltage * devider);
    //Serial.println ("************************");
      line();
      Serial.print ("V2 battery read =      ");
      Serial.print (V2);
      Serial.println (" V");  
      //Serial.println ("************************");
      line();
  }}
  void line(){
    int i;
    for (i=0; i<scrollLine; i++){
      Serial.print ("*");
    }
    Serial.println();
  }
  void center(){
    int i;
    for (i=0; i<((scrollLine/2)-7); i++){
      Serial.print (" ");
    }
  }
float waitForFloat() {
  //while (!(Serial.available() > 0));
  while (!(Serial.available() == 5));
  return Serial.parseFloat();
}

void printFloatVoltageMessage() {
  Serial.println(F("Type the present Voltage."));  
  Serial.println ("Example = 52.46");
  Serial.println ("YOU WILL NOT BE ABLE TO SEE WHAT YOU ENTER! PAY ATTENTION");
  Serial.print ("If Voltage shown is not correct restart and try again.\n");
}



