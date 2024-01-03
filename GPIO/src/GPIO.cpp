#include <GPIO.h>



uint8_t gpio_read_pin(GPIO_TypeDef *GPIOx, uint8_t pin){
	return ((GPIOx->IDR & (1 << pin)) ? 1 : 0);
}
void gpio_write_pin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t value){
	if(value) 	GPIOx->ODR |=  (1 << pin) ;
	else 		GPIOx->ODR &= ~(1 << pin);
}
void gpio_set_alt_func(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t alt_fun){
	uint8_t baseBit = (pin * 4) % 32;
	uint8_t reg = pin < 8 ? 0 : 1;

	GPIOx->AFR[reg] &= ~(0b1111 << baseBit);
	GPIOx->AFR[reg] |= (alt_fun << baseBit);
}
static void gpio_configure_pin_mode(GPIO_TypeDef *GPIOx, uint8_t pin, uint32_t mode){
	GPIOx->MODER &= ~(3 << (pin * 2));
	GPIOx->MODER |= (mode << (pin * 2));
}
static void gpio_configure_pin_otype(GPIO_TypeDef *GPIOx, uint8_t pin, uint32_t op_type){
	GPIOx->OTYPER &= ~(1 << pin);
	GPIOx->OTYPER |= (op_type << pin);
}
static void gpio_configure_pin_ospeed(GPIO_TypeDef *GPIOx, uint8_t pin, uint32_t ospeed){
	GPIOx->OSPEEDR &= ~(3 << (pin * 2));
	GPIOx->OSPEEDR |= (ospeed << (pin * 2));
}
static void gpio_configure_pin_pupd(GPIO_TypeDef *GPIOx, uint8_t pin, uint32_t pupd){
	GPIOx->PUPDR &= ~(3 << (pin * 2));

	if(pupd == GPIO_PIN_PUPD_PULL_UP || pupd == GPIO_PIN_PUPD_PULL_DOWN)
		GPIOx->PUPDR |= (pupd << (pin * 2));
	else
		GPIOx->PUPDR |= (GPIO_PIN_PUPD_NO_PUPD << (pin * 2));
}

void gpio_init(GPIO_TypeDef* GPIOx, gpio_pin_conf_t* pin_conf){
	//first configure the mode
	gpio_configure_pin_mode(GPIOx, pin_conf->pin, pin_conf->mode);
	//configure the output type
	gpio_configure_pin_otype(GPIOx, pin_conf->pin, pin_conf->op_type);
	//configure the ospeed
	gpio_configure_pin_pupd(GPIOx, pin_conf->pin, pin_conf->pupd);
	//configure the pull up pull down
	gpio_configure_pin_ospeed(GPIOx, pin_conf->pin, pin_conf->speed);
	//configure the alternate function
	if(pin_conf->alternate_function != -1)
		gpio_set_alt_func(GPIOx, pin_conf->pin, pin_conf->alternate_function);
}

void enable_clock(GPIO_TypeDef *GPIOx){
	if(GPIOx == GPIOA)
		RCC_GPIOA_CLK_ENABLE();
	else if(GPIOx == GPIOB)
		RCC_GPIOB_CLK_ENABLE();
	else if(GPIOx == GPIOC)
		RCC_GPIOC_CLK_ENABLE();
	else if(GPIOx == GPIOD)
		RCC_GPIOD_CLK_ENABLE();
	else if(GPIOx == GPIOE)
		RCC_GPIOE_CLK_ENABLE();
	else if(GPIOx == GPIOH)
		RCC_GPIOH_CLK_ENABLE();
}