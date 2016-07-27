/*
Copyright (C) 2012 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
its contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "config.h"
#include "TransformationMatrix.h"

#include <EAWebKit/EAWebkitSystem.h>

namespace WebCore {

    TransformationMatrix::operator EA::WebKit::TransformationMatrix(void) const
    {
        EA::WebKit::TransformationMatrix ea;
        
        ea.mComponents[0][0] = static_cast<float>(m_matrix[0][0]);
        ea.mComponents[0][1] = static_cast<float>(m_matrix[0][1]);
        ea.mComponents[0][2] = static_cast<float>(m_matrix[0][2]);
        ea.mComponents[0][3] = static_cast<float>(m_matrix[0][3]);

        ea.mComponents[1][0] = static_cast<float>(m_matrix[1][0]);
        ea.mComponents[1][1] = static_cast<float>(m_matrix[1][1]);
        ea.mComponents[1][2] = static_cast<float>(m_matrix[1][2]);
        ea.mComponents[1][3] = static_cast<float>(m_matrix[1][3]);

        ea.mComponents[2][0] = static_cast<float>(m_matrix[2][0]);
        ea.mComponents[2][1] = static_cast<float>(m_matrix[2][1]);
        ea.mComponents[2][2] = static_cast<float>(m_matrix[2][2]);
        ea.mComponents[2][3] = static_cast<float>(m_matrix[2][3]);

        ea.mComponents[3][0] = static_cast<float>(m_matrix[3][0]);
        ea.mComponents[3][1] = static_cast<float>(m_matrix[3][1]);
        ea.mComponents[3][2] = static_cast<float>(m_matrix[3][2]);
        ea.mComponents[3][3] = static_cast<float>(m_matrix[3][3]);


        return ea;
    }
}
