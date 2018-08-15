#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include <stdlib.h>

// Pin definitions
#define CLOCK_PIN  D1
#define DATA_PIN   D2
#define CLOCK_PIN2 D3
#define DATA_PIN2  D4

// LED info
#define LED_COUNT 230
#define MAX_BRIGHTNESS 31



// The array which holds the strip data for each pixel
CRGB stripColors[LED_COUNT];

// My color choices
static CRGB TURN_OFF = CRGB(0, 0, 0);

// RED colors
static CRGB colorRed         = CRGB(255, 0, 0);
static CRGB colorOrangeRed   = CRGB(255,69,0);
static CRGB colorCoral       = CRGB(255, 127, 80);

// Yellow colors
static CRGB colorYellow          = CRGB(255,255,0);
static CRGB colorDarkGoldenRod   = CRGB(184,134,11);
static CRGB colorKhaki           = CRGB(240, 230, 140);

// Green colors
static CRGB colorChartReuse    = CRGB(127, 255, 0);
static CRGB colorPaleGreen     = CRGB(152, 251, 152);
static CRGB colorSpringGreen   = CRGB(0, 255, 127);

// Blue colors
static CRGB colorBlue        = CRGB(0, 0, 255);
static CRGB colorAqua        = CRGB(0, 255, 255); 
static CRGB colorDeepSkyBlue = CRGB(0, 191, 255);

// White colors
static CRGB colorWhite         = CRGB(255, 255, 255);
static CRGB colorSnowWhite     = CRGB(255, 250, 250);
static CRGB colorWhiteSmoke    = CRGB(245, 245, 245);

const int colorCount = 15;
static CRGB colorList[colorCount] = {
  colorRed,
  colorOrangeRed,
  colorCoral,
  colorYellow,
  colorDarkGoldenRod,
  colorKhaki,
  colorChartReuse,
  colorPaleGreen,
  colorSpringGreen,
  colorBlue,
  colorAqua,
  colorDeepSkyBlue,
  colorWhite,
  colorSnowWhite,
  colorWhiteSmoke
};

/*
 * Uses the fast LED API to turn on the LEDs based on the stripColor array 
 */
void showStrip(uint8_t brightness = MAX_BRIGHTNESS) {
   FastLED.show(brightness);
}

/*
 * Uses a pre-set RGB color to set the stripColor array to a specific color 
 */
void setStripsColor(CRGB color) {
  for(int i = 0; i != LED_COUNT; i++) {
    stripColors[i] = color;
  }
}

/*
 * Sets all the LEDs to color BLACK 
 */
void clearPanels() {
  setStripsColor(TURN_OFF);
  FastLED.show();
}

/*
 * Sets the individual RGB color for each pixel of the strip
 */
void setPixel(int Pixel, byte _red, byte _green, byte _blue) {
   // FastLED
   stripColors[Pixel].r = _red;
   stripColors[Pixel].g = _green;
   stripColors[Pixel].b = _blue;
}

/*
 * Same as setStrip but uses individual RGB bytes
 */
void setAll(byte _red, byte _green, byte _blue) {
  for(int i = 0; i < LED_COUNT; i++ ) {
    setPixel(i, _red, _green, _blue); 
  }
  showStrip();
}

/*
 * Simple function for dimming the lights in and out into different colors
 */
void fadeInFadeOut() {
  for(int j = 0; j < 3; j++ ) { 
    // Fade IN
    for(int k = 0; k < 256; k++) { 
      switch(j) { 
        case 0: setAll(k,0,0); break;
        case 1: setAll(0,k,0); break;
        case 2: setAll(0,0,k); break;
      }
      showStrip();
      delay(3);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) { 
      switch(j) { 
        case 0: setAll(k,0,0); break;
        case 1: setAll(0,k,0); break;
        case 2: setAll(0,0,k); break;
      }
      showStrip();
      delay(3);
    }
  }
}

/*
 * Sets the lower parts of the strip to a cooler color and upper LEDs to a warmer color
 */
void fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[LED_COUNT];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < LED_COUNT; i++) {
    cooldown = random(0, ((Cooling * 10) / LED_COUNT) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= LED_COUNT - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < LED_COUNT; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

/*
 * Every color LED follows with 3 OFF LEDs and the animation moves forward
 * Like old school theatre signs
 */
void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < LED_COUNT; i=i+3) {
        setPixel(i+q, red, green, blue);    //turn every third pixel on
      }
      showStrip();
     
      delay(SpeedDelay);
     
      for (int i=0; i < LED_COUNT; i=i+3) {
        setPixel(i+q, 0,0,0);        //turn every third pixel off
      }
    }
  }
}

/*
 * The tip of the animation is the brightets and as the animation moves forward the tail of the strip
 * looses brightness and eventually dies
 */
void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
  
  for(int i = 0; i < LED_COUNT+LED_COUNT; i++) {
    
    
    // fade brightness all LEDs one step
    for(int j=0; j<LED_COUNT; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
    
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <LED_COUNT) && (i-j>=0) ) {
        setPixel(i-j, red, green, blue);
      } 
    }

    // TODO: change this to all pillars, right? 
    for(int i=0; i < 2; i++) {
      
      showStrip();
    }
    //showStrip();
    delay(SpeedDelay);
  }
}

/*
 * FastLED API to eventually fade a pixel to black. 
 * TODO: Needs further testing and debugging
 */
void fadeToBlack(int ledNo, byte fadeValue) {
   // FastLED
   stripColors[ledNo].fadeToBlackBy( fadeValue );
}

/*
 * Feels like the colors reach the bottom of the strip and bounce back up
 */
void bouncingBalls(byte red, byte green, byte blue, int BallCount) {
  float Gravity = -9.81;
  int StartHeight = 1;
  
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
  
  for (int i = 0 ; i < BallCount ; i++) {   
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0; 
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2); 
  }

  while (true) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
  
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
  
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (LED_COUNT - 1) / StartHeight);
    }
  
    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i],red,green,blue);
    }
    
    showStrip();
    setAll(0,0,0);
    delay(100);
  }
}

void runningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position=0;
  
  for(int j=0; j<LED_COUNT*2; j++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<LED_COUNT; i++) {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);
      }

      for(int i=0; i < 6; i++) {
        
        showStrip();
      }
      //showStrip();
      delay(WaveDelay);
  }
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(LED_COUNT);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause){
  for(int j = 0; j < StrobeCount; j++) {
    setAll(red,green,blue);
    showStrip();
    delay(FlashDelay);
    setAll(0,0,0);
    showStrip();
    delay(FlashDelay);
  }
 
 delay(EndPause);
}


/* TODO: Fix this for the FastLED API */
void bringToLife(CRGB color, int _speed,  uint8_t brightness = 0) {
  setStripsColor(color);
  while (brightness < MAX_BRIGHTNESS) {
    brightness++;
    FastLED.show(brightness);
    delay(_speed);
  }
}

/*
void bringToLifeOne(CRGB color, int _speed, uint8_t pixel, uint8_t brightness = 0) {
  setStripsColor(color);
  writeToMux(pixel);
  while (brightness < MAX_BRIGHTNESS) {
    brightness++;
    ledStrip.write(stripColors, LED_COUNT, brightness);
    delay(_speed);
  }
}

void showOnePixel(uint8_t pixel, CRGB color, uint8_t brightness = MAX_BRIGHTNESS) {
  setStripsColor(color);
  writeToMux(pixel);
  ledStrip.write(stripColors, LED_COUNT, brightness);
}

*/

/*
 * Turns all of the pixels in the strip ON 
 */
void showAllPixels(CRGB color, uint8_t brightness = MAX_BRIGHTNESS) {
    setStripsColor(color); // We're placing it here because in case the clearFlag is set then this array has to be filled again
    FastLED.show();
}

/*
 * Shows each pixel individually for a certain pre-set color. Previouse LEDs stay ON
 */
void showOnebyOne(CRGB color,  int _speed = 0, uint8_t brightness = MAX_BRIGHTNESS) {
  for (uint8_t i=0; i < LED_COUNT; i++) {
    stripColors[i] = color;
    FastLED.show(brightness);
    delay(_speed);
  }
}

/*
 * Shows LEDs moving up the strip. The tails determines the length of the pixels that are ON at each cycle
 */
void showOneAtATimeUp(CRGB color,  int tailLength = 1, int _speed = 0, uint8_t brightness = MAX_BRIGHTNESS) {
  for (uint8_t i=0; i < LED_COUNT; i++) {
    stripColors[i] = color;
    if (i >= tailLength) {
      stripColors[i-tailLength] = TURN_OFF;
    }
    FastLED.show(brightness);
    delay(_speed);
  }
}

/*
 * Same as above but in reverse direction
 */
void showOneAtATimeDown(CRGB color,  int tailLength = 1, int _speed = 0, uint8_t brightness = MAX_BRIGHTNESS) {
  for (uint8_t i=LED_COUNT-1; i > 0; i--) {
    stripColors[i] = color;
    if (LED_COUNT - i > tailLength) {
      stripColors[i+tailLength] = TURN_OFF;
    }
    FastLED.show(brightness);
    delay(_speed);
  }
}

/*
 * Start a pixel from each end of the strip and meet in the middle
 */
void showFromOpposingEnds(CRGB color,  int tailLength = 1, int _speed = 0, uint8_t brightness = MAX_BRIGHTNESS) {
  uint8_t i = LED_COUNT-1;
  uint8_t j = 0;
  while(i > j) {
    stripColors[i] = color;
    stripColors[j] = color;
    if (LED_COUNT - i > tailLength) {
      stripColors[i+tailLength] = TURN_OFF;
    }
    if (j > tailLength && j - tailLength >=0) {
      stripColors[j-tailLength] = TURN_OFF;
    }

    FastLED.show(brightness);
    delay(_speed);
    
    i--;
    j++;
  }
}

/*
 * Start from the middle of the strip and go to the opposite ends
 */
void showFromMiddleToEnd(CRGB color,  int tailLength = 1, int _speed = 0, uint8_t brightness = MAX_BRIGHTNESS) {
  int halfWay = LED_COUNT / 2;
  
  int i = halfWay;
  int iTail = halfWay - tailLength;
  
  int j = halfWay;
  int jTail = halfWay + tailLength;
  
  while(iTail < LED_COUNT && jTail >= 0) {
    if (i < LED_COUNT) {
      stripColors[i] = color;
    }

    if (j >= 0) {
      stripColors[j] = color;
    }

    if (iTail >= halfWay) {
      stripColors[iTail] = TURN_OFF;
    }

    if (jTail <= halfWay) {
      stripColors[jTail] = TURN_OFF;
    }

      FastLED.show(brightness);
      
    delay(_speed);
    
    i++;
    iTail++;
    
    j--;
    jTail--;
  }
}


void fireBall( int _speed = 0, uint8_t brightness = MAX_BRIGHTNESS) {
  CRGB color = CRGB(255, 0, 0);
  for (uint8_t i=LED_COUNT-1; i >=0 ; i--) {
    //stripColors[i] = color;
    stripColors[i].setRGB(i, i, i);
    FastLED.show();
  }
}


/* Converts a color from HSV to RGB.
 * h is hue, as a number between 0 and 360.
 * s is the saturation, as a number between 0 and 255.
 * v is the value, as a number between 0 and 255. */
CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v) {
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return CRGB(r, g, b);
}

/*
 * Online rainbow method for displaying colorful strip while slowly moving the colors up the strip
 */
void showRainbow( uint8_t brightness = MAX_BRIGHTNESS) {
  uint8_t time = millis() >> 4;

  for (uint8_t j=0; j < LED_COUNT; j++) {
      uint8_t p = time - j * 8;
      CRGB color = hsvToRgb((uint32_t)p * 359 / 256, 255, 255);
      stripColors[j] = color;
  }
      
  FastLED.show(brightness);
}

/*
 * TODO: Needs testing with FastLED API
*/
/*
void showGradient( RGBSelect colorCase, uint8_t brightness = MAX_BRIGHTNESS) {
  uint8_t time = millis() >> 2;
  for (uint8_t i=batch.first; i <= batch.last; i++) {
    uint8_t x = time - i * 8;
    CRGB color;
    switch (colorCase) {
      case Red:
        color = CRGB(255 - x, x, x);
        break;
      case Green:
        color = CRGB(x, 255 - x, x);
        break;
      case Blue:
        color = CRGB(x, x, 255 - x);
        break;
    }
    //CRGB color = CRGB(255 - x, x, x);
    
    setStripsColor(color);
    ledStrip.write(stripColors, LED_COUNT, brightness);
  }
}
*/

// TODO: How do I check to see if an argument didn't get a value. I'm loading skipInt with -1
// but ideally we should just be able to check if that optional skipInt wasn't passed and skip the while loop
// NOTE: For the individual controller for each pillar, we're not going to use any randomness
int getRandomNumber(int upperLimit, int lowerLimit = 0) {
  int n = (rand() % (upperLimit - lowerLimit)) + lowerLimit;
  if (n > upperLimit) {
    n = upperLimit; // Just a stupid way of making sure it stays within limit 
  }
  return n;
}

void setup() {
  // FastLED method for setting up the clock and data pin of the LED strips
  // For multiple strips you can just keep stacking up the function calls. 
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(stripColors, LED_COUNT).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<APA102, DATA_PIN2, CLOCK_PIN2, BGR>(stripColors, LED_COUNT).setCorrection( TypicalLEDStrip );

  Serial.begin(9600);
  Serial.println("Setting up debug mode");

  clearPanels();
}

uint8_t currentColorInt = 0; // This integer gets used for picking a color from the color list
uint8_t animationCase = 0; // integer for choosing which animation set to play


void loop() {
  // put your main code here, to run repeatedly:

  if (currentColorInt > colorCount) {
    currentColorInt = 0; // restart the color picker if the count exceeds the list of available colors
  }
  CRGB chosenColor = colorList[currentColorInt];
  
  
  /*
   * This makes for a good test program
   */
  //showOnebyOne(chosenColor, 10);

  switch(animationCase) {
    case 0:
      /*
      * LEDs to go up individually with a tail and come back down.
      * Repeat with different speeds
      */
      showOneAtATimeUp(chosenColor, 5, 15);
      showOneAtATimeDown(chosenColor, 5, 15);
      showOneAtATimeUp(chosenColor, 10, 10);
      showOneAtATimeDown(chosenColor, 10, 10);
      showOneAtATimeUp(chosenColor, 15, 5);
      showOneAtATimeDown(chosenColor, 15, 5);
      break;
    case 1:
      showFromOpposingEnds(chosenColor, 5, 15);
      showFromOpposingEnds(chosenColor, 10, 10);
      showFromOpposingEnds(chosenColor, 15, 5);
    default:
      showOnebyOne(chosenColor, 10);
      break;
  }

  currentColorInt++;
  
}

/*
  showOnebyOne(randomColor, PillarOne, 10);
  clearPanels(PillarOne);
  showOneAtATimeUp(randomColor, PillarOne, 10, 10);
  clearPanels(PillarOne);
  //showAllPixels(randomColor, PillarOne, 250);
  //delay(1000);
  //clearPanels(PillarOne);

  /*
  switch(n) {
    case 0:
      // This will turn on one strip at a time leaving the previous one untouched
      // Then clears all and starts from the opposite side
      for (uint8_t i=0; i < 6; i++) {
        showAllPixels(randomColor, AllPillars, 250);
        clearPanels(AllPillars);
        delay(10);
        showAllPixelsReverse(randomColor, AllPillars, 250);
        clearPanels(AllPillars);
        delay(10);
      }
      break;
    case 1:
      // This will turn on one strip at a time and turns off the previous strip
      // Once end is reached, it starts from the opposite side 
      for (uint8_t i=0; i < 6; i++) {
        showAllPixels(randomColor, AllPillars, 250, true);
        showAllPixelsReverse(randomColor, AllPillars, 250, true);
      }
      break;
    case 2:
      // Shows each pillar individually 
      showOnebyOne(randomColor, PillarOne, 10);
      clearPanels(PillarOne);
      showOnebyOne(randomColor, PillarTwo, 10);
      clearPanels(PillarTwo);
      showOnebyOne(randomColor, PillarThree, 10);
      clearPanels(PillarThree);
      showOnebyOne(randomColor, AllPillars, 0);
      clearPanels(AllPillars);
      break;
    case 3:
      // All pillars going from bottom to top with a tail
      showOneAtATimeUp(randomColor, AllPillars, 10);
      clearPanels(AllPillars);
    case 4:
      // Individual pillars going from bottom to top and then all with an explosion at the end
      showOneAtATimeUp(randomColor, PillarOne, 10);
      clearPanels(PillarOne);
      showOneAtATimeUp(randomColor, PillarThree, 10);
      clearPanels(PillarThree);
      showOneAtATimeUp(randomColor, PillarTwo, 10);
      clearPanels(PillarTwo);
      showAllPixels(randomColor, AllPillars, 0);
      delay(1500);
      clearPanels(AllPillars);
    case 5:
      // Zig zag going up and down from each pillar 
      showOneAtATimeUp(randomColor, PillarOne, 10);
      clearPanels(PillarOne);
      showOneAtATimeDown(randomColor, PillarTwo, 10);
      clearPanels(PillarTwo);
      showOneAtATimeUp(randomColor, PillarThree, 10);
      clearPanels(PillarThree);
      showOneAtATimeDown(randomColor, PillarThree, 10);
      clearPanels(PillarThree);
      showOneAtATimeUp(randomColor, PillarTwo, 10);
      clearPanels(PillarTwo);
      showOneAtATimeDown(randomColor, PillarOne, 10);
      clearPanels(PillarOne);
  }
  */

  
  //writeToMux(0);
  //fadeInFadeOut();
  //fire(20,120,40);
  //theaterChase(0xff,0,0,100);
  //meteorRain(0xff,0xff,0xff,10, 64, true, 30);
  //bouncingBalls(0xff,0,0, 3);
  //runningLights(0xff,0xff,0x00, 50); // This has the mux coded
  //Sparkle(0xff, 0xff, 0xff, 0);
  //Strobe(0xff, 0xff, 0xff, 10, 100, 1000);
  
  /*
  // Show each pixel coming down the strip
  showOneAtATimeDown(randomColor, AllPillars, 10);
  clearPanels(AllPillars);
  */

  
  // Show the pixels starting from opposite end and meeting in the middle
  //showFromOpposingEnds(randomColor, AllPillars, 10);
  //clearPanels(AllPillars);

  //showFromMiddleToEnd(randomColor, AllPillars, 10);
  //clearPanels(AllPillars);

  //clearPanels(AllPillars);
  //fireBall(AllPillars);


  //showRainbow(AllPillars); 
  /*
  // Show the rainbow animation
  for (uint8_t i=0; i < 20; i++) {
    showRainbow(AllPillars);  
  }
  
 */
