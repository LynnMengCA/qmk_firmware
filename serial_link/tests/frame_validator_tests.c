/*
The MIT License (MIT)

Copyright (c) 2016 Fred Sundvik

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "protocol/frame_validator.c"

void route_frame(uint8_t* data, uint16_t size) {
    mock(data, size);
}

void send_frame(uint8_t* data, uint16_t size) {
    mock(data, size);
}

Describe(FrameValidator);
BeforeEach(FrameValidator) {}
AfterEach(FrameValidator) {}

Ensure(FrameValidator, doesnt_validate_frames_under_5_bytes) {
    never_expect(route_frame);
    uint8_t data[] = {1, 2};
    validator_recv_frame(0, 1);
    validator_recv_frame(data, 2);
    validator_recv_frame(data, 3);
    validator_recv_frame(data, 4);
}

Ensure(FrameValidator, validates_one_byte_frame_with_correct_crc) {
    uint8_t data[] = {0x44, 0x04, 0x6A, 0xB3, 0xA3};
    expect(route_frame,
        when(size, is_equal_to(1)),
        when(data, is_equal_to_contents_of(data, 1))
    );
    validator_recv_frame(data, 5);
}

Ensure(FrameValidator, does_not_validate_one_byte_frame_with_incorrect_crc) {
    uint8_t data[] = {0x44, 0, 0, 0, 0};
    never_expect(route_frame);
    validator_recv_frame(data, 5);
}

Ensure(FrameValidator, validates_four_byte_frame_with_correct_crc) {
    uint8_t data[] = {0x44, 0x10, 0xFF, 0x00, 0x74, 0x4E, 0x30, 0xBA};
    expect(route_frame,
        when(size, is_equal_to(4)),
        when(data, is_equal_to_contents_of(data, 4))
    );
    validator_recv_frame(data, 8);
}

Ensure(FrameValidator, validates_five_byte_frame_with_correct_crc) {
    uint8_t data[] = {1, 2, 3, 4, 5, 0xF4, 0x99, 0x0B, 0x47};
    expect(route_frame,
        when(size, is_equal_to(5)),
        when(data, is_equal_to_contents_of(data, 5))
    );
    validator_recv_frame(data, 9);
}

Ensure(FrameValidator, sends_one_byte_with_correct_crc) {
    uint8_t original[] = {0x44, 0, 0, 0, 0};
    uint8_t expected[] = {0x44, 0x04, 0x6A, 0xB3, 0xA3};
    expect(send_frame,
        when(size, is_equal_to(sizeof(expected))),
        when(data, is_equal_to_contents_of(expected, sizeof(expected)))
    );
    validator_send_frame(original, 1);
}

Ensure(FrameValidator, sends_five_bytes_with_correct_crc) {
    uint8_t original[] = {1, 2, 3, 4, 5, 0, 0, 0, 0};
    uint8_t expected[] = {1, 2, 3, 4, 5, 0xF4, 0x99, 0x0B, 0x47};
    expect(send_frame,
        when(size, is_equal_to(sizeof(expected))),
        when(data, is_equal_to_contents_of(expected, sizeof(expected)))
    );
    validator_send_frame(original, 5);
}
