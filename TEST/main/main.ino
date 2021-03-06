
#include <dht11pro.h>

/*
  农田小气候分析仪程序入口
*/
dht11pro DHT11; //空气温湿度检测模块

const int DHT11PIN = 22; //DHT11的数据读取引脚为22
const int RM5 = 32;

/*
读取土壤湿度，读取值范围为234（水中）—1023（空气中）
*/
void getGroudHum() {
  //Serial.println("土壤湿度传感器模块：");
  float data = analogRead(0);
  int key_status = digitalRead(RM5); //1：未达到湿度要求
  if (key_status == 1)
  {
    float humidty = (1 - (data - 223)/800)*100;
    Serial.println("土壤过于干燥");
    Serial.print("湿度： ");
    Serial.print(humidty);
    Serial.println("%");
  }
  else
  {
    Serial.println("土壤湿度达标");
    float humidty = (1 - (data - 223)/800)*100;
    Serial.print("湿度： ");
    Serial.print(humidty);
    Serial.println("%");
  }
}
//摄氏温度度转化为华氏温度
double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

//摄氏温度转化为开氏温度
double Kelvin(double celsius)
{
  return celsius + 273.15;
}

// 露点（点在此温度时，空气饱和并产生露珠）
// 参考: http://wahiduddin.net/calc/density_algorithms.htm
double dewPoint(double celsius, double humidity)
{
  double A0 = 373.15 / (273.15 + celsius);
  double SUM = -7.90298 * (A0 - 1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / A0))) - 1) ;
  SUM += 8.1328e-3 * (pow(10, (-3.49149 * (A0 - 1))) - 1) ;
  SUM += log10(1013.246);
  double VP = pow(10, SUM - 3) * humidity;
  double T = log(VP / 0.61078); // temp var
  return (241.88 * T) / (17.558 - T);
}

// 快速计算露点，速度是5倍dewPoint()
// 参考: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity / 100);
  double Td = (b * temp) / (a - temp);
  return Td;
}


/*获取温湿度数据
  type值默认1，输出温湿度；
  type=0，输出额外内容
*/
void getHumAndTemp(int type = 1)
{
  Serial.println("\n");
  int chk = DHT11.read(DHT11PIN); //获取当前的温湿度传感器状态
  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK:
      Serial.println("OK");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.println("OK");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.println("Time out error");
      break;
    default:
      Serial.println("Unknown error");
      break;
  }
  Serial.print("湿度 (%): ");
  Serial.println((float)(DHT11.humidity + DHT11.hum_low * 0.01), 2);

  Serial.print("温度 (摄氏度): ");
  Serial.println((float)(DHT11.temperature + DHT11.temp_low * 0.01), 2);
  if (type == 0)
  {
    Serial.print("华氏温度(oF): ");
    Serial.println(Fahrenheit(DHT11.temperature), 2);

    Serial.print("开氏温度 (K): ");
    Serial.println(Kelvin(DHT11.temperature), 2);

    Serial.print("露点 (oC): ");
    Serial.println(dewPoint(DHT11.temperature, DHT11.humidity));

    Serial.print("快速计算露点 (oC): ");
    Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));
  }
}

void setup()
{
  pinMode(RM5, INPUT);
  Serial.begin(9600);
  Serial.println("DHT11 TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
}

void loop()
{
  getGroudHum();
  getHumAndTemp();
  delay(3000);
}


