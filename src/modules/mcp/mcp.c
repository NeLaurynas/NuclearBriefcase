// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "mcp.h"

#include <stdint.h>
#include <stdio.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "utils.h"
#include "defines/config.h"

// MCP23017 registers (Bank Mode 1)
#define C_IODIRA	0x00 // I/O Direction Register A
#define C_IODIRB	0x01 // I/O Direction Register B
#define C_GPIOA		0x12 // GPIO Register A
#define C_GPIOB		0x13 // GPIO Register B
#define C_GPPUA		0x0C // PULL UP A
#define C_GPPUB		0x0D // PULL UP B

#define C_IOCONA			0x0A  //IO Configuration Register A - BANK/MIRROR/SLEW/INTPOL
#define C_IOCONB			0x0B  //IO Configuration Register B - BANK/MIRROR/SLEW/INTPOL
#define C_IOCON_BANK_BIT	7
#define C_IOCON_MIRROR_BIT	6
#define C_IOCON_SEQOP_BIT	5
#define C_IOCON_DISSLW_BIT	4
#define C_IOCON_HAEN_BIT	3
#define C_IOCON_ODR_BIT		2
#define C_IOCON_INTPOL_BIT	1

// cache variables
static uint8_t cache_mcp1_gpioa = 0;
static uint8_t cache_mcp1_gpiob = 0;
static uint8_t cache_mcp2_gpioa = 0;
static uint8_t cache_mcp2_gpiob = 0;
static uint32_t cache_last_mcp1_gpio = 0;
static uint32_t cache_last_mcp2_gpio = 0;

void write_register(const uint8_t address, const uint8_t regist, const uint8_t value) {
	const uint8_t data[2] = { regist, value };
	i2c_write_blocking(MOD_MCP_I2C_PORT, address, data, 2, false);
}

uint8_t read_register(const uint8_t address, const uint8_t regist) {
	uint8_t value;
	i2c_write_blocking(MOD_MCP_I2C_PORT, address, &regist, 1, true);
	i2c_read_blocking(MOD_MCP_I2C_PORT, address, &value, 1, false);
	return value;
}

uint16_t read_dual_register(const uint8_t address, const uint8_t regist) {
	uint8_t value[2] = { 0 };
	i2c_write_blocking(MOD_MCP_I2C_PORT, address, &regist, 1, true);
	i2c_read_blocking(MOD_MCP_I2C_PORT, address, value, 2, false);
	return (value[1] << 8) | value[0];
}

inline bool is_bit_set(const uint8_t value, const uint8_t bit) {
	return 0b1 & (value >> bit);
}

inline uint8_t cfg_address(const uint8_t data) {
	return is_bit_set(data, 7) ? MOD_MCP_ADDR2 : MOD_MCP_ADDR1;
}

inline uint8_t cfg_gpio_bank(const uint8_t data) {
	return is_bit_set(data, 6) ? C_GPIOB : C_GPIOA;
}

inline uint8_t cfg_iodir_bank(const uint8_t data) {
	return is_bit_set(data, 6) ? C_IODIRB : C_IODIRA;
}

inline uint8_t cfg_gppu_bank(const uint8_t data) {
	return is_bit_set(data, 6) ? C_GPPUB : C_GPPUA;
}

inline void set_bit(uint8_t* value, const uint8_t bit, const bool set) {
	if (set) {
		*value |= (1 << bit);
	} else {
		*value &= ~(1 << bit);
	}
}

inline uint8_t cfg_get_number(const uint8_t data) {
	return data & 0b00111111; // last 6 bits
}

void mcp_cfg_set_pin_out_mode(const uint8_t data, const bool is_out) {
	const auto address = cfg_address(data);
	const auto bank = cfg_iodir_bank(data);
	auto options = read_register(address, bank);
	set_bit(&options, cfg_get_number(data), !is_out);
	write_register(address, bank, options);
}

void mcp_cfg_set_pull_up(uint8_t pinData, bool pull_up) {
	const auto address = cfg_address(pinData);
	const auto bank = cfg_gppu_bank(pinData);
	auto options = read_register(address, bank);
	set_bit(&options, cfg_get_number(pinData), pull_up);
	write_register(address, bank, options);
}

void setup_bank_configuration(const uint8_t address, const uint8_t regist) {
	uint8_t ioconData = 0;
	set_bit(&ioconData, C_IOCON_BANK_BIT, false); // set to Bank Mode 0
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
	sleep_ms(1);

	setup_bank_configuration(MOD_MCP_ADDR1, C_IOCONA);
	setup_bank_configuration(MOD_MCP_ADDR1, C_IOCONB);
	setup_bank_configuration(MOD_MCP_ADDR2, C_IOCONA);
	setup_bank_configuration(MOD_MCP_ADDR2, C_IOCONB);
	sleep_ms(1);
}

void mcp_set_out(const uint8_t pinData, const bool out) {
	const auto address = cfg_address(pinData);
	const auto bank = cfg_gpio_bank(pinData);
	auto data = read_register(address, bank);
	set_bit(&data, cfg_get_number(pinData), out);
	write_register(address, bank, data);
}

bool mcp_is_pin_low(uint8_t pinData) {
	const auto address = cfg_address(pinData);
	const auto bank = cfg_gpio_bank(pinData);
	uint8_t data;
	const bool first_mcp = address == MOD_MCP_ADDR1;
	const bool possible_cache = utils_time_diff_ms(first_mcp ? cache_last_mcp1_gpio : cache_last_mcp2_gpio, time_us_32()) < MOD_MCP_GPIO_CACHE_MS;
	const bool first_bank = bank == C_GPIOA;

	if (first_mcp && possible_cache) {
		data = (first_bank ? cache_mcp1_gpioa : cache_mcp1_gpiob);
	} else if (!first_mcp && possible_cache) {
		data = (first_bank ? cache_mcp2_gpioa : cache_mcp2_gpiob);
	} else {
		const auto newData = read_dual_register(address, C_GPIOA); // read both A and B registers
		const uint8_t bank_a = newData & 0b11111111;
		const uint8_t bank_b = (newData >> 8) & 0b11111111;
		uint8_t* cache_a = first_mcp ? &cache_mcp1_gpioa : &cache_mcp2_gpioa;
		uint8_t* cache_b = first_mcp ? &cache_mcp1_gpiob : &cache_mcp2_gpiob;
		uint32_t* cache_time = first_mcp ? &cache_last_mcp1_gpio : &cache_last_mcp2_gpio;

		*cache_a = bank_a;
		*cache_b = bank_b;
		*cache_time = time_us_32();

		data = first_bank ? bank_a : bank_b;
	}
	return !is_bit_set(data, cfg_get_number(pinData));
}
