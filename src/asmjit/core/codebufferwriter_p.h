// [AsmJit]
// Complete x86/x64 JIT and Remote Assembler for C++.
//
// [License]
// ZLIB - See LICENSE.md file in the package.

// [Guard]
#ifndef _ASMJIT_CORE_CODEBUFFERWRITER_P_H
#define _ASMJIT_CORE_CODEBUFFERWRITER_P_H

// [Dependencies]
#include "../core/assembler.h"
#include "../core/support.h"

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_core_api
//! \{

// ============================================================================
// [asmjit::CodeBufferWriter]
// ============================================================================

//! \internal
//!
//! Helper that is used to write into a `CodeBuffer` held by `BaseAssembler`.
class CodeBufferWriter {
public:
  ASMJIT_INLINE explicit CodeBufferWriter(BaseAssembler* a) noexcept
    : _cursor(a->_bufferPtr) {}

  ASMJIT_INLINE Error ensureSpace(BaseAssembler* a, size_t n) noexcept {
    size_t remainingSpace = (size_t)(a->_bufferEnd - _cursor);
    if (ASMJIT_UNLIKELY(remainingSpace < n)) {
      CodeBuffer& buffer = a->_section->_buffer;
      Error err = a->_code->growBuffer(&buffer, n);
      if (ASMJIT_UNLIKELY(err))
        return a->reportError(err);
      _cursor = a->_bufferPtr;
    }
    return kErrorOk;
  }

  ASMJIT_INLINE uint8_t* cursor() const noexcept {
    return _cursor;
  }

  ASMJIT_INLINE size_t offset(uint8_t* from) const noexcept {
    ASMJIT_ASSERT(_cursor >= from);
    return (size_t)(_cursor - from);
  }

  ASMJIT_INLINE void advance(size_t n) noexcept {
    _cursor += n;
  }

  template<typename T>
  ASMJIT_INLINE void emit8(T val) noexcept {
    typedef typename std::make_unsigned<T>::type U;
    _cursor[0] = uint8_t(U(val) & U(0xFF));
    _cursor++;
  }

  template<typename T, typename Y>
  ASMJIT_INLINE void emit8If(T val, Y cond) noexcept {
    typedef typename std::make_unsigned<T>::type U;
    ASMJIT_ASSERT(size_t(cond) <= 1u);

    _cursor[0] = uint8_t(U(val) & U(0xFF));
    _cursor += size_t(cond);
  }

  template<typename T>
  ASMJIT_INLINE void emit16uLE(T val) noexcept {
    typedef typename std::make_unsigned<T>::type U;
    Support::writeU16uLE(_cursor, uint32_t(U(val) & 0xFFFFu));
    _cursor += 2;
  }

  template<typename T>
  ASMJIT_INLINE void emit16uBE(T val) noexcept {
    typedef typename std::make_unsigned<T>::type U;
    Support::writeU16uBE(_cursor, uint32_t(U(val) & 0xFFFFu));
    _cursor += 2;
  }

  template<typename T>
  ASMJIT_INLINE void emit32uLE(T val) noexcept {
    typedef typename std::make_unsigned<T>::type U;
    Support::writeU32uLE(_cursor, uint32_t(U(val) & 0xFFFFFFFFu));
    _cursor += 4;
  }

  template<typename T>
  ASMJIT_INLINE void emit32uBE(T val) noexcept {
    typedef typename std::make_unsigned<T>::type U;
    Support::writeU32uBE(_cursor, uint32_t(U(val) & 0xFFFFFFFFu));
    _cursor += 4;
  }

  ASMJIT_INLINE void emitData(const void* data, size_t size) noexcept {
    ASMJIT_ASSERT(size != 0);
    ::memcpy(_cursor, data, size);
    _cursor += size;
  }

  ASMJIT_INLINE void emitZeros(size_t size) noexcept {
    ASMJIT_ASSERT(size != 0);
    ::memset(_cursor, 0, size);
    _cursor += size;
  }

  ASMJIT_INLINE void done(BaseAssembler* a) noexcept {
    CodeBuffer& buffer = a->_section->_buffer;
    size_t newSize = (size_t)(_cursor - a->_bufferData);
    ASMJIT_ASSERT(newSize <= buffer.capacity());

    a->_bufferPtr = _cursor;
    buffer._size = Support::max(buffer._size, newSize);
  }

  uint8_t* _cursor;
};

//! \}

ASMJIT_END_NAMESPACE

// [Guard]
#endif // _ASMJIT_CORE_CODEBUFFERWRITER_P_H