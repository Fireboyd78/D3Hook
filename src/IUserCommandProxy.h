#pragma once
#include "hook.h"

typedef struct IUserCommand {
public:
	virtual void ProcessCommand(const char *command, const char *parameters) PURE;
} *LPUserCommand;

typedef struct IUserCommandProxy {
public:
    virtual int Release(bool) PURE;
    virtual void Initialise(void) PURE;
    virtual void RegisterCommand(LPUserCommand) PURE;
    virtual void UnregisterCommand(LPUserCommand) PURE;
    virtual void IssueCommand(const char *, LPUserCommand) PURE;
} *LPUserCommandProxy;

BEGIN_HOOK_TABLE( IUserCommandProxyHook,
			      IUserCommandProxy )

    int IUserCommandProxy::Release(bool free) {
        //LOG("[IUserCommandProxy::Release]");
        return HOOK_PTR->Release(free);
    };

    void IUserCommandProxy::Initialise(void) {
        //LOG("[IUserCommandProxy::Initialise]");
        HOOK_PTR->Initialise();
    };

    void IUserCommandProxy::RegisterCommand(LPUserCommand userCmd) {
        //DWORD retAddr = *(DWORD*)(&userCmd - 1);
        //dprintf("[IUserCommandProxy::RegisterCommand] (return:0x%X)", retAddr);
        HOOK_PTR->RegisterCommand(userCmd);
    };

    void IUserCommandProxy::UnregisterCommand(LPUserCommand userCmd) {
        //DWORD retAddr = *(DWORD*)(&userCmd - 1);
        //dprintf("[IUserCommandProxy::UnegisterCommand] (return:0x%X)", retAddr);
        HOOK_PTR->UnregisterCommand(userCmd);
    };

    void IUserCommandProxy::IssueCommand(const char *command, LPUserCommand userCmd) {
        //if (strlen(command) > 0) {
        //    DWORD retAddr = *(DWORD*)(&command - 1);
        //    debugf("[IUserCommandProxy::IssueCommand] -> \"%s\", 0x%X (return:0x%X)\n", command, userCmd, retAddr);
        //}
        HOOK_PTR->IssueCommand(command, userCmd);
    };

END_HOOK_TABLE()