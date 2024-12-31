// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "mcp.h"

#include <stdint.h>
#include <stdio.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "defines/config.h"

// MCP23017 registers (Bank Mode 1)
#define C_IODIRA 0x00  // I/O Direction Register A
#define C_IODIRB 0x10  // I/O Direction Register B
#define C_GPIOA  0x09  // GPIO Register A
#define C_GPIOB  0x19  // GPIO Register B

#define C_IOCONA 0x0A  //IO Configuration Register A - BANK/MIRROR/SLEW/INTPOL
#define C_IOCONB 0x0B  //IO Configuration Register B - BANK/MIRROR/SLEW/INTPOL
#define C_IOCON_BANK_BIT 7
#define C_IOCON_MIRROR_BIT 6
#define C_IOCON_SEQOP_BIT 5
#define C_IOCON_DISSLW_BIT 4
#define C_IOCON_HAEN_BIT 3
#define C_IOCON_ODR_BIT 2
#define C_IOCON_INTPOL_BIT 1

void write_register(uint8_t address, uint8_t regist, uint8_t value) {
	const uint8_t data[2] = { regist, value };
	i2c_write_blocking(MOD_MCP_I2C_PORT, address, data, 2, false);
}

uint8_t read_register(uint8_t address, uint8_t regist) {
	uint8_t value;
	i2c_write_blocking(MOD_MCP_I2C_PORT, address, &regist, 1, true);
	i2c_read_blocking(MOD_MCP_I2C_PORT, address, &value, 1, false);
	return value;
}

inline bool is_bit_set(uint8_t value, uint8_t bit) {
	return 0b1 & (value >> bit);
}

inline uint8_t cfg_address(uint8_t data) {
	return is_bit_set(data, 7) ? MOD_MCP_ADDR_2 : MOD_MCP_ADDR_1;
}

inline uint8_t cfg_gpio_bank(uint8_t data) {
	return is_bit_set(data, 6) ? C_GPIOB : C_GPIOA;
}

inline uint8_t cfg_iodir_bank(uint8_t data) {
	return is_bit_set(data, 6) ? C_IODIRB : C_IODIRA;
}

inline void set_bit(uint8_t* value, uint8_t bit, bool set) {
	if (set) {
		*value |= (1 << bit);
	}
	else {
		*value &= ~(1 << bit);
	}
}

inline uint8_t cfg_get_number(uint8_t data) {
	return data & 0b00111111; // last 6 bits
}

void set_pin_out(uint8_t data) {
	uint8_t options = read_register(cfg_address(data), cfg_iodir_bank(data));
	set_bit(&options, cfg_get_number(data), false);
	write_register(cfg_address(data), cfg_iodir_bank(data), options); // Set all GPA pins as outputs
}

void setup_bank_configuration(uint8_t address, uint8_t regist) {
	uint8_t ioconData = 0;
	set_bit(&ioconData, C_IOCON_BANK_BIT, true); // set to Bank Mode 1
	set_bit(&ioconData, C_IOCON_MIRROR_BIT, false);
	set_bit(&ioconData, C_IOCON_SEQOP_BIT, false);
	set_bit(&ioconData, C_IOCON_DISSLW_BIT, false);
	set_bit(&ioconData, C_IOCON_HAEN_BIT, false);
	set_bit(&ioconData, C_IOCON_ODR_BIT, false);
	set_bit(&ioconData, C_IOCON_INTPOL_BIT, false);
	write_register(address, regist, ioconData);
}

void mcp_init() {
	i2c_init(MOD_MCP_I2C_PORT, 400'000); // 400 khz
	gpio_set_function(MOD_MCP_PIN_SDA, GPIO_FUNC_I2C);
	gpio_set_function(MOD_MCP_PIN_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(MOD_MCP_PIN_SDA);
	gpio_pull_up(MOD_MCP_PIN_SCL);
	sleep_ms(10);

	setup_bank_configuration(MOD_MCP_ADDR_1, C_IOCONA);
	setup_bank_configuration(MOD_MCP_ADDR_1, C_IOCONB);

	set_pin_out(MOD_NUM_LED_G);
	set_pin_out(MOD_NUM_LED_R);
}

void mcp_all() {
	uint8_t dataB = read_register(MOD_MCP_ADDR_1, C_GPIOB);
	dataB ^= 0b11111111;
	write_register(MOD_MCP_ADDR_1, C_GPIOB, dataB);
}
