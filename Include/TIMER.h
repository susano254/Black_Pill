#include <stm32f401xc.h>
#include <vector>

using namespace std;

#ifndef TIM_H
#define TIM_H

#define CHANNEL_IO_OUTPUT			(0x00)
#define CHANNEL_IO_INPUT_TI2		(0x01)
#define CHANNEL_IO_INPUT_TI1		(0x02)
#define CHANNEL_IO_INPUT_TRC		(0x03)

#define CHANNEL_POLARITY_HIGH		(0U)
#define CHANNEL_POLARITY_LOW		(1U)

#define TIM_DIR_DOWNCOUNT				(1U)
#define TIM_DIR_UPCOUNT					(0U)


typedef struct {
	uint32_t no;
	uint32_t io;
	uint32_t mode;
	uint32_t polarity;
	uint32_t capture_preload_enable;
	uint32_t capture_compare;
} channel_conf_t;

typedef struct {
	TIM_TypeDef *instance;
	uint32_t prescaler;
	uint32_t auto_reload;
	uint32_t preload;
	uint32_t direction;
} TIMx_init_t;

typedef enum {
	OCxM_FROZEN 			= 0b000,
	OCxM_SET_ACTIVE 		= 0b001,
	OCxM_SET_INACTIVE 		= 0b010,
	OCxM_TOGGLE				= 0b011,
	OCxM_FORCE_ACTIVE 		= 0b100,
	OCxM_FORCE_INACTIVE 	= 0b101,
	OCxM_PWM_1				= 0b110,
	OCxM_PWM_2				= 0b111,
} ch_mode_t;

class TIMER {
	private:

		void enable_channel(uint8_t ch_no);
		void set_polarity(uint8_t ch_no, uint32_t ch_polarity);
		void set_ch_io(uint8_t ch_no, uint32_t ch_io);
		void set_preaload_enable(uint8_t ch_no, uint32_t ch_PRE);
		void set_ch_mode(uint8_t ch_no, uint32_t ch_mode);

	public:
		TIMx_init_t init_struct;
		vector<channel_conf_t*> channels;
		void set_capture_compare(uint8_t ch_no, uint32_t ch_capture_compare);
		uint32_t get_capture_compare(channel_conf_t *ch);
		void init(TIMx_init_t *init_struct);
		void channel_init(channel_conf_t *ch);
		void update(){
			init_struct.instance->EGR |= TIM_EGR_UG;
		}
};

// #ifdef __cplusplus
// extern "C" {
// #endif                                      
// 	void TIMx_init(TIMx_init_t *init_struct);
// 	void channel_init(TIM_TypeDef *TIMx, channel_conf_t *ch);
// #ifdef __cplusplus
// }
// #endif                                      
namespace global {
	extern TIM_TypeDef *TIM2_Base;
};

#endif