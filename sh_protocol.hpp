#pragma once

#include "sh_color.hpp"
#include <cstdint>
#include <string>
#include <vector>

// Ce fichier contient tout ce qui va être lié au protocole du jeu, à la façon dont le client et le serveur vont communiquer

enum class Opcode : std::uint8_t
{
	C_UpdateDirection,
	S_GameState,
	S_GridState,
	S_GridUpdate
};

void Serialize_color(std::vector<std::uint8_t>& byteArray, const Color& value);
void Serialize_i8(std::vector<std::uint8_t>& byteArray, std::int8_t value);
void Serialize_i8(std::vector<std::uint8_t>& byteArray, std::size_t offset, std::int8_t value);
void Serialize_i16(std::vector<std::uint8_t>& byteArray, std::int16_t value);
void Serialize_i16(std::vector<std::uint8_t>& byteArray, std::size_t offset, std::int16_t value);
void Serialize_i32(std::vector<std::uint8_t>& byteArray, std::int32_t value); 
void Serialize_i32(std::vector<std::uint8_t>& byteArray, std::size_t offset, std::int32_t value);
void Serialize_u8(std::vector<std::uint8_t>& byteArray, std::uint8_t value);
void Serialize_u8(std::vector<std::uint8_t>& byteArray, std::size_t offset, std::uint8_t value);
void Serialize_u16(std::vector<std::uint8_t>& byteArray, std::uint16_t value);
void Serialize_u16(std::vector<std::uint8_t>& byteArray, std::size_t offset, std::uint16_t value);
void Serialize_u32(std::vector<std::uint8_t>& byteArray, std::uint32_t value);
void Serialize_u32(std::vector<std::uint8_t>& byteArray, std::size_t offset, std::uint32_t value);
void Serialize_str(std::vector<std::uint8_t>& byteArray, const std::string& value);
void Serialize_str(std::vector<std::uint8_t>& byteArray, std::size_t offset, const std::string& value);

Color Unserialize_color(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
std::int8_t Unserialize_i8(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
std::int16_t Unserialize_i16(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
std::int32_t Unserialize_i32(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
std::uint8_t Unserialize_u8(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
std::uint16_t Unserialize_u16(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
std::uint32_t Unserialize_u32(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
std::string Unserialize_str(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);