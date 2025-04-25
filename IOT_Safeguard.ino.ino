
#include<iotgecko.h>
#include<LiquidCrystal.h>
#define esp_baudrate 115200
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define light A2
#define lm35 A4
#define flame A5
#define gas A3
#define buzzer 4
#define led 5
#define lamp230 6
#define fan 7
long previous_millis = 0;

float temp = 0.0;
int light_value = 0;
int gas_value = 0;
int flame_value = 1;
bool notConected = true;
bool login = false;
int iot_status;

String id = "upadhyerajanikant@gmail.com";
String pass = "2769";
String ssid = "iot";
String pass_key = "project1234";

const int no_of_data = 4;
String main_data[10];
int last_H;
int last_R;
int last_T;
int a, b, c;
int sensorValue = 0;
float rain = 0;
int sys_l = 50, sys_u = 90, dia_l = 40, dia_u = 80, temp_l = 25, temp_u = 33, pulse_l = 40, pulse_u = 50;
int sys, dia, p,  stay = 1;
iotgecko gecko = iotgecko(esp_baudrate);

void setup() {
  pinMode (light, INPUT);
  pinMode (gas, INPUT_PULLUP);
  pinMode (buzzer, OUTPUT);
  pinMode (led, OUTPUT);
  pinMode (fan, OUTPUT);
  pinMode (lamp230, OUTPUT);

  digitalWrite(lamp230, LOW);
  digitalWrite(fan, LOW);
  digitalWrite(buzzer, LOW);
  digitalWrite(led, HIGH);
  //Serial.begin(9600);

  lcd.begin(20, 4);
  lcd.print("Industry Protection");
  lcd.setCursor(4, 1);
  lcd.print("Using Cloud");
  delay(2000);
  wifi_iot_init();

}
void loop() {
  // put your main code here, to run repeatedly:


  lcd.clear();
  temperature();
  ldr();
  mq2();
  flame_sensor();
  process();
  if (millis() - previous_millis > 5000)
  {
    send_data();
    ckeck_wifi_connection();

  }
  delay(1000);
}




void temperature()
{
  for ( int i = 0; i < 100; i++ )
  {
    temp = temp + (5.0 * analogRead(lm35) * 100.0) / 1023;
  }
  temp = temp / 100.0;
  //Serial.println(temp);
  lcd.setCursor(0, 0);
  lcd.print("Temperature =");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
}

void ldr()
{
  light_value = ((analogRead(light) * 100.0) / 1023 + 0.9);
  Serial.println(light_value);
  lcd.setCursor(0, 1);
  lcd.print("Light       =");
  lcd.setCursor(13, 1);
  lcd.print(light_value);
  lcd.print("%");
}
void mq2()
{
  gas_value = ((analogRead(gas) * 100.0) / 1023);
  gas_value = map(gas_value, 0, 75, 0, 100);
  
  if (gas_value > 100)
  {
    gas_value = 100;
  }
  lcd.setCursor(0, 2);
  lcd.print("Gas Amount  =");
  lcd.setCursor(13, 2);
  lcd.print(gas_value);
  lcd.print("%");
}



void flame_sensor()
{
  flame_value = digitalRead(flame);
  if (flame_value == 0)
  {
    lcd.setCursor(0, 3);
    lcd.print("Flame detected");
  }
  else
  { lcd.setCursor(0, 3);
    lcd.print("Flame not detected");
  }
}

void start_blinking_led()
{ for (int i = 0; i <= 10; i++)
  {
    digitalWrite(led, LOW);
    delay(30);
    digitalWrite(led, HIGH);
    delay(30);
  }
}

void stop_blinking_led()
{
  digitalWrite(led, HIGH);
}

void process()
{
  if (temp > 40 || gas_value > 20 || flame_value == 0)
    digitalWrite(buzzer, HIGH);
  else
    digitalWrite(buzzer, LOW);

  if (temp > 40 || gas_value > 20 || light_value < 40 || flame_value == 0)
    start_blinking_led();
  else
    stop_blinking_led();

  if (temp > 40 || gas_value > 20)
    digitalWrite(fan, HIGH);
  else
    digitalWrite(fan, LOW);
  if (light_value < 40)
    digitalWrite(lamp230, HIGH);
  else
    digitalWrite(lamp230, LOW);
}

void send_data()
{
  main_data[0] = String(temp);
  main_data[1] = String(!(flame_value));
  main_data[2] = String(gas_value);
  main_data[3] = String(light_value);
  iot_status = gecko.SendGParams(main_data, no_of_data); //send data to iotgecko.com
}

void ckeck_wifi_connection()
{
  if (iot_status == VALID)
  { lcd.clear();
    //      last_H = a;
    //      last_R = b;
    //      last_T = c;
    lcd.print("data sent");
    lcd.setCursor(0, 1);
    lcd.print("succesfully....");
    //Serial.println(F("data send succesfully..:)"));
  }
  else
  {
    lcd.clear();
    lcd.print(F("connection lost"));
    delay(100);
    lcd.setCursor(0, 1);
    lcd.print(F("reconnecting..."));
    //Serial.println(F("connection lost"));
    while (!gecko.GeckoReconnect()) //reconnect to the iotgecko.com
    {
      //Serial.println(F("conncetion failed.....reconnecting"));
      delay(2000);
    }
    lcd.clear();
    lcd.print(F("Connected"));
    //Serial.println(F("connected succesfully"));
  }
  previous_millis = millis();

}

void wifi_iot_init()
{
  lcd.clear();
  lcd.print(F("Connecting Wifi"));
  //Serial.println(F("conncecting to wifi"));
  //Serial.print(ssid);
  //Serial.print("\t");
  ///Serial.println(pass_key);
  while (notConected)
  {
    if (gecko.GeckoConnect(ssid, pass_key))
    {
      lcd.clear();
      lcd.print(F("Connected"));
      //Serial.println(F("connceted to wifi..."));
      notConected = false;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print(F("connecting to"));
  lcd.setCursor(0, 1);
  lcd.print(F("iotgecko.com"));
  //Serial.println(F("connecting to iotgecko.com"));
  delay(2000);
  while (!login)
  {
    if (gecko.GeckoVerify(id, pass))
    {
      lcd.clear();
      lcd.print(F("Connected"));
      //Serial.println(F("connected succesfully"));
      login = true;
    }
  }
  delay(1500);
}
