#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
// bel auth 95f47366505542158eaf88129a4241a0
// vini auth 642b4342a8f743e699da137b65b4b69c
char auth[] = "95f47366505542158eaf88129a4241a0";
char ssid[] = "NodeMCU";
char pass[] = "password";

#define IR_LED 4 //D2
#define IR_PIN_RECEIVER 14 //D5

IRsend irsend(IR_LED);
IRrecv irrecv(IR_PIN_RECEIVER);

decode_results results;


int pinvalue;
bool receive_on = false;
uint32_t trained_code;
uint32_t code_to_send;
uint32_t temp_air [12] = {0x8808F4B , 0x8808E4A, 0x8808D49, 0x8808C48, 0x8808B47, 0x8808A46, 
                          0x8808945, 0x8808743, 0x8808642, 0x8808541, 0x8808440, 0x880834F};


void handler_trainer(){
  while (true){
    if (irrecv.decode(&results)) {
    trained_code = results.value;
    break;
  }
  delay(2000);  
  }
}

void handler_receiver(){
  if (irrecv.decode(&results) && receive_on == true) {
    Serial.print("captured IR: ");
    serialPrintUint64(results.value, HEX);
    Serial.print(" : ");
    serialPrintUint64(results.value);
    Serial.println("");
    irrecv.resume();
  }
  delay(100);
}

void send_code(uint32_t code){  
  irsend.begin();                       
  Serial.print("code to send : ");
  Serial.print(code, BIN);
  Serial.print(" : ");
  Serial.println(code, HEX);
  irsend.sendLG(code, 32); // maybe 28
  irsend.sendNEC(code, 32);
  irsend.sendRC5(code, 32);
  irsend.sendRC6(code, 32);
  irsend.sendSony(code, 32);
  irsend.sendPanasonic(code, 32);
  irsend.sendSAMSUNG(code , 32);
}

BLYNK_WRITE(V2){ // liga o receiver de ir para fins de debug
  if (param.asInt() == 0){
    receive_on = true;
    Serial.print("IRrecv is now running and waiting for IR message on Pin: ");
    Serial.println(IR_PIN_RECEIVER);
  } else{
    receive_on = false;
    Serial.println("IRrecv is now off. ");
  }
}

BLYNK_WRITE(V3){ // ligar desligar ar e tv LG
  if(param.asInt() == 0){
    code_to_send = 0x20DF10EF; //ligar Tv da Lg
    send_code(code_to_send);
    code_to_send = 0x880084C; //ligar ar da LG
    send_code(code_to_send);
    Serial.println("Turning ON");
  }else{
    code_to_send = 0x88C0051; //desligar ar Lg
    send_code(code_to_send);
    code_to_send = 0x20DF10EF;
    send_code(code_to_send);
    Serial.println("Turning OFF");
  }
}

BLYNK_WRITE(V4){ // funcao treinar
  Serial.print("IRrecv is now running the trainer on pin: ");
  Serial.println(IR_PIN_RECEIVER);
  handler_trainer();
  Serial.print("trained code: ");
  Serial.println(trained_code, HEX);
}

BLYNK_WRITE(V5){ //funcao treinada
  send_code(trained_code);
}

BLYNK_WRITE(V6){ // temperatura ar lg
  pinvalue = param.asInt();
  code_to_send = temp_air[pinvalue-18];
  send_code(code_to_send);
}

BLYNK_WRITE(V7){ // controle de volume da Tv Lg
  if (param.asInt() == 0){
    code_to_send = 0x20DF40BF;
    send_code(code_to_send);
  }else{
    code_to_send = 0x20DFC03F;
    send_code(code_to_send);
  }
}

void setup()
{
  Serial.begin(115200);
  irrecv.enableIRIn();
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  handler_receiver();
  Blynk.run();
}
