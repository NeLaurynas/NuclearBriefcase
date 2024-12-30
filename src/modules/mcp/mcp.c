// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "mcp.h"

#include <stdio.h>
#include <stdint.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "defines/config.h"

// MCP23017 registers
#define IODIRA 0x00  // I/O Direction Register A
#define IODIRB 0x01  // I/O Direction Register B
#define GPIOA  0x12  // GPIO Register A
#define GPIOB  0x13  // GPIO Register B
// #define OLATA  0x14  // Output Latch Register A
// #define OLATB  0x15  // Output Latch Register B

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

void write_both_register(uint8_t address, uint8_t regist, uint16_t value) {
	const uint8_t data[3] = {regist, (uint8_t)value, (uint8_t)(value >> 8)};
	i2c_write_blocking(MOD_MCP_I2C_PORT, address, data, 3, false);
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
	set_bit(&ioconData, IOCON_BANK_BIT, false);
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

	setup_bank_configuration(MOD_MCP_ADDR_1, IOCONA);
	setup_bank_configuration(MOD_MCP_ADDR_1, IOCONB);

	write_register(MOD_MCP_ADDR_1, IODIRA, 0x00); // Set all GPA pins as outputs
	write_register(MOD_MCP_ADDR_1, IODIRB, 0x00); // Set all GPB pins as outputs

	write_register(MOD_MCP_ADDR_1, GPIOA, 0b11111111);
}

void display_bytes_as_binary(const void* data, size_t size) {
	const unsigned char* bytes = (const unsigned char*)data;
	for (size_t i = 0; i < size; i++) {
		for (int bit = 7; bit >= 0; bit--) { // Iterate through bits from MSB to LSB
			printf("%d", (bytes[i] >> bit) & 1);
		}
		printf(" "); // Separate each byte
	}
	printf("\n");
}

void mcp_all() {
	// Read the current state of both GPIOA and GPIOB in one operation
	uint8_t dataA = read_register(MOD_MCP_ADDR_1, GPIOA);
	uint8_t dataB = read_register(MOD_MCP_ADDR_1, GPIOB);

	display_bytes_as_binary(&dataA, sizeof(uint8_t));
	display_bytes_as_binary(&dataB, sizeof(uint8_t));

	// i2c_scan();

	// Toggle the corresponding bit (0 to 15)
	// dataA ^= 0b11111111; // Toggle bit i for GPIOA (0 to 7) and GPIOB (8 to 15) // XOR - 1 ^ 1 = 0, 0 ^ 1 = 1
	// dataB ^= 0b11111111;
	dataA = 0b01000000;
	dataB = 0b00000010;
	// write_register(MOD_MCP_ADDR_1, OLATA, data); // Write back the toggled value
	write_register(MOD_MCP_ADDR_1, GPIOA, dataA);
	write_register(MOD_MCP_ADDR_1, GPIOB, dataB);

	// printf("Toggled GPIO%d\n", i);
}
