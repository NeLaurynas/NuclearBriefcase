// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "mcp.h"

#include <stdio.h>
#include <stdint.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "defines/config.h"

// MCP23017 registers (Bank Mode 1)
#define IODIRA 0x00  // I/O Direction Register A
#define IODIRB 0x10  // I/O Direction Register B
#define GPIOA  0x09  // GPIO Register A
#define GPIOB  0x19  // GPIO Register B

#define IOCONA 0x0A  //IO Configuration Register A - BANK/MIRROR/SLEW/INTPOL
#define IOCONB 0x0B  //IO Configuration Register B - BANK/MIRROR/SLEW/INTPOL
#define IOCON_BANK_BIT 7
#define IOCON_MIRROR_BIT 6
#define IOCON_SEQOP_BIT 5
#define IOCON_DISSLW_BIT 4
#define IOCON_HAEN_BIT 3
#define IOCON_ODR_BIT 2
#define IOCON_INTPOL_BIT 1

void write_register(uint8_t address, uint8_t regist, uint8_t value) {
	const uint8_t data[2] = {regist, value};
	i2c_write_blocking(MOD_MCP_I2C_PORT, address, data, 2, false);
}

uint8_t read_register(uint8_t address, uint8_t regist) {
	uint8_t value;
	i2c_write_blocking(MOD_MCP_I2C_PORT, address, &regist, 1, true);
	i2c_read_blocking(MOD_MCP_I2C_PORT, address, &value, 1, false);
	return value;
}

inline void set_bit(uint8_t* value, uint8_t bit, bool set) {
	if (bit >= 0 && bit <= 15) {
		if (set) { *value |= (1 << bit); }
		else { *value &= ~(1 << bit); }
	}
}

inline bool is_bit_set(uint8_t value, uint8_t bit) {
	if (bit >= 0 && bit <= 15) { return (bool)(0x1 & (value >> bit)); }
	return false;
}

void setup_bank_configuration(uint8_t address, uint8_t regist) {
	uint8_t ioconData = 0;
	set_bit(&ioconData, IOCON_BANK_BIT, true); // set to Bank Mode 1
	set_bit(&ioconData, IOCON_MIRROR_BIT, false);
	set_bit(&ioconData, IOCON_SEQOP_BIT, false);
	set_bit(&ioconData, IOCON_DISSLW_BIT, false);
	set_bit(&ioconData, IOCON_HAEN_BIT, false);
	set_bit(&ioconData, IOCON_ODR_BIT, false);
	set_bit(&ioconData, IOCON_INTPOL_BIT, false);
	return write_register(address, regist, ioconData);
}

void mcp_init() {
	i2c_init(MOD_MCP_I2C_PORT, 400 * 1000); // 400 khz
	gpio_set_function(MOD_MCP_PIN_SDA, GPIO_FUNC_I2C);
	gpio_set_function(MOD_MCP_PIN_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(MOD_MCP_PIN_SDA);
	gpio_pull_up(MOD_MCP_PIN_SCL);
	sleep_ms(10);

	setup_bank_configuration(MOD_MCP_ADDR_1, IOCONA);
	setup_bank_configuration(MOD_MCP_ADDR_1, IOCONB);

	write_register(MOD_MCP_ADDR_1, IODIRA, 0b00000000); // Set all GPA pins as outputs
	write_register(MOD_MCP_ADDR_1, IODIRB, 0b00000000); // Set all GPB pins as outputs

	write_register(MOD_MCP_ADDR_1, GPIOA, 0b11111111);
}

void mcp_all() {
	// Read the current state of both GPIOA and GPIOB in one operation
	uint8_t dataA = read_register(MOD_MCP_ADDR_1, GPIOA);
	uint8_t dataB = read_register(MOD_MCP_ADDR_1, GPIOB);

	dataA ^= 0b11111111;
	dataB ^= 0b11111111;
	write_register(MOD_MCP_ADDR_1, GPIOA, dataA);
	write_register(MOD_MCP_ADDR_1, GPIOB, dataB);
}
