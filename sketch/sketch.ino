#include <Arduino.h>
#include <Arduino_RouterBridge.h>

volatile int counter = 0;
volatile int doubleValue = 0;

void updateCounter(int x) {

  counter = x;

  // on agrandit volontairement la fenêtre de condition de concurrence
  delay(5);

  doubleValue = x * 2;

  Monitor.print("RPC update: counter=");
  Monitor.print(counter);
  Monitor.print(" double=");
  Monitor.println(doubleValue);
}

void setup() {

  Bridge.begin();
  Monitor.begin();

  Bridge.provide("update", updateCounter);   // remplacer par provide_safe pour le test
}

void loop() {

  int c = counter;
  int d = doubleValue;

  if (d != c * 2) {
    Monitor.print("ERROR inconsistent state: counter=");
    Monitor.print(c);
    Monitor.print(" double=");
    Monitor.println(d);
  }

  delay(50);
}
