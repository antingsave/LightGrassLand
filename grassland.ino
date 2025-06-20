const int selectPins[3] = {2, 3, 4};  // A, B, C 選擇腳
const int muxOutputPin = A0;
const int pin9 = A1;

int ldrValues[9];

#include <Adafruit_NeoPixel.h>

#define PIN 6         // 数据线接 D6
#define WIDTH 8
#define HEIGHT 8
#define NUMPIXELS (WIDTH * HEIGHT)

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  //Serial.begin(9600); 
  for (int i = 0; i < 3; i++) {
    pinMode(selectPins[i], OUTPUT);
  }
  pixels.begin();
}

float time=0;
void loop() {
  for (int i = 0; i < 8; i++) {
    selectChannel(i);
    delayMicroseconds(10); // 小延遲穩定訊號
    ldrValues[i] = analogRead(muxOutputPin);

    // Serial.print("Channel ");
    // Serial.print(i);
    // Serial.print(": ");
    // Serial.println(ldrValues[i]);
  }
  ldrValues[8] = analogRead(pin9);
  
  float scale=0.45;
  float timescale=0.2149;
  float wind=0.2694;
 for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      int i = y * WIDTH + x;
      float val=bilinearInterpolate(x,y);
      val=smoothstep(200,1023,val);
      val=pow(val,1.5);
      //val=(1-cos(val*PI))/2;

      float px=x+y%2*0.5;
      float py=y*0.8660254;

      float ng = noise(px*scale, py*scale+time*wind,time*timescale);
      ng = smoothstep(0.2, 1, ng);
      ng=pow(ng,2);
      ng=(1-cos(ng*PI))/2;
      //n = 1.0 / (1.0 + exp(-10.0 * (n - 0.5)));
      float gg=val*0.95+ng*0.05;
      gg = int(gg * 255);  // 映射為亮度

      float nb = noise(px*scale, py*scale+time*wind,time*timescale+0.3);
      nb = smoothstep(0.2, 1, nb);
      nb=pow(nb,2);
      nb=(1-cos(nb*PI))/2;
      //n = 1.0 / (1.0 + exp(-10.0 * (n - 0.5)));
      float bb=val*0.95+nb*0.05;
      bb = int(bb * 255);  // 映射為亮度

      float brightness=val*0.95;
      brightness = int(brightness * 255);  // 映射為亮度
      pixels.setPixelColor(i, pixels.Color(brightness,lerp(gg,bb,0.49), lerp(bb,gg,0.49)));  // 綠色草原風
    }
  }
  pixels.show();
  time+=0.1;

  delay(10);
}

void selectChannel(int channel) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(selectPins[i], (channel >> i) & 1);
  }
}
int index(int x,int y){
    return x+y*3;
  }
float bilinearInterpolate(float px, float py) {
    float fx = px /7*2;
    int x0 = floor(fx);
    int x1 = min(x0 + 1, 2);
    float dx = fx - x0;

    float fy = py /7*2;  
    int y0 = floor(fy);
    int y1 = min(y0 + 1, 2);
    float dy = fy - y0;

    return    (1 - dx) * (1 - dy) * ldrValues[index(x0,y0)] +
              dx * (1 - dy) * ldrValues[index(x1,y0)] +
              (1 - dx) * dy * ldrValues[index(x0,y1)] +
              dx * dy * ldrValues[index(x1,y1)];

}
float smoothstep(float edge0, float edge1, float x) {
  x = constrain((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return x * x * (3 - 2 * x);
}
static int perm[512] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,
    140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,
    168, 68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,
    111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208,
    89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,
    186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,
    82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,
    183,170,213,119,248,152, 2,44,154,163,70,221,153,101,155,167,
    43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,
    185, 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,
    179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,
    199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,138,
    236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,

    // duplicate for overflow
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,
    140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,
    168, 68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,
    111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208,
    89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,
    186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,
    82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,
    183,170,213,119,248,152, 2,44,154,163,70,221,153,101,155,167,
    43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,
    185, 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,
    179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,
    199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,138,
    236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,
  };

  float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
  }

  float lerp(float a, float b, float t) {
    return a + t * (b - a);
  }

  float grad(int hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
  }

  float noise(float x, float y, float z) {
    x+=215.32478;
    y+=149.2579631;
    z+=91.32847;

    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    int A  = perm[X] + Y;
    int AA = perm[A] + Z;
    int AB = perm[A + 1] + Z;
    int B  = perm[X + 1] + Y;
    int BA = perm[B] + Z;
    int BB = perm[B + 1] + Z;

    float res = lerp(
      lerp(
        lerp(grad(perm[AA], x, y, z),
             grad(perm[BA], x - 1, y, z), u),
        lerp(grad(perm[AB], x, y - 1, z),
             grad(perm[BB], x - 1, y - 1, z), u), v),
      lerp(
        lerp(grad(perm[AA + 1], x, y, z - 1),
             grad(perm[BA + 1], x - 1, y, z - 1), u),
        lerp(grad(perm[AB + 1], x, y - 1, z - 1),
             grad(perm[BB + 1], x - 1, y - 1, z - 1), u), v),
      w);
    return (res + 1.0) * 0.5; // 0~1 範圍
  }
