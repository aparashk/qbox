/*
 * Copyright (c) 2023 Qualcomm Innovation Center, Inc. All Rights Reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <systemc>

#include <riscv-aclint-swi.h>

void module_register() { GSC_MODULE_REGISTER_C(riscv_aclint_swi, sc_core::sc_object*, sc_core::sc_object*); }