#include <AntaresESP32HTTP.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Properties Antares
  #define ACCESSKEY "aece2c4c6aae46bc:7fb2dfbb2027c712" //Access key akun Antares anda
  #define WIFISSID "Playstation 5"                      //SSID WiFi
  #define PASSWORD "horizonzero"                        //Password WiFi
  #define applicationName "KualitasAir"                 //Application name Antares
  #define deviceName "TaFathur1"                      //Device Antares

AntaresESP32HTTP antares(ACCESSKEY); 

//Pin Sensor
  #define senSuhu 21
  #define senTur 35
  #define senPh 34

//Conf Suhu
  OneWire oneWire(senSuhu);
  DallasTemperature sensor(&oneWire);

float suhu;
int anTur;
int anPh;
float valTur,ntu;
float valPh;

//Variabel Fuzzy
  float phAsam,phNetral,phBasa;
  float turJernih,turKeruh,turSKeruh;
  float perB,perTB;

void setup() {
  pinMode(senTur,INPUT);
  pinMode(senPh,INPUT);
  Serial.begin(115200);
//  antares.setDebug(true);
//  antares.wifiConnection(WIFISSID,PASSWORD);
  sensor.begin();
}

void loop() {
  bacaSuhu();
  bacaTurbidity();
  bacaPh();
  fuzzy();
  kirimData();
  Serial.println("===================");
  delay(1000);
}

void bacaSuhu(){
  sensor.requestTemperatures();
  suhu = sensor.getTempCByIndex(0);
  Serial.print("Suhu      : ");
  Serial.println(suhu);
}

void bacaTurbidity(){
  anTur = analogRead(senTur);
  ntu = map(anTur, 0, 4095, 30, 0);
  Serial.print("Turbidity : ");
  Serial.println(anTur);
}

void bacaPh(){
  anPh = analogRead(senPh);
  Serial.print("PH        : ");
  Serial.println(valPh);
}

//Variabel Rule
  float ap1,ap2,ap3,ap4,ap5,ap6,ap7,ap8,ap9;
  float in1=60,in2=80;
  float ce;

void fuzzy(){
  fuzzyfikasi();
  rule();
  def();
  perB = ((ce-60)/(80-60))*100;
  perTB = ((80-ce)/(80-60))*100;
  Serial.print(perB);Serial.print("% Bersih, ");
  Serial.print(perTB);Serial.println("% Tidak Bersih");
}

void kirimData(){
  antares.add("ph",valPh);
  antares.add("turbidity",anTur);
  antares.add("suhu",suhu);
  antares.add("fuzzy",ce);
  antares.add("bersih",perB);
  antares.add("tbersih",perTB);

  antares.send(applicationName, deviceName);
  delay(10000);
}

void fuzzyfikasi(){
  phAsam = bahuKiri(6,7,valPh);
  phNetral = trapesium(6,7,8,9,valPh);
  phBasa = bahuKanan(8,9,valPh);
  turJernih = bahuKiri(4,5,anTur);
  turKeruh = trapesium(4,5,25,26,anTur);
  turSKeruh = bahuKanan(25,26,anTur);
}

void rule(){
  ap1 = min(phAsam,turJernih);
  ap2 = min(phAsam,turKeruh);
  ap3 = min(phAsam,turSKeruh);
  ap4 = min(phNetral,turJernih);
  ap5 = min(phNetral,turKeruh);
  ap6 = min(phNetral,turSKeruh);
  ap7 = min(phBasa,turJernih);
  ap8 = min(phBasa,turKeruh);
  ap9 = min(phBasa,turSKeruh);
}

void def(){
  float pem = (ap1*in1)+(ap2*in1)+(ap3*in1)+(ap4*in2)+(ap5*in2)+(ap6*in1)+(ap7*in1)+(ap8*in1)+(ap9*in1);
  float pen = ap1+ap2+ap3+ap4+ap5+ap6+ap7+ap8+ap9;
  ce = pem/pen;
  Serial.print("Def       : ");
  Serial.println(ce);
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

float trapesium(float mina, float a, float b, float maxb, float x){
  if(x <= mina){
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

float bahuKanan(float a, float b, float x){
  if(x <= a){
    return 0;
  }
  else if(x >= a && x <= b){
    return (x-a)/(b-a);
  }
  else if(x >= b){
    return 1;
  }
}
