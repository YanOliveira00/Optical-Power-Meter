//Código criado por Yan S. de Oliveira, em 2016.
//Código complementar feitos em Python.
//Corresponde ao TCC do curso de Engenharia da Computação UFPa.
//Editado e comentado em 17/03/2017 
//Definições iniciais
//Pinos do TLC5620 6,7,8 e 13, respectivamente.
#define PIN_DATA  6  
#define PIN_CLK   7  
#define PIN_LOAD  5  
#define PIN_LDAC  4  
#define PIN_MOD  3 //Trabalhos futuros 

//Portas que serão utilizadas para a placa e seus feedbacks
#define PIN_AMPFEED  A1
#define PIN_PDFEED A0
#define PIN_1X A5
#define PIN_100X A2
#define PIN_1000X A4
#define PIN_10X A3 

//Modos de escrita do TLC, vide datasheet.
#define DACA  0
#define DACB  1
#define DACC  2
#define DACD  3 

//Especificações para TLC, vide datasheet.
#define DAC_REF   0	// 1 * Vref
#define DAC_REFX2 1	// 2 * Vref
#define FREQUENCY 10 //Frequencia em uS 10 = 100kHz
float mapAMP = 0;//Valor de corrente real enviado pelo TLC
float corrente = 0;//Corrente lida do Serial
float VStep = 0; //Variação de tensão por step no TLC 
int limit = 0;//armazena o limitador recebido (1-3)
int steps = 0;//steps do TLC para envio de tensão ao ampop (0-255)
int Func =0;//Armazena função de leitura (1-6)

//variável abstrata para leitura dos pinos, após terem sido mapeados pela função map
float  leitura = 0; 
uint8_t TXT;

//union utilizado para converter um float em bytes e vice-versa
typedef union {
 short intPoint;
 byte binary[2];
} binaryInt; 
char ch;//Char que lerá o valor do limitador Ex.: '1'
char fim;//Recebe char que determina fim do ciclo solicitado pelo programa em Python 

void initTLC5620() {//inicia tlc
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLK,  OUTPUT);
  pinMode(PIN_LOAD, OUTPUT);
  pinMode(PIN_LDAC, OUTPUT); 
} //Ler limitadores, e identificar para os cálculos

void initLimit() {	//inicia limitador. Espera resposta. 
  while(limit<1 or limit>3) { //entre 1 e 3
	if (Serial.available()>0) { //Se houver byte no serial
      	ch=Serial.read();
      	limit = (int)ch - (int)'0'; //Inicia o limitador com o valor escolhido (1-3)   	
    	}  
  }
 }

//Ler Função
void initFunction() { //inicia limitador. Espera resposta.
  while(Func<1 or Func>6) { //entre 1 e 6
	if (Serial.available()>0) { //Se houver byte no serial
      	ch=Serial.read();
      	Func = (int)ch - (int)'0'; //Inicia o limitador com o valor escolhido (1-3)   	
    	}  
  }
  if(Func==1){
	TXT=PIN_AMPFEED;
  }
  if(Func==2){
	TXT=PIN_PDFEED;
  } 
  if(Func==3){
	TXT=PIN_1X;
  } 
  if(Func==4){
	TXT=PIN_10X;
  }  
  if(Func==5){
	TXT=PIN_100X;
  }
  if(Func==6){
	TXT=PIN_1000X;
  }
 }


 //escrita necessita de uma sincronia entre o Clock e a entrada
//de dados enviada por DATA. Com isso usamos a configuração MSB na subida do Clock
// e terminamos o dado usando o LSB no fim do pulso. Respeitando as instruções do datasheet do TLC5620
void writeTLC5620(int channel, int range, int data) { //formato padrão:
//writeTLC5620(DACA,DAC_REF,steps)
  int MSB;
  int LSB;
  MSB = (channel & 0x03) << 1 | (range & 0x01) << 0; //0000 0000(DACA) & 0000 0011 << 1 = 0000 0000 | 0000 0000(DAC_REF) & 0000 0001 = 0000 0000
  LSB = data;
  //datasheet tlc5620
  digitalWrite(PIN_LDAC, HIGH);
  digitalWrite(PIN_LOAD, HIGH);
  //organizando usando o shiftOut
  shiftOut(PIN_DATA, PIN_CLK, MSBFIRST, MSB);//highbyte
  shiftOut(PIN_DATA, PIN_CLK, MSBFIRST, LSB);//lowbyte 
  digitalWrite(PIN_LOAD, LOW);
  delayMicroseconds(FREQUENCY);//frequencia selecionada
  digitalWrite(PIN_LOAD, HIGH);
  delayMicroseconds(FREQUENCY);
  digitalWrite(PIN_LDAC, LOW);
  delayMicroseconds(FREQUENCY);
  digitalWrite(PIN_LDAC, HIGH);
  //pronto! Dado enviado para a porta escolhida
}

//Função para escrever_Serial
//Envia-se delimitadores de inicio e fim para então ser passado os 4 bytes do valor float, já calculado pelo Arduino
void send_int (short arg)
{ 
  binaryInt hi; //union hi criada.
  hi.intPoint = arg; //aponta para o valor que se quer enviar
  Serial.write('&');
  Serial.write(hi.binary,2);//envia o buffer com os bytes da union, representando o short "arg"
}

//mapear
  void mapear(int PIN) {
	leitura = analogRead(PIN);
	leitura *= (5.0 / 1023.0);
  } 

//PASSO 2:
//A função(def_current) irá receber o valor da corrente assinalada
//no Python e converter pra tensão necessária, considerando o
//limitador escolhido e o step necessário para fazer, de acordo com
//a corrente encontrada nos resistores.
//função def_steps recebe o float enviado pelo pyserial e transforma para steps viáveis ao TLC
void def_step(float i, float iMax) {
  if (i <= iMax) {
	steps = round((i * 255) / iMax);
  } else {
	steps = 255;
  }
} 
//determina corrente
void def_current() {
  corrente = -1.00;
  while (corrente <= 0) { //lê o char como float	
	if (Serial.available() > 0){
 	Serial.flush();//Espera o Serial para de enviar para então ler
  	corrente=Serial.parseFloat();	//Recebe uma sequencia e os transforma para float	
   }
  }
  //função que verifica limitadores
  limit=0;
  initLimit();
  Func=0;
  initFunction();  
  if (limit == 1) { //se limitador for 220 Ohm, sabemos que tensão máx é 0,595V. iMax=18.6 mA
	def_step(corrente, 18.25); //18.6 achado em calculo //calculará o número de steps que será escrito no TLC para atingir tal corrente
  }
  if (limit == 2) { //Corrente Máxima para esse limitador= 41.76mA
	def_step(corrente, 41.3); //41.76 achado em cálculo
  }
  if (limit == 3) { //Corrente Máxima para esse limitador = 70.9mA
	def_step(corrente, 70.45); //70.9 em cálculo
  } 
  //escrever no TLC com referencia setada em
  writeTLC5620(DACA, DAC_REF, steps);
 
}//fim função definir step 


//INÍCIO
void setup() {
  Serial.begin(250000);
  initTLC5620();//inicializa TLC
  writeTLC5620(DACA, DAC_REF, 0);//roda o vazio para evitar problemas de sobrecarga do laser
  def_current();//define os parametros de corrente. 
  pinMode(PIN_MOD, OUTPUT);  //inicializando MOD
  pinMode(PIN_PDFEED, INPUT);  //inicializando PDFEED
  pinMode(PIN_AMPFEED, INPUT);  //inicializando AMPFEED
  pinMode(PIN_1X, INPUT);  //inicializando 1X
  pinMode(PIN_10X, INPUT);  //inicializando 10X
  pinMode(PIN_100X, INPUT);  //inicializando 100X
  pinMode(PIN_1000X, INPUT);  //inicializando 1X 
}

//Considerando as funções que determinam passo 1 e 2, os passos seguintes se resumem a mostrar
//o que foi observado e armazenar em um file via pyserial.
void loop() {
  send_int(analogRead(TXT)); 
  //Ao fim da leitura, pySerial envia uma flag recomeçando o loop
  //logo, seta-se o TLC para 0 novamente e espera um novo valor de corrente e limitador
  if (Serial.available()>0){
	fim=Serial.read();
	if (fim=='9'){
    	writeTLC5620(DACA, DAC_REF, 0);
    	def_current();   	
  	}
	}
}
