//
// Copyright (C) 2023 DefenceX PTY LTD. All rights reserved.
//
// This software is distributed under the included copyright license.
// Any redistribution or reproduction, in part or in whole, in any form or medium, is strictly prohibited without the
// prior written consent of DefenceX PTY LTD.
//
// For any inquiries or concerns, please contact:
// DefenceX PTY LTD
// Email: enquiries@defencex.ai
//
///
/// \file pngget.h

#ifndef PNGGET_H
#define PNGGET_H

#include <png.h>
#include <stdint.h>

#include <string>
#include <vector>

png_bytep* get_row_pointwes();
void read_png_file(char* file_name);
std::vector<uint8_t> read_png_rgb24(std::string_view filename);
#endif
