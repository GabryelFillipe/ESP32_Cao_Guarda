#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// Credenciais da rede
const char* ssid = "Galaxy A117882";
const char* password = "ueqm5463";

// URL de teste para o "Health Check"
const char* serverName = "https://jsonplaceholder.typicode.com/todos/1";

// Definição dos pinos dos LEDs
const int ledVerde = 18;
const int ledVermelho = 19;

void setup() {
  Serial.begin(115200);

  // Configuração dos pinos dos LEDs como saída
  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

  // Estado inicial: vermelho aceso até que a ligação seja bem-sucedida
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledVermelho, HIGH);

  WiFi.begin(ssid, password);
  Serial.print("A ligar ao Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nLigação Wi-Fi estabelecida com sucesso!");
  // Apaga o LED vermelho assim que o Wi-Fi liga (aguardará a verificação HTTP para acender o verde)
  digitalWrite(ledVermelho, LOW); 
}

void loop() {
  // Verifica se o Wi-Fi está ligado
  if (WiFi.status() == WL_CONNECTED) {
    
    // Cria o objeto cliente seguro
    WiFiClientSecure* client = new WiFiClientSecure;
    
    if (client) {
      // Ignora a validação do certificado para facilitar o teste
      client->setInsecure();

      HTTPClient https;
      Serial.print("[HTTPS] A iniciar o pedido de verificação...\n");
      
      if (https.begin(*client, serverName)) {
        // Realiza o GET
        int httpCode = https.GET();
        
        if (httpCode > 0) {
          Serial.printf("[HTTPS] Código de resposta: %d\n", httpCode);
          
          if (httpCode == HTTP_CODE_OK) { // HTTP 200 (Sucesso)
            String payload = https.getString();
            Serial.println("Servidor Online. Conteúdo recebido:");
            Serial.println(payload);
            
            // Sucesso: Acende Verde, Apaga Vermelho
            digitalWrite(ledVerde, HIGH);
            digitalWrite(ledVermelho, LOW);
            
          } else {
            // Servidor respondeu, mas com erro (ex: 404, 500)
            Serial.println("Erro no servidor!");
            
            // Erro: Acende Vermelho, Apaga Verde
            digitalWrite(ledVerde, LOW);
            digitalWrite(ledVermelho, HIGH);
          }
        } else {
          Serial.printf("[HTTPS] Falha no pedido, erro: %s\n", https.errorToString(httpCode).c_str());
          
          // Falha na requisição: Acende Vermelho, Apaga Verde
          digitalWrite(ledVerde, LOW);
          digitalWrite(ledVermelho, HIGH);
        }

        https.end();
      } else {
        Serial.printf("[HTTPS] Não foi possível ligar ao servidor.\n");
        
        // Falha de ligação ao servidor: Acende Vermelho, Apaga Verde
        digitalWrite(ledVerde, LOW);
        digitalWrite(ledVermelho, HIGH);
      }

      // Liberta a memória para evitar memory leak (travamento)
      delete client; 
    }
  } else {
    // Wi-Fi caiu
    Serial.println("Ligação Wi-Fi perdida! A tentar restabelecer...");
    
    // Queda de Wi-Fi: Acende Vermelho, Apaga Verde
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledVermelho, HIGH);
    
    // Tenta reconectar o Wi-Fi
    WiFi.reconnect();
  }

  // Aguarda 15 segundos para a próxima verificação, conforme exigido no desafio
  delay(15000);
}
