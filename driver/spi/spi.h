#include "txm_module.h"
#include "qapi_timer.h"
#include "qapi_fs_types.h"
#include "qapi_fs.h"

void spi_init(void);
uint8 * spi_reg_read(uint8 reg);
qapi_Status_t spi_reg_write(uint8 reg, uint8 data);
uint8 * spi_read_len(uint8 reg, uint8 len);
qapi_Status_t spi_write_len(uint8 reg, uint8 *buf, uint8 len);
uint32 spi_check_transfer_status(void);