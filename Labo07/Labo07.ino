#include <SPI.h>
#include <Wire.h>

const int MI_ID = 5;
const int DESTINO_ID = 6;

String mensajeOriginal = "PON TU MENSAJE AQUI";

// ======================================================
// PINES
// ======================================================

// definir los pines del módulo LoRa con #define
//       según la tabla de conexiones.
#define SCK 18   
#define MISO 19  
#define MOSI 23  
#define NSS 5    

// definir el pin del sensor Touch
//       y el umbral. Recuerde comprobar el
//       valor de su ESP32 antes de fijar el umbral.
#define TOUCH_PIN 27 
//Este es el umbral
#define TOUCH_THRESHOLD 800

bool toqueActivo = false;
// ======================================================
// ATBASH
// ======================================================

char convertirAtbash(char letra) {

  if (letra >= 'A' && letra <= 'Z') {
    return 'Z' - (letra - 'A');
  }

  if (letra >= 'a' && letra <= 'z') {
    return 'z' - (letra - 'a');
  }

  return letra;
}

String cifrarAtbash(String mensaje) {

  String mensajeCifrado = "";

  for (int i = 0; i < mensaje.length(); i++) {
    mensajeCifrado += convertirAtbash(mensaje.charAt(i));
  }

  return mensajeCifrado;
}

// ======================================================
// ENVIAR MENSAJE
// ======================================================

void enviarMensaje() {

  String mensajeCifrado = cifrarAtbash(mensajeOriginal);

  Serial.println();
  Serial.println("TOUCH DETECTADO");

  Serial.print("Mensaje original: ");
  Serial.println(mensajeOriginal);

  Serial.print("Mensaje cifrado: ");
  Serial.println(mensajeCifrado);

  // Formato del paquete: ORIGEN|DESTINO|MENSAJE
  LoRa.beginPacket();
  LoRa.print(MI_ID);
  LoRa.print("|");
  LoRa.print(DESTINO_ID);
  LoRa.print("|");
  LoRa.print(mensajeCifrado);
  LoRa.endPacket();

  Serial.print("Paquete enviado: ");
  Serial.print(MI_ID);
  Serial.print("|");
  Serial.print(DESTINO_ID);
  Serial.print("|");
  Serial.println(mensajeCifrado);

  Serial.println("----------------------------");
}

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("TRANSMISOR LORA");

  Serial.print("Mi ID: ");
  Serial.println(MI_ID);

  Serial.print("Destino: ");
  Serial.println(DESTINO_ID);

  // iniciar el bus SPI con SPI.begin(...) usando los pines definidos
  SPI.begin(SCK, MISO, MOSI, NSS);
  // indicar a la librería los pines del módulo con LoRa.setPins(...)
  //       (NSS, RESET = -1, DIO0 = -1)
  LoRa.setPins(NSS, -1, -1);

  if (!LoRa.begin(433E6)) {
    Serial.println("ERROR: No se pudo iniciar LoRa");
    while (true);
  }

  Serial.println("LoRa iniciado correctamente");
  Serial.println("Esperando toque...");
}

// ======================================================
// LOOP
// ======================================================

void loop() {

  // leer el sensor Touch
  int touchValue = touchRead(TOUCH_PIN);
  // detectar el toque (valor menor al umbral) y evitar que se
  //       envien varios paquetes mientras el dedo sigue presionando
  //       (use una variable de estado)
  // cuando se detecte un toque nuevo, llamar a enviarMensaje()
  // detectar el toque (valor menor al umbral) 
  if (touchValue < TOUCH_THRESHOLD) {
    if (!toqueActivo) {
      toqueActivo = true;
      enviarMensaje();
    }
  } else {
    toqueActivo = false;
  }

  delay(100);
}
}