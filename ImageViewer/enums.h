#pragma once

enum ImageType {UNCOMPRESSED = 1, GRAYSCALE, LEGACY, RLE, CHUNK};
enum class SectionType {DICTIONARY, UNIQUE};
enum ChunkType {FULL_CHUNK_ALL_DICT, DICT_MAJORITY, FULL_CHUNK_ALL_UNIQUE, UNIQUE_MAJORITY};