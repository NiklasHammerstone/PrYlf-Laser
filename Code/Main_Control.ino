#include <INA219_WE.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>

//~~~~~~~~~ Settings ~~~~~~~~~~~

#define temp_crystal A0
#define temp_crystal_TEC A1
#define temp_diode A3
#define temp_diode_TEC A2
#define laser_pot A6
#define I2C_ADDRESS_CURRENT 0x40
#define TEC_crystal 5
#define TEC_diode 4
#define TTL_laser 9
#define enable_cooling 3 

#define max_current 1500 //mA
#define ideal_crystal_temp 25.0 //°C 25
#define ideal_diode_temp 20.0 //°C 20
#define laser_mode 200  //200   //100: With PWM 50Hz, 250: CW, Higher thermal load

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LiquidCrystal_I2C lcd(0x27, 20, 4);
INA219_WE ina219 = INA219_WE(I2C_ADDRESS_CURRENT);

const int slaveSelectPin = 10;
int current_diode = 0;   
boolean overcurrent = false;

void setup() {

  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(temp_crystal, INPUT);
  pinMode(temp_crystal_TEC, INPUT);
  pinMode(temp_diode, INPUT);
  pinMode(temp_diode_TEC, INPUT);
  pinMode(laser_pot, INPUT);
  pinMode(TEC_crystal, OUTPUT);
  pinMode(TEC_diode, OUTPUT);
  pinMode(TTL_laser, OUTPUT);
  pinMode(slaveSelectPin, OUTPUT);
  Serial.begin(9600);
    Serial.println("test");
  
  Wire.begin();
  ina219.init();
  Serial.println("wire");
  if(!ina219.init()){
    Serial.println("INA219 not connected!");
  }
  Serial.println("loop");
  ina219.setPGain(PG_160);
  Serial.println("gain");
  ina219.setBusRange(BRNG_16);
  Serial.println("bus");
  SPI.begin();
  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Pump: "); 
  lcd.setCursor(10,0);
  lcd.print("mA, ");
  lcd.setCursor(18,0);
  lcd.print("mW");
  lcd.setCursor(0,1);
  lcd.print("Diode temp: ");
  lcd.setCursor(19,1);
  lcd.print("C");
  lcd.setCursor(0,2);
  lcd.print("Cryst temp: ");
  lcd.setCursor(19,2);
  lcd.print("C");
  lcd.setCursor(0,3);
  lcd.print("Status: Ready");
  
  analogWrite(TTL_laser, laser_mode); //Laser is only switched on here, nowhere else.. 
  
}

void loop() {
  current_diode = 0;
  current_diode = (int) ina219.getCurrent_mA() - 2; //Zero Offset = 2mA
  
  updateDisplay();
  
if (current_diode > max_current){
    //STATUS: ALARM
    analogWrite(TTL_laser, 0);  //deactivate pump laser. Only way the reactivate is to restart the system.
    lcd.setCursor(0, 3);
    lcd.print("Status: OVERCURRENT");
    overcurrent = true;
  }
  if (digitalRead(enable_cooling)==HIGH){
    //Temperaturregelung 
    if (temp(temp_crystal)>ideal_crystal_temp){
      digitalWrite(TEC_crystal, HIGH);
    }else{
      digitalWrite(TEC_crystal, LOW);
    }

    if (temp(temp_diode)>ideal_diode_temp){
      digitalWrite(TEC_diode, HIGH);
    }else{
      digitalWrite(TEC_diode, LOW);
    }
  }

}

double temp(uint8_t pin){
  //Reads the temperature from the analog pin (assuming LM35 sensor)
  double voltage = 5.0*(analogRead(pin)/1023.0);
  return voltage / 0.01;
}

void updateDisplay(){

  lcd.setCursor(6, 0);
  lcd.print("    ");
  lcd.setCursor(6, 0);
  lcd.print(current_diode);
  lcd.setCursor(14, 0);
  lcd.print("    ");
  lcd.setCursor(14, 0);
  lcd.print(getApproxWattage(current_diode));
  
  lcd.setCursor(12, 1);  
  int _ = analogRead(temp_diode);  //I have no idea why this is necessary, but without this, reading the temperator of the diode does not work.
  lcd.print(String(temp(temp_diode), 2));
  lcd.setCursor(12, 2);
  lcd.print(String(temp(temp_crystal), 2));

  if(current_diode > 5){     //Above 5mA (noise) the diode definitely is on!
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    lcd.setCursor(0, 3);
    lcd.print("Status: PUMP ACTIVE");
  }else if(!overcurrent){
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    lcd.setCursor(0, 3);
    lcd.print("Status: Ready");
  }
}

int getApproxWattage(double mA){
  //Approximates the Wattage of the diode from the current
  double mW = 1.4894*mA-155.49;
  if (mW < 0){
    return 0;
  }else{
  return (int) mW;}
  }
