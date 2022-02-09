/*
 *  Copyright (c) 2021, RedNodeLabs.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements a simple CLI for the RNL RedNodeBus service.
 */

#include "cli_rnl_rnb.hpp"

#include "cli/cli.hpp"

namespace ot {
namespace Cli {

constexpr RnlRnb::Command RnlRnb::sCommands[];

RnlRnb::RnlRnb(Output &aOutput)
    : OutputWrapper(aOutput)
{

}

otError RnlRnb::ProcessHelp(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    for (const Command &command : sCommands)
    {
        OutputLine(command.mName);
    }

    return OT_ERROR_NONE;
}

otError RnlRnb::ProcessVersion(Arg aArgs[])
{
    otError error;

    if (aArgs[0].IsEmpty())
    {
        OutputLine("%s", otPlatRadioRnlRnbGetVersion(GetInstancePtr()));

        error = OT_ERROR_NONE;
    }
    else
    {
        error = OT_ERROR_INVALID_ARGS;
    }

    return error;
}

otError RnlRnb::ProcessSendRequest(Arg aArgs[])
{
    otError error = OT_ERROR_INVALID_ARGS;
    uint16_t rnbRequestLength;
    otRadioRnlRnbRequest *rnbRequest;

    VerifyOrExit(!aArgs[0].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(!aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    if (aArgs[2].IsEmpty())
    {
        SuccessOrExit(error = aArgs[0].ParseAsUint16(rnbRequestLength));

        VerifyOrExit(rnbRequestLength > 0, error = OT_ERROR_INVALID_ARGS);
        VerifyOrExit(rnbRequestLength <= OT_RADIO_RNL_RNB_REQUEST_MAX_SIZE, error = OT_ERROR_INVALID_ARGS);

        rnbRequest = (otRadioRnlRnbRequest*) mReceiveBuffer;

        SuccessOrExit(error = aArgs[1].ParseAsHexString(rnbRequest->mRnbRequest, rnbRequestLength));

        error = otPlatRadioRnlRnbSendRequest(GetInstancePtr(), rnbRequest, rnbRequestLength);
    }

exit:
    return error;
}

otError RnlRnb::Process(Arg aArgs[])
{
    otError        error = OT_ERROR_INVALID_ARGS;
    const Command *command;

    if (aArgs[0].IsEmpty())
    {
        IgnoreError(ProcessHelp(aArgs));
        ExitNow();
    }

    command = Utils::LookupTable::Find(aArgs[0].GetCString(), sCommands);
    VerifyOrExit(command != nullptr, error = OT_ERROR_INVALID_COMMAND);

    error = (this->*command->mHandler)(aArgs + 1);

exit:
    return error;
}

} // namespace Cli
} // namespace ot
