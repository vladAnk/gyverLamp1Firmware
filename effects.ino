// CHSV - Representation of an HSV pixel (hue, saturation, value (aka brightness)).

#define TRACK_STEP 5

byte isToSaturateLed[NUM_LEDS]; //0-ничего не делать; 1-ярче;  2-темнее.

#define SA_DO_NOTHING_VALUE 0
#define SA_SATURATE_VALUE 1
#define SA_DESATURATE_VALUE 2
//#define SATURATION_MULTIPLIER_1_2 1.2 // с дробными значениями что-то не то. такое ощущение что округляются
//#define SATURATION_MULTIPLIER_1_5 1.5
#define SATURATION_MULTIPLIER_2 2
#define SATURATION_MULTIPLIER_3 3
#define SATURATION_MULTIPLIER_4 4

#define DESATURATION_STEP_10prc 26	//10% от начального значения (останется после скейла)
#define DESATURATION_STEP_25prc 64	//25% от начального значения
#define DESATURATION_STEP_50prc 128	//50% от начального значения
#define DESATURATION_STEP_75prc 192	//75% от начального значения
#define DESATURATION_STEP_85prc 218	//85% от начального значения
#define DESATURATION_STEP_95prc 244 //95% от начального значения


//uses isToSaturateLed[]
void manageSaturationWithArray(byte onSaturatedAction, byte onDesaturatedAction, byte saturationMultiplyer, byte desaturationStep){	//SA_SATURATE_VALUE, SA_DESATURATE_VALUE, SA_DO_NOTHING_VALUE
  for (int i = 0; i < NUM_LEDS; i++) {
    if(isToSaturateLed[i] == SA_DO_NOTHING_VALUE ){
      continue;
    }

    if(isToSaturateLed[i] == SA_SATURATE_VALUE ){
      if (getPixBrightness(i) >= 250) {
        isToSaturateLed[i] = onSaturatedAction;
        continue;
      } else {
        leds[i]*=saturationMultiplyer;
      }
    }

    if(isToSaturateLed[i] == SA_DESATURATE_VALUE ){
      if ((uint32_t)getPixColor(i) == 0) {
        isToSaturateLed[i] = onDesaturatedAction;
        continue;
      } else {
        leds[i].nscale8(desaturationStep);
      }
    }
  }
}

// ****************************** ОГОНЁК ******************************
int16_t position;
boolean direction;


void lighter() {
  FastLED.clear();
  if (direction) {
    position++;
    leds[position] = CHSV(randomByte, 255, 50);
    if (position > NUM_LEDS - 2) {
      direction = false;
    }
  } else {
    position--;
    leds[position] = CHSV(randomByte, 255, 50);
    if (position < 1) {
      direction = true;
    }
  }
}

void lighter2() {
  if (direction) {
    leds[position] = CHSV(oppositeRandomByte, 255, 80);
    position++;
    leds[position] = CHSV(randomByte, 255, 255);
    if (position > NUM_LEDS - 2) {
      direction = false;
    }
  } else {
    leds[position] = CHSV(oppositeRandomByte, 255, 80);
    position--;
    leds[position] = CHSV(randomByte, 255, 255);
    if (position < 1) {
      direction = true;
    }
  }

}

// ****************************** СВЕТЛЯЧКИ ******************************
#define MAX_SPEED 30
#define BUGS_AMOUNT 4
int16_t speed[BUGS_AMOUNT];
int16_t pos[BUGS_AMOUNT];
CRGB bugColors[BUGS_AMOUNT];

void lightBugs() {
  if (loadingFlag) {
    loadingFlag = false;
    for (int i = 0; i < BUGS_AMOUNT; i++) {
      bugColors[i] = CHSV(random(0, 9) * 28, 255, 255);
      pos[i] = random(0, NUM_LEDS);
      speed[i] += random(-3, 3);
    }
  }
  fade();
  for (int i = 0; i < BUGS_AMOUNT; i++) {
    speed[i] += random(-3, 3);
    if (speed[i] == 0) speed[i] += (-3, 3);

    if (abs(speed[i]) > MAX_SPEED) speed[i] = 0;
    pos[i] += speed[i] / 10;
    if (pos[i] < 0) {
      pos[i] = 0;
      speed[i] = -speed[i];
    }
    if (pos[i] > NUM_LEDS - 1) {
      pos[i] = NUM_LEDS - 1;
      speed[i] = -speed[i];
    }
    leds[pos[i]] = bugColors[i];
  }
}

void lightBugs3() {
  if (loadingFlag) {
    loadingFlag = false;
    for (int i = 0; i < BUGS_AMOUNT; i++) {
      bugColors[i] = CHSV(random(0, 9) * 28, 255, 50);
      pos[i] = random(0, NUM_LEDS);
      speed[i] += random(-1, 2);
    }
  }
  for (int i = 0; i < BUGS_AMOUNT; i++) {
    speed[i] += random(-1, 2);
    if (speed[i] == 0) speed[i] += (-1, 2);

    if (abs(speed[i]) > MAX_SPEED) speed[i] = 0;
    pos[i] += speed[i] / 10;
    if (pos[i] < 0) {
      pos[i] = 0;
      speed[i] = -speed[i];
    }
    if (pos[i] > NUM_LEDS - 1) {
      pos[i] = NUM_LEDS - 1;
      speed[i] = -speed[i];
    }
    leds[pos[i]] = bugColors[i];
    isToSaturateLed[pos[i]] = SA_SATURATE_VALUE;
    manageSaturationWithArray(SA_DO_NOTHING_VALUE, SA_SATURATE_VALUE, SATURATION_MULTIPLIER_2, DESATURATION_STEP_50prc);
  }
}

// ****************************** ЦВЕТА ******************************
byte hue;
void colors() {
  hue += 2;
  CRGB thisColor = CHSV(hue, 255, 255);
  fillAll(CHSV(hue, 255, 255));
}

// ****************************** РАДУГА ******************************
void rainbow() {
  hue += 2;
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = CHSV((byte)(hue + i * float(255 / NUM_LEDS)), 255, 255);
}

void rainbowLong() {
  hue += 2;
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = CHSV((byte)(hue + i * float(255 / NUM_LEDS / 2)), 255, 255);
}

// ****************************** КАПЛИ ******************************  
// время от времени создается новая цветная точка, и заливает градиентно с учетом соседних цветов все вокруг.
byte my_randomLed;// Если нужна лента длиной более 255 диодов, увеличь тип данных
byte my_deltaColor = 1;
byte my_currentColor = 1;
byte my_currentBrightness = 0;
CHSV my_hsv; 
CRGB my_rgb;
byte my_skipCounter=0;
byte my_randomSkip=10;
void initRaindrops(){
    //выбираем случайный цвет для случайного пикселя
    my_randomLed = random(0, NUM_LEDS-1);
    my_baseColor = random(0, 255);
    my_hsv = CHSV(my_baseColor, 255, 30);// к этому значению цвета будут подтягиваться все остальные цвета
    leds[my_randomLed] = my_hsv;
    hsv2rgb_rainbow(my_hsv, my_rgb);// в конвертируем hsv-цвет в rgb. Результат будет в "my_rgb"
}
void raindrops() {
	if(random(0, 100) > 98){//иногда выбираем случайный цвет для случайного пикселя
		initRaindrops();
	}

    my_randomSkip = random(0, NUM_LEDS-1);
	for (int i = 0; i < NUM_LEDS; i++){// подтягиваем каждый пиксель по цвету к целевому

    if(i > my_randomSkip){//не доходим до конца, чтобы создать эффект волны (источник волны - нулевой пиксель)
      leds[i]--; // уменьшаем каждую составляющую (r,g,b) на 1. Это нужно чтобы в случае рандом скипа не накапливался белый цвет в скипнутых ледах
      continue;
    }
    my_currentBrightness = getPixBrightness[i];//получаем текущие значения цветов леда в переменные rPix, gPix, bPix;
		if(my_currentBrightness>250){
			continue;
		}

   //подтягиваем текущее значение цвета леда к целевому
		if(rPix < my_rgb.r){
      leds[i] += CRGB( my_deltaColor, 0, 0);
		} else {
			leds[i] -= CRGB( my_deltaColor, 0, 0);
		}
		if(gPix < my_rgb.g){
			leds[i] += CRGB( 0, my_deltaColor, 0); 
		} else {
			leds[i] -= CRGB( 0, my_deltaColor, 0);
		}
		if(bPix < my_rgb.b){
      leds[i] += CRGB( 0, 0, my_deltaColor); 
    } else {
      leds[i] -= CRGB( 0, 0, my_deltaColor);
		}
	}
}

void raindrops2() {//здесь волна идет не снизу лампы, а от опорного леда
  if(random(0, 100) > 97){//иногда выбираем случайный цвет для случайного пикселя
    initRaindrops();
  }

  my_randomSkip = random(0, NUM_LEDS-1);
  for (int i = 0; i < NUM_LEDS; i++){// подтягиваем каждый пиксель по цвету к целевому
      if(
      i > (my_randomLed + my_randomSkip)
    || i < (my_randomLed - my_randomSkip)
    ){//не доходим до конца, чтобы создать эффект волны (источник волны - опорный пиксель)
      leds[i]--; // уменьшаем каждую составляющую (r,g,b) на 1. Это нужно чтобы в случае рандом скипа не накапливался белый цвет в скипнутых ледах
      continue;
    }
    my_currentBrightness = getPixBrightness[i];//получаем текущие значения цветов леда в переменные rPix, gPix, bPix;
    if(my_currentBrightness>250){
      continue;
    }

   //подтягиваем текущее значение цвета леда к целевому
    if(rPix < my_rgb.r){
      leds[i] += CRGB( my_deltaColor, 0, 0);
    } else {
      leds[i] -= CRGB( my_deltaColor, 0, 0);
    }
    if(gPix < my_rgb.g){
      leds[i] += CRGB( 0, my_deltaColor, 0); 
    } else {
      leds[i] -= CRGB( 0, my_deltaColor, 0);
    }
    if(bPix < my_rgb.b){
      leds[i] += CRGB( 0, 0, my_deltaColor); 
    } else {
      leds[i] -= CRGB( 0, 0, my_deltaColor);
    }
  }
}

// ****************************** КОНФЕТТИ ******************************
byte thisNum = random(1, NUM_LEDS)-1;
//SA_SATURATE_VALUE, SA_DESATURATE_VALUE, SA_DO_NOTHING_VALUE
void sparkles(byte onSaturatedAction, byte onDesaturatedAction, byte saturationMultiplyer, byte desaturationStep){
  byte thisNum = random(1, NUM_LEDS)-1;
  if ((thisNum>NUM_LEDS/4) && getPixColor(thisNum) == 0){
    leds[thisNum] = CHSV((byte)(random(0, 255)), 255, 30);
    isToSaturateLed[thisNum] = SA_SATURATE_VALUE;
  }
  manageSaturationWithArray(onSaturatedAction, onDesaturatedAction, saturationMultiplyer, desaturationStep);
}

void sparkles6() {
  sparkles(SA_DESATURATE_VALUE, SA_SATURATE_VALUE, SATURATION_MULTIPLIER_2, DESATURATION_STEP_95prc);
}

void sparkles7() {
	//sparkles(SA_DESATURATE_VALUE, SA_DO_NOTHING_VALUE, SATURATION_MULTIPLIER_2, DESATURATION_STEP_95prc);
 sparkles(SA_DO_NOTHING_VALUE, SA_SATURATE_VALUE, SATURATION_MULTIPLIER_2, DESATURATION_STEP_75prc);
}

void sparkles8() {
	sparkles(SA_DESATURATE_VALUE, SA_SATURATE_VALUE, SATURATION_MULTIPLIER_2, DESATURATION_STEP_95prc);
}

void sparkles3() {
	sparkles(SA_DESATURATE_VALUE, SA_DO_NOTHING_VALUE, SATURATION_MULTIPLIER_2, DESATURATION_STEP_75prc);
}

void sparkles5() {
  sparkles(SA_DESATURATE_VALUE, SA_DO_NOTHING_VALUE, SATURATION_MULTIPLIER_2, DESATURATION_STEP_95prc);
}

// ****************************** СТАТИЧЕСКИЙ СВЕТ ******************************
/*
void staticColor(){
	for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(staticPaletteColors[i], 255 , staticColorBrightness);
  }
}
*/

// ****************** СЛУЖЕБНЫЕ ФУНКЦИИ *******************

void fade() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if ((uint32_t)getPixColor(i) == 0) continue;
    leds[i].fadeToBlackBy(TRACK_STEP);
  }
}

void resetSaturationArray(){
  for (int i = 0; i < NUM_LEDS; i++) {
    isToSaturateLed[i] = 0;
  }
}

void blink(byte times) {
	byte r = leds[0].r;
	byte g = leds[0].g;
	byte b = leds[0].b;
	for(int i = 0; i < times; i++){
		leds[0].setRGB(0, 0, 0); //black
		FastLED.show();
		delay(500);			// wait for a half-second
		leds[0].setRGB(250, 250, 250); //white
		FastLED.show();
		delay(500);
	}
	leds[0].setRGB(0, 0, 0);
	FastLED.show();
	delay(500);
	leds[0].setRGB(r, g, b);
	FastLED.show();
}

void longBlink(byte times) {
	byte r = leds[0].r;
	byte g = leds[0].g;
	byte b = leds[0].b;
	for(int i = 0; i < times; i++){
		leds[0].setRGB(0, 0, 0); //black
		FastLED.show();
		delay(500);			// wait for a half-second
		leds[0].setRGB(250, 250, 250); //white
		FastLED.show();
		delay(1500);
	}
	leds[0].setRGB(0, 0, 0);
	FastLED.show();
	delay(500);
	leds[0].setRGB(r, g, b);
	FastLED.show();
}
