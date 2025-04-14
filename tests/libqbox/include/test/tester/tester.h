/*
 * This file is part of libqbox
 * Copyright (c) 2022-2023 Qualcomm Innovation Center, Inc. All Rights Reserved.
 * Author: GreenSocs 2021
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TESTS_INCLUDE_TEST_TESTER_TESTER_H
#define TESTS_INCLUDE_TEST_TESTER_TESTER_H

#include <cstring>

#include <systemc>
#include <tlm>

#include <ports/initiator-signal-socket.h>

#include "test/test.h"
#include <tlm_sockets_buswidth.h>

class CpuTesterCallbackIface
{
public:
    virtual ~CpuTesterCallbackIface() {}

    virtual void map_target(tlm::tlm_target_socket<DEFAULT_TLM_BUSWIDTH>& s, uint64_t addr, uint64_t size) = 0;
    virtual void map_irqs_to_cpus(sc_core::sc_vector<InitiatorSignalSocket<bool> >& irqs) = 0;

    virtual uint64_t mmio_read(int id, uint64_t addr, size_t len) = 0;
    virtual void mmio_write(int id, uint64_t addr, uint64_t data, size_t len) = 0;

    virtual bool dmi_request(int id, uint64_t addr, size_t len, tlm::tlm_dmi& ret) = 0;

    virtual void dmi_request_failed(int id, uint64_t addr, size_t len, tlm::tlm_dmi& ret){};
};

class CpuTester : public sc_core::sc_module
{
public:
    static constexpr size_t MMIO_SIZE = 1024;

    using TargetSocket = tlm_utils::simple_target_socket_tagged<CpuTester, DEFAULT_TLM_BUSWIDTH>;

protected:
    CpuTesterCallbackIface& m_cbs;

    void b_transport(int id, tlm::tlm_generic_payload& txn, sc_core::sc_time& delay)
    {
        uint64_t addr = txn.get_address();
        uint64_t data = 0;
        uint64_t* ptr = reinterpret_cast<uint64_t*>(txn.get_data_ptr());
        size_t len = txn.get_data_length();

        if (len > 8) {
            TEST_FAIL("Unsupported b_transport data length");
        }

        switch (txn.get_command()) {
        case tlm::TLM_READ_COMMAND:
            data = m_cbs.mmio_read(id, addr, len);
            std::memcpy(ptr, &data, len);
            break;

        case tlm::TLM_WRITE_COMMAND:
            std::memcpy(&data, ptr, len);
            m_cbs.mmio_write(id, addr, *ptr, len);
            break;

        default:
            TEST_FAIL("TLM command not supported");
        }

        txn.set_response_status(tlm::TLM_OK_RESPONSE);
    }

    void register_b_transport(TargetSocket& socket, int id)
    {
        socket.register_b_transport(this, &CpuTester::b_transport, id);
    }

public:
    CpuTester(const sc_core::sc_module_name& n, CpuTesterCallbackIface& cbs): sc_core::sc_module(n), m_cbs(cbs) {}
};

#endif
