#include <AntaresESP32HTTP.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//Variabel PIN Sensor
  #define turSens 32
  #define phSens 33
  #define tempSens 23

//Variabel Kalibrasi Suhu
  OneWire oneWire(tempSens);
  DallasTemperature suhuSensor(&oneWire);
  float suhuOut;

//Variabel Kalibrasi PH
  float ph4 = 3.22;
  float ph7 = 2.58;
  float volt, phOut;

//Variabel Turbidty
  float voltTur,turOut;

//Variabel NTP
  const long utcOffSetInSeconds = 0;
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffSetInSeconds);
  int currTime;

//Properties Antares
  #define ACCESSKEY "aece2c4c6aae46bc:7fb2dfbb2027c712" //Access key akun Antares anda
  #define WIFISSID "Fathur"                      //SSID WiFi
  #define PASSWORD "fathur123"                        //Password WiFi
  #define applicationName "KualitasAir"                 //Application name Antares
  #define deviceName "TaFathur1"                        //Device Antares

AntaresESP32HTTP antares(ACCESSKEY); 

void getTime(){
  timeClient.update();
  currTime = timeClient.getEpochTime();
  Serial.println(currTime);
}

void setup() {
  Serial.begin(115200);
  suhuSensor.begin();
  
  antares.setDebug(true);
  antares.wifiConnection(WIFISSID,PASSWORD);
  
  timeClient.begin();
  Serial.println("RANCANG BANGUN ALAT KLASIFIKASI KUALITAS AIR BERBASIS IoT");
  delay(2000);
}

void loop() {
  getTime();
  getPH();
  getTurbidity();
  getTemper();
  fuzzyLogic();
//  kirimData();
  Serial.println("==============================\n");
  delay(5000);
}

void getPH(){
  volt = analogRead(phSens)*3.3/4095.0;
  phOut = 7.00 + (ph7 - volt)/((ph4 - ph7)/3);
  Serial.print("PH        : "); Serial.println(phOut);
}

void getTurbidity(){
  voltTur = analogRead(turSens)*3.3/4095.0;
//  turOut = -2572.2*pow(voltTur,2)+8700.5*voltTur-4352.9;
  Serial.print(analogRead(turSens));
  Serial.print(" Turbidity : ");Serial.println(voltTur);
}

void getTemper(){
  suhuSensor.requestTemperatures();
  suhuOut = suhuSensor.getTempCByIndex(0);
  Serial.print("Suhu    : ");Serial.println(suhuOut);
}

//Variabel Himpunan Fuzzy
  float phAsam, phNetral, phBasa;
  float turJernih, turKeruh, turSKeruh;
  float perB, perTB;
  
//Variabel Rule
  float ap1,ap2,ap3,ap4,ap5,ap6,ap7,ap8,ap9;
  float z1,z2,z3,z4,z5,z6,z7,z8,z9;
  float in1 = 60, in2=80;
  float zDef;

void fuzzyLogic(){
  fuzzyfikasi();
  rule();
  defuzzyfikasi();
}

void fuzzyfikasi(){
  phAsam = bahuKiri(6,7,phOut);
  phNetral = trapesium(6,7,8,9,phOut);
  phBasa = bahuKanan(8,9,phOut);
  turJernih = bahuKiri(4,5,turOut);
  turKeruh = trapesium(4,5,25,26,turOut);
  turSKeruh = bahuKanan(25,26,turOut);
}

void rule(){
  ap1 = min(phAsam,turJernih);
  z1  = 80-(20*ap1);
  ap2 = min(phAsam,turKeruh);
  z2  = 80-(20*ap2);
  ap3 = min(phAsam,turSKeruh);
  z3  = 80-(20*ap3);
  ap4 = min(phNetral,turJernih);
  z4  = (ap4*20)+60;
  ap5 = min(phNetral,turKeruh);
  z5  = (ap5*20)+60;
  ap6 = min(phNetral,turSKeruh);
  z6  = 80-(20*ap6);
  ap7 = min(phBasa,turJernih);
  z7  = 80-(20*ap7);
  ap8 = min(phBasa,turKeruh);
  z8  = 80-(20*ap8);
  ap9 = min(phBasa,turSKeruh);
  z9  = 80-(20*ap9);
}

void defuzzyfikasi(){
  float pem = (ap1*z1)+(ap2*z2)+(ap3*z3)+(ap4*z4)+(ap5*z5)+(ap6*z6)+(ap7*z7)+(ap8*z8)+(ap9*z9);
  float pen = ap1+ap2+ap3+ap4+ap5+ap6+ap7+ap8+ap9;
  zDef = pem/pen;
  perB = ((zDef-60)/(80-60))*100;
  perTB = ((80-zDef)/(80-60))*100;
  Serial.print("z Deffuzifikasi: ");Serial.println(zDef);
  Serial.print(perB);Serial.print("% Bersih, ");Serial.print(perTB);Serial.println("% Tidak Bersih");
}

void kirimData(){
  antares.add("ph", phOut);
  antares.add("turbidity", turOut);
  antares.add("suhu", suhuOut);
  antares.add("fuzzy", zDef);
  antares.add("bersih", perB);
  antares.add("tbersih", perTB);
  antares.add("waktu",currTime);

  antares.send(applicationName, deviceName);
  delay(10000);
}

float bahuKiri(float a, float b, float x){
  if(x <= a){
    return 1;
  }
  else if(x >= a && x <= b){
    return (b-x)/(b-a);
  }
  else if(x >= b){
    return 0;
  }
}

float bahuKanan(float a, float b, float x){
  if (x <= a){
    return 0;
  }
  else if(x >= a && x <= b){
    return (x-a)/(b-a);
  }
  else if(x >= b){
    return 1;
  }
}

float trapesium(float mina, float a, float b, float maxb, float x){
  if (x <= mina){
    return 0;
  }
  else if(x >= mina && x <= a){
    return (x-mina)/(a-mina);
  }
  else if(x >= a && x <= b){
    return 1;
  }
  else if(x >= b && x <= maxb){
    return (maxb-x)/(maxb-b);
  }
  else if(x >= maxb){
    return 0;
  }
}
