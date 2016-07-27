/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Google Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(MEDIA_STREAM)

#include "RTCSessionDescription.h"

#include "Dictionary.h"
#include "ExceptionCode.h"
#include "RTCSessionDescriptionDescriptor.h"

namespace WebCore {

static bool verifyType(const String& type)
{
    return type == "offer" || type == "pranswer" || type == "answer";
}

PassRefPtr<RTCSessionDescription> RTCSessionDescription::create(const Dictionary& dictionary, ExceptionCode& ec)
{
    String type;
    bool ok = dictionary.get("type", type);
    if (!ok || !verifyType(type)) {
        ec = TYPE_MISMATCH_ERR;
        return 0;
    }

    String sdp;
    ok = dictionary.get("sdp", sdp);
    if (!ok || sdp.isEmpty()) {
        ec = TYPE_MISMATCH_ERR;
        return 0;
    }

    return adoptRef(new RTCSessionDescription(RTCSessionDescriptionDescriptor::create(type, sdp)));
}

PassRefPtr<RTCSessionDescription> RTCSessionDescription::create(PassRefPtr<RTCSessionDescriptionDescriptor> descriptor)
{
    ASSERT(descriptor);
    return adoptRef(new RTCSessionDescription(descriptor));
}

RTCSessionDescription::RTCSessionDescription(PassRefPtr<RTCSessionDescriptionDescriptor> descriptor)
    : m_descriptor(descriptor)
{
}

RTCSessionDescription::~RTCSessionDescription()
{
}

const String& RTCSessionDescription::type() const
{
    return m_descriptor->type();
}

void RTCSessionDescription::setType(const String& type, ExceptionCode& ec)
{
    if (verifyType(type))
        m_descriptor->setType(type);
    else
        ec = TYPE_MISMATCH_ERR;
}

const String& RTCSessionDescription::sdp() const
{
    return m_descriptor->sdp();
}

void RTCSessionDescription::setSdp(const String& sdp)
{
    m_descriptor->setSdp(sdp);
}

RTCSessionDescriptionDescriptor* RTCSessionDescription::descriptor()
{
    return m_descriptor.get();
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)
