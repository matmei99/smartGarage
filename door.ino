int r1 =10;
int r2 =9;
void setup() {
  pinMode(r1,OUTPUT);
  pinMode(r2,OUTPUT);

}

void loop() {
  
  delay(3000);
  openGarage();
  delay(3000);
  closeGarage();

}

void openGarage(){
  digitalWrite(r1,HIGH);
  digitalWrite(r2,LOW);
}

void closeGarage(){
  digitalWrite(r1,HIGH);
  digitalWrite(r2,LOW);
}
