#define relay 3      //реле подключается к выводу D3
#define soil A0      //сигнальный контакт с датчика подключается к A0
unsigned long timingSoil = 0;   //переменная таймера
unsigned long timeMeasure = 300000; //период включения датчика в мс
int soilPorog = 512; //пороговое значение влажности почвы (от 0 до 1023)
unsigned long timeWater = 5000; //время работы насоса в мс

void setup()
{
  pinMode (relay, OUTPUT); //реле в режиме работы "выход"
  pinMode (2, OUTPUT); //контакт для подачи питания на датчик в режиме "выход"
  digitalWrite(relay, HIGH);//размыкаем контакты реле
}

void loop()
{
  if ((millis() - timingSoil) >= timeMeasure)   //каждые минут
  {
    timingSoil = millis();
    digitalWrite (2, HIGH); //включаем питание на датчик
    delay (2000); //ждем 2 секунды

    if (analogRead(soil) >= soilPorog) //если значение влажности больше порога
    {
      digitalWrite(relay, LOW); //реле замыкает контакты, включается насос
      delay (timeWater); //проходит 5 секунд работы насоса
      return; //выход из условия после первой итерации
    }

    digitalWrite (2, LOW); //выключаем датчик
    digitalWrite(relay, HIGH); //реле размыкает контакты, выключен насос
  }
}
