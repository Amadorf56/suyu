// Copyright 2021 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included

#pragma once

#include "common/common_types.h"
#include "common/swap.h"

namespace Service::HID {
constexpr std::size_t max_buffer_size = 17;

template <typename State>
struct AtomicStorage {
    s64 sampling_number;
    State state;
};

template <typename State>
struct Lifo {
    s64 timestamp{};
    s64 total_buffer_count = max_buffer_size;
    s64 buffer_tail{};
    s64 buffer_count{};
    std::array<AtomicStorage<State>, max_buffer_size> entries{};

    const AtomicStorage<State>& ReadCurrentEntry() const {
        return entries[buffer_tail];
    }

    const AtomicStorage<State>& ReadPreviousEntry() const {
        return entries[GetPreviuousEntryIndex()];
    }

    std::size_t GetPreviuousEntryIndex() const {
        return (buffer_tail + total_buffer_count - 1) % total_buffer_count;
    }

    std::size_t GetNextEntryIndex() const {
        return (buffer_tail + 1) % total_buffer_count;
    }

    void WriteNextEntry(const State& new_state) {
        if (buffer_count < total_buffer_count - 1) {
            buffer_count++;
        }
        buffer_tail = GetNextEntryIndex();
        const auto& previous_entry = ReadPreviousEntry();
        entries[buffer_tail].sampling_number = previous_entry.sampling_number + 1;
        entries[buffer_tail].state = new_state;
    }
};

} // namespace Service::HID
