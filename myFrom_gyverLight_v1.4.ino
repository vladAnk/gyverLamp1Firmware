/*
  Скетч к проекту "Эффектный светильник"
  Страница проекта (схемы, описания): https://alexgyver.ru/gyverlight/
  Исходники на GitHub: https://github.com/AlexGyver/gyverlight/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  https://AlexGyver.ru/
*/

/*
   Управление кнопкой/сенсором
  - Удержание - яркость
  - 1х тап - переключ режима
  - 2х тап - вкл/выкл
  - 3х тап - старт/стоп авто смены режимов
  - 4х тап - вкл/выкл белый свет
*/

/*
   Версия 1.3 - пофикшен баг с fillAll
*/
/*
  AVE - правки под себя:
	выброшен белый режим
*/

// ************************** НАСТРОЙКИ ***********************
#define CURRENT_LIMIT 2000  // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define AUTOPLAY_TIME 60    // время между сменой режимов в секундах

#define NUM_LEDS 13         // количсетво светодиодов в одном отрезке ленты
#define NUM_STRIPS 4        // количество отрезков ленты (в параллели)
#define LED_PIN 6           // пин ленты
#define BTN_PIN 2           // пин кнопки/сенсора
#define MIN_BRIGHTNESS 5  // минимальная яркость при ручной настройке
#define BRIGHTNESS 250      // начальная яркость

// ************************** ДЛЯ РАЗРАБОТЧИКОВ ***********************
#define MODES_AMOUNT 14

#include "GyverButton.h"
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

#include <FastLED.h>
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;

#include "GyverTimer.h"
GTimer_ms effectTimer(60);
GTimer_ms autoplayTimer((long)AUTOPLAY_TIME * 1000);
GTimer_ms brightTimer(20);

int brightness = BRIGHTNESS;
int tempBrightness;
byte thisMode;

bool gReverseDirection = false;
boolean loadingFlag = true; // этот флаг означает что режим изменился и нужно инициализировать жуков в lightBugs()
boolean autoplay = true; // автоматическая смена режимов по таймеру
boolean powerDirection = true; // направление изменения яркости (при зажатой кнопке)
boolean powerActive = false;  //  изменение яркости в процессе (зажата кнопка и яркость еще не достигла макс/мин значения; либо лампа включена/выключена кнопкой, и при этом яркость еще не достигла целевого значения brightness)
boolean powerState = true;  // состояние питания (true - включено, false - отключено (управляется кнопкой))
boolean whiteMode = false;  // включен режим ровного белого света
boolean brightDirection = true;
boolean wasStep = false;

byte my_baseColor = 0;

byte randomByte = random(0, 255);
byte calculateContrastHue(byte hue){
  return (255 / 2) - randomByte;
}
byte oppositeRandomByte = calculateContrastHue(randomByte);
byte staticColorBrightness = 255;
byte deltaColor = 20;

// залить все
void fillAll(CRGB newcolor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = newcolor;
  }
}



// функция получения цвета пикселя по его номеру
uint32_t getPixColor(int thisPixel) {
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}

byte maxValue = 0;
byte rPix;
byte gPix;
byte bPix;
byte getPixBrightness(int thisPixel) { // "Value" in HSV model (hue-saturatuion-value)
  maxValue = 0;
  rPix = leds[thisPixel].r;
  gPix = leds[thisPixel].g;
  bPix = leds[thisPixel].b;
  maxValue = max(rPix, gPix);
  maxValue = max(maxValue, bPix);
  return maxValue;
}

byte staticPaletteColors[NUM_LEDS];
void setStaticPaletteColors(byte startColor, byte deltaColor){
	for (int i = 0; i < NUM_LEDS; i++){
		staticPaletteColors[i]=startColor+i*deltaColor;
	}
}

//Этот метод запускается ардуинкой 1 раз при подаче питания на нее 
void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT / NUM_STRIPS);
  FastLED.setBrightness(brightness);
  FastLED.show();

  randomSeed(analogRead(0));
  touch.setTimeout(300);
  touch.setStepTimeout(50);

  gPal = HeatColors_p;
}

//////////////////////////
//     VVV LOOP VVV     //
//////////////////////////
//Этот метод крутится ардуинкой по кругу
void loop() {

  //КНОПКА - СЛУШАЕМ ЕЕ
  touch.tick();

  //КНОПКА - ПО ЧИСЛУ ЕЕ НАЖАТИЙ ВЫБИРАЕМ ДЕЙСТВИЕ
  if (touch.hasClicks()) {
    byte clicks = touch.getClicks();
    switch (clicks) {
      case 1: //СЛЕДУЮЩИЙ РЕЖИМ
        if (!whiteMode && !powerActive) { //смена режимов отключена если находимся в режиме белого света либо если находимся в процессе выключения питания.
          //видимо это нужно чтобы во время регулирования яркости с помощью кнопки не переключались режимы.
          nextMode();
        }
        break;
      case 2: //ВКЛ/ВЫКЛ ЛАМПУ
        powerDirection = !powerDirection;
        powerActive = true;
        tempBrightness = brightness * !powerDirection;
        break;
      case 3: //ВКЛ/ВЫКЛ АВТОСМЕНУ РЕЖИМОВ
         if (!whiteMode && !powerActive) autoplay = !autoplay; //автосмена режимов отключена если находимся в режиме белого света либо если находимся в процессе выключения питания.
		 break;
	  case 4: //ВКЛ/ВЫКЛ РЕЖИМ БЕЛОГО СВЕТА
	     if (!powerActive) { //вход/выход в режим белого света игнорируется если находимся в процессе изменения яркости.
              whiteMode = !whiteMode;
              if (whiteMode) {
                effectTimer.stop(); // перестаем общаться с лентой и перебирать режимы
                fillAll(CRGB::White);
                FastLED.show();
              } else {
                effectTimer.start(); // возобновляем общение с лентой
              }
        }
        break;
      default:
        break;
    }
  }

  ////КНОПКА - НАСТРОЙКА ЯРКОСТИ ПО ДАННЫМ С КНОПКИ
  if (touch.isStep()) {
    if (!powerActive) {
      wasStep = true;
      if (brightDirection) {
        brightness += 3;
      } else {
        brightness -= 3;
      }
      brightness = constrain(brightness, MIN_BRIGHTNESS, 255);
      FastLED.setBrightness(brightness);
      FastLED.show();
    }
  }

  //КНОПКА - ОБРАБАТЫВАЕМ СОБЫТИЕ ОТЖАТИЯ КНОПКИ
  if (touch.isRelease()) {
    if (wasStep) {
      wasStep = false;
      brightDirection = !brightDirection;
    }
  }

  //СВЕТОДИОДНАЯ ЛЕНТА - ОБЩАЕМСЯ С НЕЙ
  if (effectTimer.isReady() && powerState) {
    switch (thisMode) {
      case 0: lighter(); 		break; //начальный режим с малым током (1 активный диод)
	  case 1: if(my_baseColor==0){initRaindrops();};  raindrops2(); break; //красиво
      case 2: lighter2();       break;
      case 3: if(my_baseColor==0){initRaindrops();}; raindrops();   break; //офигенный - ламповый огонек
      case 4: rainbowLong(); 	break; //офигенный
      case 5: rainbow(); 		break; //офигенный
	  case 6: sparkles6();  	break; //норм
	  case 7: sparkles7();  	break; //норм
	  case 8: sparkles8();  	break; //норм, ЗАМЕТНО мерцание
	  case 9: sparkles3();  	break; // норм
	  case 10: sparkles5();  	break; // норм
      case 11: lightBugs(); 	break; //офигенный
      case 12: lightBugs3(); 	break; //норм,но мигает
      case 13: colors(); 		break;
    }
    FastLED.show();
  }
  
  // таймер смены режима
  if (autoplayTimer.isReady() && autoplay) {    
    nextMode();
  }

  //плавное включение/выключение лампы за счет управления яркостью
  brightnessTick();
}
//////////////////////////
//     ^^^ LOOP ^^^     //
//////////////////////////

void nextMode() {
  thisMode++;
  if (thisMode >= MODES_AMOUNT) thisMode = 0;
  loadingFlag = true;
  resetSaturationArray();
  randomByte = random(0, 255);
  oppositeRandomByte = calculateContrastHue(randomByte);
  FastLED.clear();
}

void brightnessTick() {
  if (powerActive) {
    if (brightTimer.isReady()) {
      if (powerDirection) {
        powerState = true;
        tempBrightness += 5;
        if (tempBrightness > brightness) {
          tempBrightness = brightness;
          powerActive = false;
        }
        FastLED.setBrightness(tempBrightness);
        FastLED.show();
      } else {
        tempBrightness -= 5;
        if (tempBrightness < 0) {
          tempBrightness = 0;
          powerActive = false;
          powerState = false;
        }
        FastLED.setBrightness(tempBrightness);
        FastLED.show();
      }
    }
  }
}
