//
// Copyright (c) 2024, DefenceX PTY LTD
//
// This file is part of the MediaX project developed by DefenceX.
//
// Licensed under the Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)
// License. See the LICENSE file in the project root for full license details.
//
/// \brief A simple video receiver example
///
/// \file capture.h
///

#ifndef CAPTURE_H
#define CAPTURE_H

enum class IoMethod {
  IO_METHOD_READ,
  IO_METHOD_MMAP,
  IO_METHOD_USERPTR,
};

#endif  // CAPTURE_H
