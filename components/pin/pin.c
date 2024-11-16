#include <stdio.h>
#include "soc/reg_base.h" // DR_REG_GPIO_BASE, DR_REG_IO_MUX_BASE
#include "driver/rtc_io.h" // rtc_gpio_*
#include "pin.h"

// TODO: GPIO Matrix Registers - GPIO_OUT_REG, GPIO_OUT_W1TS_REG, ...
// NOTE: Remember to enclose the macro values in parenthesis, as below
#define GPIO_BT_SELECT_REG          (DR_REG_GPIO_BASE + 0x0000)
#define GPIO_OUT_REG          (DR_REG_GPIO_BASE + 0x0004)
#define GPIO_OUT_W1TS_REG          (DR_REG_GPIO_BASE + 0x0008)
#define GPIO_OUT_W1TC_REG          (DR_REG_GPIO_BASE + 0x000c)
#define GPIO_OUT1_REG          (DR_REG_GPIO_BASE + 0x0010)
#define GPIO_OUT1_W1TS_REG          (DR_REG_GPIO_BASE + 0x0014)
#define GPIO_OUT1_W1TC_REG          (DR_REG_GPIO_BASE + 0x0018)
#define GPIO_SDIO_SELECT_REG          (DR_REG_GPIO_BASE + 0x001c)
#define GPIO_ENABLE_REG          (DR_REG_GPIO_BASE + 0x0020)
#define GPIO_ENABLE_W1TS_REG          (DR_REG_GPIO_BASE + 0x0024)
#define GPIO_ENABLE_W1TC_REG          (DR_REG_GPIO_BASE + 0x0028)
#define GPIO_ENABLE1_REG          (DR_REG_GPIO_BASE + 0x002c)
#define GPIO_ENABLE1_W1TS_REG          (DR_REG_GPIO_BASE + 0x0030)
#define GPIO_ENABLE1_W1TC_REG          (DR_REG_GPIO_BASE + 0x0034)
#define GPIO_STRAP_REG          (DR_REG_GPIO_BASE + 0x0038)
#define GPIO_IN_REG          (DR_REG_GPIO_BASE + 0x003c)
#define GPIO_IN1_REG          (DR_REG_GPIO_BASE + 0x0040)
#define GPIO_STATUS_REG          (DR_REG_GPIO_BASE + 0x0044)
#define GPIO_STATUS_W1TS_REG          (DR_REG_GPIO_BASE + 0x0048)
#define GPIO_STATUS_W1TC_REG          (DR_REG_GPIO_BASE + 0x004c)
#define GPIO_STATUS1_REG          (DR_REG_GPIO_BASE + 0x0050)
#define GPIO_STATUS1_W1TS_REG          (DR_REG_GPIO_BASE + 0x0054)
#define GPIO_STATUS1_W1TC_REG          (DR_REG_GPIO_BASE + 0x0058)
#define GPIO_ACPU_INT_REG          (DR_REG_GPIO_BASE + 0x0060)
#define GPIO_ACPU_NMI_INT_REG          (DR_REG_GPIO_BASE + 0x0064)
#define GPIO_PCPU_INT_REG          (DR_REG_GPIO_BASE + 0x0068)
#define GPIO_PCPU_NMI_INT_REG          (DR_REG_GPIO_BASE + 0x006c)
#define GPIO_CPUSDIO_INT_REG          (DR_REG_GPIO_BASE + 0x0070)
#define GPIO_ACPU_INT1_REG          (DR_REG_GPIO_BASE + 0x0074)
#define GPIO_ACPU_NMI_INT1_REG          (DR_REG_GPIO_BASE + 0x0078)
#define GPIO_PCPU_INT1_REG          (DR_REG_GPIO_BASE + 0x007c)
#define GPIO_PCPU_NMI_INT1_REG          (DR_REG_GPIO_BASE + 0x0080)
#define GPIO_CPUSDIO_INT1_REG          (DR_REG_GPIO_BASE + 0x0084)

#define GPIO_cali_conf_REG          (DR_REG_GPIO_BASE + 0x0128)
#define GPIO_cali_data_REG          (DR_REG_GPIO_BASE + 0x012c)
#define GPIO_FUNC0_IN_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0130)
#define GPIO_FUNC0_OUT_SEL_CFG_REG          (DR_REG_GPIO_BASE + 0x0530)

// TODO: IO MUX Registers
// HINT: Add DR_REG_IO_MUX_BASE with PIN_MUX_REG_OFFSET[n]
#define IO_MUX_REG(n) (DR_REG_IO_MUX_BASE + PIN_MUX_REG_OFFSET[n])


#define REG(r) (*(volatile uint32_t *)(r))
#define REG_BITS 32
// TODO: Finish these macros. HINT: Use the REG() macro.
#define REG_SET_BIT(r,b) (REG(r) |= ((1) << (b)))
#define REG_CLR_BIT(r,b) (REG(r) &= ~((1) << (b)))
#define REG_GET_BIT(r,b) ((REG(r) & (0x1 << (b))) != 0)

#define GPIO_REG_OFFSET 4

#define GPIO_PIN_REG_BASE DR_REG_GPIO_BASE + 0x88

#define GPIO_PIN_REG(pin) (GPIO_PIN_REG_BASE + GPIO_REG_OFFSET*pin)
#define IO_FUNC_REG(pin) (GPIO_FUNC0_OUT_SEL_CFG_REG + pin*GPIO_REG_OFFSET)
#define MUX_RESET_WRITE 0x2900


#define OUT_SEL_SETUP_VAL 0x100

#define MCU_SEL_TOP 14
#define MCU_SEL_BOT 12
#define FUN_DRV_TOP 11
#define FUN_DRV_BOT 10
#define FUN_IE 9
#define FUN_WPU 8
#define FUN_WPD 7


#define PAD_DRIVER 2

#define SECOND_REG_PIN_CUTOFF 31
// Gives byte offset of IO_MUX Configuration Register
// from base address DR_REG_IO_MUX_BASE
static const uint8_t PIN_MUX_REG_OFFSET[] = {
    0x44, 0x88, 0x40, 0x84, 0x48, 0x6c, 0x60, 0x64, // pin  0- 7
    0x68, 0x54, 0x58, 0x5c, 0x34, 0x38, 0x30, 0x3c, // pin  8-15
    0x4c, 0x50, 0x70, 0x74, 0x78, 0x7c, 0x80, 0x8c, // pin 16-23
    0x90, 0x24, 0x28, 0x2c, 0xFF, 0xFF, 0xFF, 0xFF, // pin 24-31
    0x1c, 0x20, 0x14, 0x18, 0x04, 0x08, 0x0c, 0x10, // pin 32-39
};

// Now that the pin is reset, set the output level to zero
// Reset the configuration of a pin to not be an input or an output.
// Pull-up is enabled so the pin does not float.
int32_t pin_reset(pin_num_t pin)
{
	if (rtc_gpio_is_valid_gpio(pin)) { // hand-off work to RTC subsystem
		rtc_gpio_deinit(pin);
		rtc_gpio_pullup_en(pin);
		rtc_gpio_pulldown_dis(pin);
	}
	// TODO: Reset GPIO_PINn_REG: All fields zero
	REG(GPIO_PIN_REG(pin)) = 0x00;

	// TODO: Reset GPIO_FUNCn_OUT_SEL_CFG_REG: GPIO_FUNCn_OUT_SEL=0x100
	REG(IO_FUNC_REG(pin)) = OUT_SEL_SETUP_VAL;

	// TODO: Reset IO_MUX_x_REG: MCU_SEL=2, FUN_DRV=2, FUN_WPU=1
	REG(IO_MUX_REG(pin)) = MUX_RESET_WRITE;

	// Now that the pin is reset, set the output level to zero
	return pin_set_level(pin, 0);
}

int32_t pin_pullup(pin_num_t pin, bool enable)
{
	if (rtc_gpio_is_valid_gpio(pin)) { // hand-off work to RTC subsystem
		if (enable) return rtc_gpio_pullup_en(pin);
		else return rtc_gpio_pullup_dis(pin);
	}
	// TODO: Set or clear the FUN_WPU bit in an IO_MUX register
	if (enable) {
		REG_SET_BIT(IO_MUX_REG(pin), FUN_WPU);
	} else {
		REG_CLR_BIT(IO_MUX_REG(pin), FUN_WPU);
	}
	return 0;
}

// Enable or disable a pull-down on the pin.
int32_t pin_pulldown(pin_num_t pin, bool enable)
{
	if (rtc_gpio_is_valid_gpio(pin)) { // hand-off work to RTC subsystem
		if (enable) return rtc_gpio_pulldown_en(pin);
		else return rtc_gpio_pulldown_dis(pin);
	}
	// TODO: Set or clear the FUN_WPD bit in an IO_MUX register
	if (enable) {
		REG_SET_BIT(IO_MUX_REG(pin), FUN_WPD);
	} else {
		REG_CLR_BIT(IO_MUX_REG(pin), FUN_WPD);
	}
	return 0;
}

// Enable or disable the pin as an input signal.
int32_t pin_input(pin_num_t pin, bool enable)
{
	// TODO: Set or clear the FUN_IE bit in an IO_MUX register
	if (enable) {
		REG_SET_BIT(IO_MUX_REG(pin), FUN_IE);
	} else {
		REG_CLR_BIT(IO_MUX_REG(pin), FUN_IE);
	}
	return 0;
}

// Enable or disable the pin as an output signal.
int32_t pin_output(pin_num_t pin, bool enable)
{
	// TODO: Set or clear the I/O pin bit in the ENABLE or ENABLE1 register
	if (pin > SECOND_REG_PIN_CUTOFF) {
		pin -= (SECOND_REG_PIN_CUTOFF+1);
		if (enable) {
			REG_SET_BIT(GPIO_ENABLE1_W1TS_REG, pin);
		} else {
			REG_SET_BIT(GPIO_ENABLE1_W1TC_REG, pin);
		}
		//we use ENABLE1 regs
	} else {
		//we use ENABLE regs
		if (enable) {
			REG_SET_BIT(GPIO_ENABLE_W1TS_REG, pin);
		} else {
			REG_SET_BIT(GPIO_ENABLE_W1TC_REG, pin);
		}
	}
	return 0;
}

// Enable or disable the pin as an open-drain signal.
int32_t pin_odrain(pin_num_t pin, bool enable)
{
	// TODO: Set or clear the PAD_DRIVER bit in a PIN register
	if (enable) {
		REG_SET_BIT(GPIO_PIN_REG(pin), PAD_DRIVER);
	} else {
		REG_CLR_BIT(GPIO_PIN_REG(pin), PAD_DRIVER);
	}
	return 0;
}

// Sets the output signal level if the pin is configured as an output.
int32_t pin_set_level(pin_num_t pin, int32_t level)
{
		if (pin > SECOND_REG_PIN_CUTOFF) {
		pin -= SECOND_REG_PIN_CUTOFF;
		if (level) {
			REG_SET_BIT(GPIO_OUT1_W1TS_REG, pin);
		} else {
			REG_SET_BIT(GPIO_OUT1_W1TC_REG, pin);
		}
		//we use ENABLE1 regs
	} else {
		//we use ENABLE regs
		if (level) {
			REG_SET_BIT(GPIO_OUT_W1TS_REG, pin);
		} else {
			REG_SET_BIT(GPIO_OUT_W1TC_REG, pin);
		}
	}
	return 0;
}

// Gets the input signal level if the pin is configured as an input.
int32_t pin_get_level(pin_num_t pin)
{
	// TODO: Get the I/O pin bit from the IN or IN1 register
	if (pin > SECOND_REG_PIN_CUTOFF) {
		pin -= (SECOND_REG_PIN_CUTOFF+1);
		return REG_GET_BIT(GPIO_IN1_REG,pin);
	} else {
		return REG_GET_BIT(GPIO_IN_REG,pin);
	}

}

// Get the value of the input registers, one pin per bit.
// The two 32-bit input registers are concatenated into a uint64_t.
uint64_t pin_get_in_reg(void)
{
	uint64_t ret_val = ((uint64_t) REG(GPIO_IN1_REG) << (SECOND_REG_PIN_CUTOFF+1)) | REG(GPIO_IN_REG);
	//printf("Buttons: %b\n", ret_val);
	return ret_val;
}

// Get the value of the output registers, one pin per bit.
// The two 32-bit output registers are concatenated into a uint64_t.
uint64_t pin_get_out_reg(void)
{
	uint64_t ret_val = ((uint64_t) REG(GPIO_OUT1_REG) << (SECOND_REG_PIN_CUTOFF+1)) | REG(GPIO_OUT_REG);
	return ret_val;
}
