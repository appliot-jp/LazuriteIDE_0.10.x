/* FILE NAME: bm1423gmv.c
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015  Lapis Semiconductor Co.,Ltd.
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

void setup() {
  byte rc;

  Serial.begin(115200);
  
  Wire.begin();
  
  rc = bm1423.init(0);
}

void loop() {
  byte rc;
  float mag[3];
  rc = bm1423.get_val(mag);

  if (rc == 0) {
    Serial.print("BM1423 XDATA=");
    Serial.println_double(mag[0], 2);
    Serial.print("BM1423 YDATA=");
    Serial.println_double(mag[1], 2);
    Serial.print("BM1423 ZDATA=");
    Serial.println_double(mag[2], 2);
    Serial.print("\n");    
  }
  
  delay(500);
}
