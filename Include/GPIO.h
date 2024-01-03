#ifndef 	GPIO_H
#define 	GPIO_H

#include <RCC.h>

/**********GPIO MODE Values***********/
#define GPIO_PIN_INPUT_MODE 		((uint32_t) 0x00)
#define GPIO_PIN_OUTPUT_MODE 		((uint32_t) 0x01)
#define GPIO_PIN_ALT_FUN_MODE 		((uint32_t) 0x02)
#define GPIO_PIN_Analog_MODE 		((uint32_t) 0x03)

/****************GPIO OP Type Selection values*************/
#define GPIO_PIN_OP_TYPE_PUSH_PULL	((uint32_t) 0x0)
#define GPIO_PIN_OP_TYPE_OPEN_DRAIN	((uint32_t) 0x1)

/******************GPIO OSPEED Selection values***************/
#define GPIO_PIN_OSPEED_LOW 		((uint32_t) 0x00)
#define GPIO_PIN_OSPEED_MEDIUM		((uint32_t) 0x01)
#define GPIO_PIN_OSPEED_HIGH 		((uint32_t) 0x02)
#define GPIO_PIN_OSPEED_VERY_HIGH	((uint32_t) 0x03)

/******************GPIO PUPD Selection values***************/
#define GPIO_PIN_PUPD_NO_PUPD 		((uint32_t) 0x00)
#define GPIO_PIN_PUPD_PULL_UP		((uint32_t) 0x01)
#define GPIO_PIN_PUPD_PULL_DOWN		((uint32_t) 0x02)

/***********************GPIOA PINS*********************/
#define PA0							((uint8_t) 	0)
#define PA1							((uint8_t) 	1)
#define PA2							((uint8_t) 	2)
#define PA3							((uint8_t) 	3)
#define PA4							((uint8_t) 	4)
#define PA5							((uint8_t) 	5)
#define PA6							((uint8_t) 	6)
#define PA7							((uint8_t) 	7)
#define PA8							((uint8_t) 	8)
#define PA9							((uint8_t) 	9)
#define PA10						((uint8_t) 	10)
#define PA11						((uint8_t) 	11)
#define PA12						((uint8_t) 	12)
#define PA13						((uint8_t) 	13)
#define PA14						((uint8_t) 	14)
#define PA15						((uint8_t) 	15)

/***********************GPIOB PINS*********************/
#define PB0							((uint8_t) 	0)
#define PB1							((uint8_t) 	1)
#define PB2							((uint8_t) 	2)
#define PB3							((uint8_t) 	3)
#define PB4							((uint8_t) 	4)
#define PB5							((uint8_t) 	5)
#define PB6							((uint8_t) 	6)
#define PB7							((uint8_t) 	7)
#define PB8							((uint8_t) 	8)
#define PB9							((uint8_t) 	9)
#define PB10						((uint8_t) 	10)
#define PB11						((uint8_t) 	11)
#define PB12						((uint8_t) 	12)
#define PB13						((uint8_t) 	13)
#define PB14						((uint8_t) 	14)
#define PB15						((uint8_t) 	15)

/***********************GPIOC PINS*********************/
#define PC0							((uint8_t) 	0)
#define PC1							((uint8_t) 	1)
#define PC2							((uint8_t) 	2)
#define PC3							((uint8_t) 	3)
#define PC4							((uint8_t) 	4)
#define PC5							((uint8_t) 	5)
#define PC6							((uint8_t) 	6)
#define PC7							((uint8_t) 	7)
#define PC8							((uint8_t) 	8)
#define PC9							((uint8_t) 	9)
#define PC10						((uint8_t) 	10)
#define PC11						((uint8_t) 	11)
#define PC12						((uint8_t) 	12)
#define PC13						((uint8_t) 	13)
#define PC14						((uint8_t) 	14)
#define PC15						((uint8_t) 	15)



/**************************NUCLEO_BOARD PINS**************************/
#define A0							PA0
#define A1							PA1
#define A2							PA3
#define A3							PA4
#define A4							PA5
#define A5							PA6
#define A6							PA7
#define A7							PA2
#define D0							PA10
#define D1							PA9
#define D2							PA12
#define D3							PB0
#define D4							PB7
#define D5							PB6
#define D6							PB1
#define D7							PC14
#define D8							PC15
#define D9							PA8
#define D10							PA11
#define D11							PB5
#define D12							PB4
#define D13							PB3





#define GPIO_OUTPUT_HIGH			((uint32_t) 0x1)
#define GPIO_OUTPUT_LOW				((uint32_t) 0x0)
typedef struct {
	uint32_t pin;
	uint32_t mode;
	uint32_t op_type;
	uint32_t pupd;
	uint32_t speed;
	uint32_t alternate_function;
}gpio_pin_conf_t;


#ifdef __cplusplus
extern "C" {
#endif                                      
	void gpio_init(GPIO_TypeDef*, gpio_pin_conf_t*);
	uint8_t gpio_read_pin(GPIO_TypeDef*, uint8_t);
	void gpio_write_pin(GPIO_TypeDef*, uint8_t, uint8_t);
	void gpio_set_alt_func(GPIO_TypeDef*, uint8_t, uint8_t);
	void enable_clock(GPIO_TypeDef *);
#ifdef __cplusplus
}
#endif                                      

#endif