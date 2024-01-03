#include <stm32f401xc.h>

#define FLASH_CR_PSIZE_x8	(0x00 << FLASH_CR_PSIZE_Pos)
#define FLASH_CR_PSIZE_x16	(0x01 << FLASH_CR_PSIZE_Pos)
#define FLASH_CR_PSIZE_x32	(0x02 << FLASH_CR_PSIZE_Pos)
#define FLASH_CR_PSIZE_x64	(0x03 << FLASH_CR_PSIZE_Pos)
#define LAST_SECTOR_NUM		5

class Flash {

	public:
		void set_latency(uint32_t latency);
		void unlock();
		void lock();
		void write_data(uint32_t address, uint32_t *data, uint16_t size);
		void read_data(uint32_t address, uint32_t *buffer, uint16_t size);
		void enable_data_cache();
		void disable_data_cache();
		void enable_instruction_cache();
		void disable_instruction_cache();
		void enable_prefetch();
		void disable_prefetch();
		void set_parallelism(uint32_t parallelism);
		void sector_erase(uint8_t sector);
		void mass_erase();

	private:
		inline void set_programming_bit();
};

#ifdef __cplusplus
extern "C" {
#endif                                      
	void FLASH_SET_LATENCY(uint32_t latency);
#ifdef __cplusplus
}
#endif                                      