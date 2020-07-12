char* msg = "";
bool intr = false;
uint8_t data = 0x0;

void SERCOM1_0_Handler() {
  msg = "SPI Data Register Empty Interrupt";
  SERCOM1->SPI.DATA.reg = 0xAA;
  intr = true;
}

void SERCOM1_1_Handler() {
  msg = "SPI Data Transmit Complete";
  SERCOM1->SPI.INTFLAG.bit.TXC = 1; //clear receive complete interrupt
  intr = true;
}

void SERCOM1_2_Handler() {
  msg = "SPI Data Received Complete";
  data = SERCOM1->SPI.DATA.reg; //Read data register
//  Serial.print("DATA: "); Serial.println(data);
  SERCOM1->SPI.INTFLAG.bit.RXC = 1; //clear receive complete interrupt
  intr = true;
}

void SERCOM1_3_Handler() {
  msg = "SSL Interupt";
  SERCOM1->SPI.INTFLAG.bit.SSL = 1; //clear slave select interrupt
  intr = true;
}

void initSPI() {
  
  // disable SPI so we can configure it
  SERCOM1->SPI.CTRLA.bit.ENABLE = 0; 
  while (SERCOM1->SPI.SYNCBUSY.bit.ENABLE);

  //Setting up NVIC
  NVIC_EnableIRQ(SERCOM1_0_IRQn);
  NVIC_SetPriority(SERCOM1_0_IRQn,2);
  NVIC_EnableIRQ(SERCOM1_1_IRQn);
  NVIC_SetPriority(SERCOM1_1_IRQn,2);
  NVIC_EnableIRQ(SERCOM1_2_IRQn);
  NVIC_SetPriority(SERCOM1_2_IRQn,2);
  NVIC_EnableIRQ(SERCOM1_3_IRQn);
  NVIC_SetPriority(SERCOM1_3_IRQn,2);

  // Set up peripheral clock
  GCLK->GENCTRL[0].reg = GCLK_GENCTRL_DIV(0) |       // Divide the 48MHz clock source by divisor 1: 48MHz/1 = 48MHz
//                         GCLK_GENCTRL_IDC |          // Set the duty cycle to 50/50 HIGH/LOW
                         GCLK_GENCTRL_GENEN |
                         GCLK_GENCTRL_SRC_GCLKIN;
//  while (GCLK->SYNCBUSY.bit.GENCTRL0);               // Wait for synchronization
//  GCLK->GENCTRL[0].bit.SRC = 0x02; //GCLK_IN
  GCLK->PCHCTRL[GCM_SERCOM1_CORE].bit.GEN = 0;
  GCLK->PCHCTRL[GCM_SERCOM1_CORE].bit.CHEN = 0x1;
  while (GCLK->PCHCTRL[GCM_SERCOM1_CORE].bit.CHEN != 0x1); // Wait for synchronization

  MCLK->APBAMASK.bit.SERCOM1_ = 1;
  
  // Set PA16 as SERCOM1:0
  PORT->Group[PORTA].PINCFG[16].bit.PMUXEN = 0x1; 
  PORT->Group[PORTA].PMUX[16 >> 1].bit.PMUXE = 0x2;
  
  // Set PA17 as SERCOM1:1
  PORT->Group[PORTA].PINCFG[17].bit.PMUXEN = 0x1; 
  PORT->Group[PORTA].PMUX[17 >> 1].bit.PMUXO = 0x2;

  // Set PA18 as SERCOM1:2
  PORT->Group[PORTA].PINCFG[18].bit.PMUXEN = 0x1; 
  PORT->Group[PORTA].PMUX[18 >> 1].bit.PMUXE = 0x2;

  // Set PA19 as SERCOM1:3
  PORT->Group[PORTA].PINCFG[19].bit.PMUXEN = 0x1; 
  PORT->Group[PORTA].PMUX[19 >> 1].bit.PMUXO = 0x2;

  SERCOM1->SPI.CTRLA.bit.DORD = 0; //MSB first
  SERCOM1->SPI.CTRLA.bit.CPOL = 0; //SCK is low when idle, leading edge is rising edge
  SERCOM1->SPI.CTRLA.bit.CPHA = 0; //data sampled on leading sck edge and changed on a trailing sck edge
  SERCOM1->SPI.CTRLA.bit.FORM = 0x0; //Frame format = SPI

  SERCOM1->SPI.CTRLA.bit.MODE = 0x2; // set SPI Operating Mode to slave
  SERCOM1->SPI.CTRLA.bit.DIPO = 0; //DATA PAD0 MOSI is used as input (slave mode)
  SERCOM1->SPI.CTRLA.bit.DOPO = 0x2; //DATA PAD3 MISO is used as output

  SERCOM1->SPI.CTRLB.bit.SSDE = 0x1; //Slave Select Detection Enabled
  SERCOM1->SPI.CTRLB.bit.CHSIZE = 0; //character size 8 Bit
  SERCOM1->SPI.CTRLA.bit.IBON = 0x1; //Buffer Overflow notification
  SERCOM1->SPI.CTRLA.bit.RUNSTDBY = 1; //wake on receiver complete

  //Set up SPI interrupts
  SERCOM1->SPI.INTENSET.bit.SSL = 0x1; //Enable Slave Select low interrupt        
  SERCOM1->SPI.INTENSET.bit.RXC = 0x1; //Receive complete interrupt
  SERCOM1->SPI.INTENSET.bit.TXC = 0x1; //Receive complete interrupt
  SERCOM1->SPI.INTENSET.bit.ERROR = 0x1; //Receive complete interrupt
  SERCOM1->SPI.INTENSET.bit.DRE = 0x1; //Data Register Empty interrupt

  //Enable SPI
  SERCOM1->SPI.CTRLA.bit.ENABLE = 1;
  while (SERCOM1->SPI.SYNCBUSY.bit.ENABLE);
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  initSPI();
  Serial.println("Started");
}


void loop() {
  if (intr) {
    Serial.println(msg);
    intr = false;
  }
//  Serial.println(data);
}
