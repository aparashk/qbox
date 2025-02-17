/*
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All Rights Reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <systemc>

#include <qemu_xhci.h>

void module_register() { GSC_MODULE_REGISTER_C(qemu_xhci, sc_core::sc_object*, sc_core::sc_object*); }