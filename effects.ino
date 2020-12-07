// CHSV - Representation of an HSV pixel (hue, saturation, value (aka brightness)).

#define TRACK_STEP 5

byte isToSaturateLed[NUM_LEDS]; //0-ничего не делать; 1-ярче;  2-темнее.

#define SA_DO_NOTHING_VALUE 0
#define SA_SATURATE_VALUE 1
#define SA_DESATURATE_VALUE 2
#define SATURATION_MULTIPLIER_1_1 1.1
#define SATURATION_MULTIPLIER_1_2 1.2 // с дробными значениями что-то не то. такое ощущение что округляются
#define SATURATION_MULTIPLIER_1_3 1.3
#define SATURATION_MULTIPLIER_1_5 1.5
#define SATURATION_MULTIPLIER_2 2
#define SATURATION_MULTIPLIER_3 3
#define SATURATION_MULTIPLIER_4 4

#define DESATURATION_STEP_10prc 26	//10% от начального значения (останется после скейла)
#define DESATURATION_STEP_25prc 64	//25% от начального значения
#define DESATURATION_STEP_50prc 128	//50% от начального значения
#define DESATURATION_STEP_75prc 192	//75% от начального значения
#define DESATURATION_STEP_85prc 218	//85% от начального значения
#define DESATURATION_STEP_95prc 244 //95% от начального значения
#define DESATURATION_STEP_98prc 250 //98% от начального значения
#define DESATURATION_STEP_99prc 253 //99% от начального значения

//saColorUsedComponents: SA_R=1; SA_G=2; SA_B=3; SA_RGB=123; SA_RG=12; SA_GB=23; SA_RB=13; - какие компоненты цвета использовать. Монотонный цвет в данном режиме весьма красив.
#define SA_R 1
#define SA_G 2
#define SA_B 3
#define SA_RGB 123
#define SA_RG 12
#define SA_GB 23
#define SA_RB 13


byte oldR = 0;
byte oldG = 0;
byte oldB = 0;
int newR = 0;
int newG = 0;
int newB = 0;
//uses isToSaturateLed[]
void manageSaturationWithArray(byte onSaturatedAction, byte onDesaturatedAction, byte saturationMultiplyer, byte desaturationStep){	//SA_SATURATE_VALUE, SA_DESATURATE_VALUE, SA_DO_NOTHING_VALUE
  for (int i = 0; i < NUM_LEDS; i++) {
    if(isToSaturateLed[i] == SA_DO_NOTHING_VALUE ){
      continue;
    }

    if(isToSaturateLed[i] == SA_SATURATE_VALUE ){
		if (getPixBrightness(i) >= 250) {
			isToSaturateLed[i] = onSaturatedAction;
		} else {
			oldR=leds[i].r;
			oldG=leds[i].g;
			oldB=leds[i].b;
			newR = oldR*saturationMultiplyer;
			newG = oldG*saturationMultiplyer;
			newB = oldB*saturationMultiplyer;
			if(newR==oldR && oldR>0){
				newR=newR+2;//минимальный сдвиг для дробных saturationMultiplyer, чтобы не застревать
			}
			if(newG==oldG && oldG>0){
				newG=newG+2;//минимальный сдвиг для дробных saturationMultiplyer, чтобы не застревать
			}
			if(newB==oldB && oldB>0){
				newB=newB+2;//минимальный сдвиг для дробных saturationMultiplyer, чтобы не застревать
			}
			if(newR>255){newR=255;}
			if(newG>255){newG=255;}
			if(newB>255){newB=255;}
		  
			leds[i].r=(byte)newR;
			leds[i].g=(byte)newG;
			leds[i].b=(byte)newB;
		}
		continue;
    }

    if(isToSaturateLed[i] == SA_DESATURATE_VALUE ){
      if ((uint32_t)getPixColor(i) == 0) {
        isToSaturateLed[i] = onDesaturatedAction;
      } else {
		oldR=leds[i].r;
		oldG=leds[i].g;
		oldB=leds[i].b;
        leds[i].nscale8(desaturationStep);
		newR = leds[i].r;
		newG = leds[i].g;
		newB = leds[i].b;
		if(newR==oldR && oldR != 0){
			newR = newR - 1;
		}
		if(newG==oldG && oldG != 0){
			newG = newG - 1;
		}
		if(newB==oldB && oldB != 0){
			newB = newB - 1;
		}
		leds[i].r=(byte)newR;
		leds[i].g=(byte)newG;
		leds[i].b=(byte)newB;
      }
    }
  }
}

byte rAddition = 0;
byte gAddition = 0;
byte bAddition = 0;
//saColorUsedComponents: SA_R=1; SA_G=2; SA_B=3; SA_RGB=123; SA_RG=12; SA_GB=23; SA_RB=13; - какие компоненты цвета использовать. Монотонный цвет в данном режиме весьма красив.
void manageSaturationWithArray2(byte onSaturatedAction, byte onDesaturatedAction, byte saturationStep, byte desaturationStep, byte saColorUsedComponents){	//SA_SATURATE_VALUE, SA_DESATURATE_VALUE, SA_DO_NOTHING_VALUE
  for (int i = 0; i < NUM_LEDS; i++) {
    if(isToSaturateLed[i] == SA_DO_NOTHING_VALUE ){
      continue;
    } 

    if(isToSaturateLed[i] == SA_SATURATE_VALUE ){
      if (getPixBrightness(i) >= 255-saturationStep) {
        isToSaturateLed[i] = onSaturatedAction;
      } else {
		  if(saColorUsedComponents == SA_RGB || saColorUsedComponents == SA_R || saColorUsedComponents == SA_RG || saColorUsedComponents == SA_RB){
			  rAddition = saturationStep;
		  } else {
			  rAddition = 0;
		  }
		  if(saColorUsedComponents == SA_RGB || saColorUsedComponents == SA_G || saColorUsedComponents == SA_RG || saColorUsedComponents == SA_GB){
			  gAddition = saturationStep;
		  } else {
			  gAddition = 0;
		  }
		  if(saColorUsedComponents == SA_RGB || saColorUsedComponents == SA_B || saColorUsedComponents == SA_GB || saColorUsedComponents == SA_RB){
			  bAddition = saturationStep;
		  } else {
			  bAddition = 0;
		  }
		  leds[i] += CRGB( rAddition, gAddition, bAddition);
		//leds[i].addToRGB(saturationStep);
      }
	  continue;
    }

    if(isToSaturateLed[i] == SA_DESATURATE_VALUE ){
      if ((uint32_t)getPixColor(i) < desaturationStep) {
        isToSaturateLed[i] = onDesaturatedAction;
      } else {
		  if(saColorUsedComponents == SA_RGB || saColorUsedComponents == SA_R || saColorUsedComponents == SA_RG || saColorUsedComponents == SA_RB){
			  rAddition = desaturationStep;
		  } else {
			  rAddition = 0;
		  }
		  if(saColorUsedComponents == SA_RGB || saColorUsedComponents == SA_G || saColorUsedComponents == SA_RG || saColorUsedComponents == SA_GB){
			  gAddition = desaturationStep;
		  } else {
			  gAddition = 0;
		  }
		  if(saColorUsedComponents == SA_RGB || saColorUsedComponents == SA_B || saColorUsedComponents == SA_GB || saColorUsedComponents == SA_RB){
			  bAddition = desaturationStep;
		  } else {
			  bAddition = 0;
		  }
		  leds[i] -= CRGB( rAddition, gAddition, bAddition);
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
      //leds[i]--; // уменьшаем каждую составляющую (r,g,b) на 1. Это нужно чтобы в случае рандом скипа не накапливался белый цвет в скипнутых ледах
	  leds[i].nscale8_video(2);
      continue;
    }
    my_currentBrightness = getPixBrightness(i);//получаем текущие значения цветов леда в переменные rPix, gPix, bPix;
    if(my_currentBrightness>250){
      continue;
    }

   //подтягиваем текущее значение цвета леда к целевому
    if(rPix < my_rgb.r){
      leds[i] += CRGB( my_deltaColor, 0, 0);
    } else if(rPix > my_rgb.r) {
      leds[i] -= CRGB( my_deltaColor, 0, 0);
    }
    if(gPix < my_rgb.g){
      leds[i] += CRGB( 0, my_deltaColor, 0); 
    } else if(gPix > my_rgb.g) {
      leds[i] -= CRGB( 0, my_deltaColor, 0);
    }
    if(bPix < my_rgb.b){
      leds[i] += CRGB( 0, 0, my_deltaColor); 
    } else if(bPix < my_rgb.b){
      leds[i] -= CRGB( 0, 0, my_deltaColor);
    }
  }
}

// ****************************** КОНФЕТТИ ******************************
byte thisNum = random(1, NUM_LEDS)-1;
//SA_SATURATE_VALUE, SA_DESATURATE_VALUE, SA_DO_NOTHING_VALUE
void sparkles(byte newFireFrequency, byte saturationMultiplyer, byte desaturationStep, byte onSaturatedAction, byte onDesaturatedAction){
  byte thisNum = random(1, NUM_LEDS)-1;
  if (
	(random(0, 10)<newFireFrequency)
	 && (
		(onSaturatedAction == SA_DESATURATE_VALUE && leds[thisNum].r <10 && leds[thisNum].g <10 && leds[thisNum].b <10)
		||
		(onSaturatedAction == SA_DO_NOTHING_VALUE)
		)
	){
		leds[thisNum] = CHSV(random(0, 255), 255, 30);
		isToSaturateLed[thisNum] = SA_SATURATE_VALUE;
  }
  manageSaturationWithArray(onSaturatedAction, onDesaturatedAction, saturationMultiplyer, desaturationStep);
}

void sparklesIncremental(byte newFireFrequency, byte saturationMultiplyer, byte desaturationStep, byte onSaturatedAction, byte onDesaturatedAction, byte saColorUsedComponents){
  byte thisNum = random(1, NUM_LEDS)-1;
  if (
	(random(0, 10)<newFireFrequency)
	 && (
		(onSaturatedAction == SA_DESATURATE_VALUE && leds[thisNum].r <10 && leds[thisNum].g <10 && leds[thisNum].b <10)
		||
		(onSaturatedAction == SA_DO_NOTHING_VALUE)
		)
	){
		leds[thisNum] = CHSV(random(0, 255), 255, 30);
		isToSaturateLed[thisNum] = SA_SATURATE_VALUE;
  }
  manageSaturationWithArray2(onSaturatedAction, onDesaturatedAction, saturationMultiplyer, desaturationStep, saColorUsedComponents);
}



void sparkles1() {
  sparkles(5, SATURATION_MULTIPLIER_2, DESATURATION_STEP_85prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}

void sparkles2() {
 sparkles(5, SATURATION_MULTIPLIER_2, DESATURATION_STEP_95prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
/*
void sparkles3() {
	sparkles(10, SATURATION_MULTIPLIER_2,  DESATURATION_STEP_95prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
*/
void sparkles4() {
	sparkles(1, SATURATION_MULTIPLIER_2, DESATURATION_STEP_95prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}

void sparkles5() {
  sparkles(1, SATURATION_MULTIPLIER_2, DESATURATION_STEP_85prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
/*
void sparkles6() {
  sparkles(2, SATURATION_MULTIPLIER_2, DESATURATION_STEP_85prc, SA_DO_NOTHING_VALUE, SA_SATURATE_VALUE);
}
*/
void sparkles7() {
  sparkles(2, SATURATION_MULTIPLIER_2, DESATURATION_STEP_85prc, SA_DESATURATE_VALUE, SA_DESATURATE_VALUE);
}
/*
void sparkles8() {
  sparkles(10, SATURATION_MULTIPLIER_2, DESATURATION_STEP_95prc, SA_DO_NOTHING_VALUE, SA_SATURATE_VALUE);
}
*/
void sparkles9() {
  sparkles(10, SATURATION_MULTIPLIER_2, DESATURATION_STEP_95prc, SA_DESATURATE_VALUE, SA_DESATURATE_VALUE);
}
/*
void sparkles10() {
  sparkles(2, SATURATION_MULTIPLIER_2, DESATURATION_STEP_95prc, SA_DO_NOTHING_VALUE, SA_SATURATE_VALUE);
}
*/

//saColorUsedComponents: SA_R=1; SA_G=2; SA_B=3; SA_RGB=123; SA_RG=12; SA_GB=23; SA_RB=13; - какие компоненты цвета использовать. Монотонный цвет в данном режиме весьма красив.
void sparkles11() {
  sparklesIncremental(2, 5, 5, SA_DESATURATE_VALUE, SA_SATURATE_VALUE, SA_R);
}
void sparkles12() {
  sparklesIncremental(2, 10, 5, SA_DESATURATE_VALUE, SA_SATURATE_VALUE, SA_G);
}
void sparkles13() {
  sparklesIncremental(2, 5, 10, SA_DESATURATE_VALUE, SA_SATURATE_VALUE, SA_B);
}
void sparkles14() {
  sparklesIncremental(5, 5, 5, SA_DESATURATE_VALUE, SA_SATURATE_VALUE, SA_RG);//желтый
}
void sparkles15() {
  sparklesIncremental(5, 5, 5, SA_DESATURATE_VALUE, SA_SATURATE_VALUE, SA_RB);
}
void sparkles16() {
  sparklesIncremental(5, 5, 5, SA_DESATURATE_VALUE, SA_SATURATE_VALUE, SA_GB);
}


void sparkles17() {
  sparkles(10, SATURATION_MULTIPLIER_2, DESATURATION_STEP_98prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
void sparkles18() {
  sparkles(2, SATURATION_MULTIPLIER_2, DESATURATION_STEP_99prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
/*
void sparkles19() {
  sparkles(2, SATURATION_MULTIPLIER_1_1, DESATURATION_STEP_99prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
*/
void sparkles20() {
  sparkles(10, SATURATION_MULTIPLIER_1_1, DESATURATION_STEP_99prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
void sparkles21() {
  sparkles(10, SATURATION_MULTIPLIER_1_1, DESATURATION_STEP_85prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
void sparkles22() {
  sparkles(2, SATURATION_MULTIPLIER_1_1, DESATURATION_STEP_85prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
void sparkles23() {
  sparkles(2, SATURATION_MULTIPLIER_1_3, DESATURATION_STEP_85prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
/*
void sparkles24() {
  sparkles(2, SATURATION_MULTIPLIER_1_5, DESATURATION_STEP_95prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
*/



void sparkles25() {
  sparkles(1, SATURATION_MULTIPLIER_2, DESATURATION_STEP_98prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
void sparkles26() {
  sparkles(1, SATURATION_MULTIPLIER_2, DESATURATION_STEP_99prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}
void sparkles27() {
  sparkles(1, SATURATION_MULTIPLIER_1_1, DESATURATION_STEP_99prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}

void sparkles29() {
  sparkles(1, SATURATION_MULTIPLIER_1_1, DESATURATION_STEP_85prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
}

void sparkles31() {
  sparkles(1, SATURATION_MULTIPLIER_1_3, DESATURATION_STEP_85prc, SA_DESATURATE_VALUE, SA_SATURATE_VALUE);
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
    isToSaturateLed[i] = SA_DESATURATE_VALUE;   //ошибка была здесь. при вызове Sparcles при множестве горящих ледов и обнуленном isToSaturateLed (SA_DO_NOTHING_VALUE) создавалось тупиковое состояние.
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

void shortBlink(byte times) {
	byte r = leds[0].r;
	byte g = leds[0].g;
	byte b = leds[0].b;
	for(int i = 0; i < times; i++){
		leds[0].setRGB(0, 0, 0); //black
		FastLED.show();
		delay(100);			// wait for a half-second
		leds[0].setRGB(250, 250, 250); //white
		FastLED.show();
		delay(100);
	}
	leds[0].setRGB(0, 0, 0);
	FastLED.show();
	delay(100);
	leds[0].setRGB(r, g, b);
	FastLED.show();
}
