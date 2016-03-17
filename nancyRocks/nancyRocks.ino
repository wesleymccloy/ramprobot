int readingIR;

void setup() {
  Serial.begin(9600); 
  //pinMode(A1, INPUT);  
  
}

void loop() {
  readingIR = analogRead(A1); 
  Serial.println(readingIR);
  delay(100); 

}
