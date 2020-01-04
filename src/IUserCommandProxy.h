#pragma once
#include "hook.h"

class IUserCommand {
public:
	virtual void ProcessCommand(const char *command, const char *parameters) PURE;
};

class IUserCommandProxy {
public:
    virtual int ˜dtor(bool) PURE;

    virtual void Initialise(void) PURE;
    virtual void RegisterCommand(IUserCommand *) PURE;
    virtual void UnregisterCommand(IUserCommand *) PURE;
    virtual void IssueCommand(const char *, IUserCommand *) PURE;
};

class IUserCommandProxyHook : public AutoHook::Type<IUserCommandProxy> {
public:
    int IUserCommandProxy::˜dtor(bool free) {
#ifdef USERCMDPROXY_DEBUGGING
        LOG("[IUserCommandProxy::Release]");
#endif
        return pBase->˜dtor(free);
    };

    void IUserCommandProxy::Initialise(void) {
#ifdef USERCMDPROXY_DEBUGGING
        LOG("[IUserCommandProxy::Initialise]");
#endif
        pBase->Initialise();
    };

    void IUserCommandProxy::RegisterCommand(IUserCommand *userCmd) {
#ifdef USERCMDPROXY_DEBUGGING
        DWORD retAddr = *(DWORD*)(&userCmd - 1);
        LogFile::Format("[IUserCommandProxy::RegisterCommand] (return:0x%X)\n", retAddr);
#endif
        pBase->RegisterCommand(userCmd);
    };

    void IUserCommandProxy::UnregisterCommand(IUserCommand *userCmd) {
#ifdef USERCMDPROXY_DEBUGGING
        DWORD retAddr = *(DWORD*)(&userCmd - 1);
        LogFile::Format("[IUserCommandProxy::UnegisterCommand] (return:0x%X)\n", retAddr);
#endif
        pBase->UnregisterCommand(userCmd);
    };

    void IUserCommandProxy::IssueCommand(const char *command, IUserCommand *userCmd) {
#ifdef USERCMDPROXY_DEBUGGING
        if (strlen(command) > 0) {
            DWORD retAddr = *(DWORD*)(&command - 1);
            debugf("[IUserCommandProxy::IssueCommand] -> \"%s\", 0x%X (return:0x%X)\n", command, userCmd, retAddr);
        }
#endif
        pBase->IssueCommand(command, userCmd);
    };
};

IUserCommandProxy * AutoHook::Type<IUserCommandProxy>::pBase;