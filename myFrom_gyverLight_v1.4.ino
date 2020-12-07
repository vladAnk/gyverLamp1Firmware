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
    сделано плавное зажигание/гашение огней в методе sparkles
    сделаны новые пресеты с учетом этого: sparkles1-sparkles10
    сделан новый режим raindrops: низ лампы горит ровным немного двигающимся огнем
    добавлено отображение срабатывания кнопки - мигает нижний диод столько раз, сколько нажатий сосчитано; так же миганием отмечено срабатывание автосмены режима(см. методы ...blink).
*/

// ************************** НАСТРОЙКИ ***********************
#define CURRENT_LIMIT 2000  // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define AUTOPLAY_TIME 120    // время между сменой режимов в секундах

#define NUM_LEDS 13         // количсетво светодиодов в одном отрезке ленты
#define NUM_STRIPS 4        // количество отрезков ленты (в параллели)
#define LED_PIN 6           // пин ленты
#define BTN_PIN 2           // пин кнопки/сенсора
#define MIN_BRIGHTNESS 5  // минимальная яркость при ручной настройке
#define BRIGHTNESS 250      // начальная яркость

// ************************** ДЛЯ РАЗРАБОТЧИКОВ ***********************
#define MODES_AMOUNT 15

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
byte brightnessTicks = 100; // для ограничения времени пребывания в состоянии powerActive

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
byte getPixBrightness(int thisPixel) {
  maxValue = 0;
  rPix = leds[thisPixel].r;
  gPix = leds[thisPixel].g;
  bPix = leds[thisPixel].b;
  maxValue = max(rPix, gPix);
  maxValue = max(maxValue, bPix);
  return maxValue;	// "Value" in HSV model (hue-saturatuion-value)
}

/*
byte staticPaletteColors[NUM_LEDS];
void setStaticPaletteColors(byte startColor, byte deltaColor){
	for (int i = 0; i < NUM_LEDS; i++){
		staticPaletteColors[i]=startColor+i*deltaColor;
	}
}
*/
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
      /* непонятно почему перестало работать переключение режима по нажатию кнопки , но при закомментированном условии все прохоисдит как надо
        if (!whiteMode && !powerActive) { //смена режимов отключена если находимся в режиме белого света либо если находимся в процессе выключения питания.
          //видимо это нужно чтобы во время регулирования яркости с помощью кнопки не переключались режимы.
          nextMode();
        }
        */
		blink(1);
        nextMode();
        break;
      case 2: //ВКЛ/ВЫКЛ ЛАМПУ
		blink(2);
        if(!powerActive){
           powerDirection = !powerDirection;
           powerActive = true;
           tempBrightness = brightness * !powerDirection;
		   longBlink(1);
        }
        break;
      case 3: //ВКЛ/ВЫКЛ АВТОСМЕНУ РЕЖИМОВ
        //if (!whiteMode && !powerActive) autoplay = !autoplay;
		blink(3);
		if (!whiteMode) {
			autoplay = !autoplay;
			longBlink(1);
		} //автосмена режимов отключена если находимся в режиме белого света либо если находимся в процессе выключения питания.
		break;
	  case 4: //ВКЛ/ВЫКЛ РЕЖИМ БЕЛОГО СВЕТА
       /* непонятно почему перестал работать выход из белого режима(вход-норм), но при закомментированном условии все прохоисдит как надо*/
	     //if (!powerActive) { //вход/выход в режим белого света игнорируется если находимся в процессе изменения яркости.
         blink(4);
		 whiteMode = !whiteMode;
         if (whiteMode) {
            effectTimer.stop(); // перестаем общаться с лентой и перебирать режимы
            fillAll(CRGB::White);
            FastLED.show();
			longBlink(1);
         } else {
            //FastLED.clear();
            //FastLED.show();
            effectTimer.start(); // возобновляем общение с лентой
         }
        //}
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
      case 0: lighter(); 		  break; //начальный режим с малым током (1 активный диод)
	  case 1: if(my_baseColor==0){initRaindrops();}; raindrops();   break; //офигенный - ламповый огонек
	  case 2: if(my_baseColor==0){initRaindrops();};  raindrops2(); break; //красиво
	  case 3: rainbowLong(); 	break; //офигенный
      case 4: sparkles1();  	break; //мягкий, праздничный, довольно быстрый
      case 5: sparkles2();  	break; //умеренной скорости, красивый
      case 6: sparkles4();  	break; //красивый, глубокие цвета
      case 7: sparkles5();  	break; //всполохи, симпатичный, для освещения не годится, праздничный
	  case 8: sparkles7();  	break; //яркий, годится для освещения, что-то не то с логикой, т.к. основной рисунок не меняется (удалить)
	  case 9: sparkles9();  	break; //яркий, годится для освещения, плавная смена цветов
	  case 10: lighter2();      break;
      case 11: lightBugs(); 	  break; //офигенный
      case 12: lightBugs3(); 	break; //норм,но мигает
      case 13: rainbow();     break; //офигенный
	  case 14: colors(); 		  break;
      default: break;
    }
    FastLED.show();
  }
  
  // таймер смены режима
  if (autoplayTimer.isReady() && autoplay) {
    shortBlink(3);
	resetSaturationArray();
    nextMode();
  }

  //плавное включение/выключение лампы за счет управления яркостью
  // похоже что этот метод, а именно powerActive совместно с изменением яркости в методах effects препятствовало смене режимов.
  //brightnessTick();
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
  //FastLED.clear();
}
/*
void brightnessTick() {
  if (powerActive) {
    if (brightTimer.isReady()) {
      if (powerDirection) {
        powerState = true;
        tempBrightness += 10;
        if (tempBrightness > brightness) {
          tempBrightness = brightness;
          powerActive = false;
        }
        brightnessTicks++;
        if(brightnessTicks > 120){
          brightnessTicks = 100;
          powerActive = false;
          tempBrightness = brightness;
        }
        FastLED.setBrightness(tempBrightness);
        FastLED.show();
      } else {
        tempBrightness -= 10;
        if (tempBrightness < 0) {
          tempBrightness = 0;
          powerActive = false;
          powerState = false;
        }
        brightnessTicks--;
        if(brightnessTicks < 80){
         brightnessTicks = 100;
         powerActive = false;
         powerState = false;
         tempBrightness = 0;
        }
        FastLED.setBrightness(tempBrightness);
        FastLED.show();
      }
    }
  }
}
*/
