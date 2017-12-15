/*
 * Copyright (C) 2015 Yusuke Suzuki <utatane.tea@gmail.com>.
 * Copyright (C) 2016 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "BytecodeIntrinsicRegistry.h"

#include "CommonIdentifiers.h"
#include "Nodes.h"

//+EAWebKitChange
//04/14/2016 added some validation regarding sizeof member function pointers, which MSVC can get wrong
//issue is described here https://social.msdn.microsoft.com/Forums/vstudio/en-US/72343dd4-3a43-46e8-889e-73dc4d8e9432/discrepancy-with-sizeof-in-template?forum=vcgeneral
#if defined(EA_PLATFORM_WIN64)
//pc64 these are the values if we have a proper definition of the member function pointer JSC::RegisterID * (__cdecl JSC::BytecodeIntrinsicNode::*)(JSC::BytecodeGenerator &, JSC::RegisterID *)
//static_assert(sizeof(WTF::KeyValuePair<WTF::RefPtr<WTF::UniquedStringImpl>, JSC::RegisterID * (__cdecl JSC::BytecodeIntrinsicNode::*)(JSC::BytecodeGenerator &, JSC::RegisterID *)>) == 24, "Size not expected");
//static_assert(sizeof(WTF::RefPtr<WTF::UniquedStringImpl>) == 8, "Size not expected");
//static_assert(sizeof(JSC::RegisterID * (__cdecl JSC::BytecodeIntrinsicNode::*)(JSC::BytecodeGenerator &, JSC::RegisterID *)) == 16, "Size not expected");

//pc64 these are the values we will have if compiling with /vmg which is the current plan
static_assert(sizeof(WTF::KeyValuePair<WTF::RefPtr<WTF::UniquedStringImpl>, JSC::RegisterID * (__cdecl JSC::BytecodeIntrinsicNode::*)(JSC::BytecodeGenerator &, JSC::RegisterID *)>) == 32, "Size not expected");
static_assert(sizeof(WTF::RefPtr<WTF::UniquedStringImpl>) == 8, "Size not expected");
static_assert(sizeof(JSC::RegisterID * (__cdecl JSC::BytecodeIntrinsicNode::*)(JSC::BytecodeGenerator &, JSC::RegisterID *)) == 24, "Size not expected");
#endif
//-EAWebKitChange

namespace JSC {

#define INITIALISE_BYTECODE_INTRINSIC_NAMES_TO_SET(name) m_bytecodeIntrinsicMap.add(propertyNames.name##PrivateName.impl(), &BytecodeIntrinsicNode::emit_intrinsic_##name);

BytecodeIntrinsicRegistry::BytecodeIntrinsicRegistry(const CommonIdentifiers& propertyNames)
    : m_propertyNames(propertyNames)
    , m_bytecodeIntrinsicMap()
{
    JSC_COMMON_BYTECODE_INTRINSICS_EACH_NAME(INITIALISE_BYTECODE_INTRINSIC_NAMES_TO_SET)
}

BytecodeIntrinsicNode::EmitterType BytecodeIntrinsicRegistry::lookup(const Identifier& ident) const
{
    if (!m_propertyNames.isPrivateName(ident))
        return nullptr;
    auto iterator = m_bytecodeIntrinsicMap.find(ident.impl());
    if (iterator == m_bytecodeIntrinsicMap.end())
        return nullptr;
    return iterator->value;
}

} // namespace JSC

