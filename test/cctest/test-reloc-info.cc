// Copyright 2010 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "cctest.h"
#include "assembler.h"

namespace v8 {
namespace internal {

static void WriteRinfo(RelocInfoWriter* writer,
                       byte* pc, RelocInfo::Mode mode, intptr_t data) {
  RelocInfo rinfo(pc, mode, data);
  writer->Write(&rinfo);
}


// Tests that writing both types of positions and then reading either
// or both works as expected.
TEST(Positions) {
  const int instr_size = 10 << 10;
  const int reloc_size = 10 << 10;
  const int buf_size = instr_size + reloc_size;
  SmartPointer<byte> buf(new byte[buf_size]);
  byte* pc = *buf;
  CodeDesc desc = { *buf, buf_size, instr_size, reloc_size, NULL };

  RelocInfoWriter writer(*buf + buf_size, pc);
  for (int i = 0, pos = 0; i < 100; i++, pc += i, pos += i) {
    RelocInfo::Mode mode = (i % 2 == 0) ?
        RelocInfo::STATEMENT_POSITION : RelocInfo::POSITION;
    WriteRinfo(&writer, pc, mode, pos);
  }

  // Read only (non-statement) positions.
  {
    RelocIterator it(desc, RelocInfo::ModeMask(RelocInfo::POSITION));
    pc = *buf;
    for (int i = 0, pos = 0; i < 100; i++, pc += i, pos += i) {
      RelocInfo::Mode mode = (i % 2 == 0) ?
          RelocInfo::STATEMENT_POSITION : RelocInfo::POSITION;
      if (mode == RelocInfo::POSITION) {
        CHECK_EQ(pc, it.rinfo()->pc());
        CHECK_EQ(mode, it.rinfo()->rmode());
        CHECK_EQ(static_cast<intptr_t>(pos), it.rinfo()->data());
        it.next();
      }
    }
    CHECK(it.done());
  }

  // Read only statement positions.
  {
    RelocIterator it(desc, RelocInfo::ModeMask(RelocInfo::STATEMENT_POSITION));
    pc = *buf;
    for (int i = 0, pos = 0; i < 100; i++, pc += i, pos += i) {
      RelocInfo::Mode mode = (i % 2 == 0) ?
          RelocInfo::STATEMENT_POSITION : RelocInfo::POSITION;
      if (mode == RelocInfo::STATEMENT_POSITION) {
        CHECK_EQ(pc, it.rinfo()->pc());
        CHECK_EQ(mode, it.rinfo()->rmode());
        CHECK_EQ(static_cast<intptr_t>(pos), it.rinfo()->data());
        it.next();
      }
    }
    CHECK(it.done());
  }

  // Read both types of positions.
  {
    RelocIterator it(desc, RelocInfo::kPositionMask);
    pc = *buf;
    for (int i = 0, pos = 0; i < 100; i++, pc += i, pos += i) {
      RelocInfo::Mode mode = (i % 2 == 0) ?
          RelocInfo::STATEMENT_POSITION : RelocInfo::POSITION;
      CHECK_EQ(pc, it.rinfo()->pc());
      CHECK_EQ(mode, it.rinfo()->rmode());
      CHECK_EQ(static_cast<intptr_t>(pos), it.rinfo()->data());
      it.next();
    }
    CHECK(it.done());
  }
}

} }  // namespace v8::internal
